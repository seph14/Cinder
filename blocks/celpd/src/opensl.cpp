#include "celpd.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "z_libpd.h"

using namespace ci;
using namespace cel;
using namespace cel::pd;

using std::shared_ptr;
using std::thread;
using std::unique_lock;
using std::mutex;

const int      kTicksPerBuffer = 1;
const uint32_t kBufferSamples  = 1024;  // must be a multiple of libpd block size (ie 64)

SLuint32 slSampleRate(int sampleRate)
{
    int slrate = -1;

    switch (sampleRate) {
        case 8000:
            slrate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            slrate = SL_SAMPLINGRATE_11_025;
            break;
        case 16000:
            slrate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            slrate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            slrate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            slrate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            slrate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            slrate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            slrate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            slrate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            slrate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            slrate = SL_SAMPLINGRATE_192;
            break;
        default:
            break;
    }

    return slrate;
}


class OpenSL : public PdAudio
{
  public:
    OpenSL(Pd& pd, int inChannels, int outChannels, int sampleRate) 
        : mPd(pd), mEngineObject(NULL), mOutputMixObject(NULL), bqPlayerObject(NULL), 
          mPlayerRunning(false), mOutputReady(false), mOutputBufIndex(0), mInputBufIndex(0), 
          mInputChannels(inChannels), mOutputChannels(outChannels)
    {
        initSL(inChannels, outChannels, sampleRate);
        libpd_init_audio(inChannels, outChannels, sampleRate);
        mOutputBufSamples = kBufferSamples * outChannels;
        mInputBufSamples  = kBufferSamples * inChannels;
        CI_LOGD("OSL: Allocating buffers size kTicks (%d) * blocksize (%d) * channels (%d) = %d",
                kTicksPerBuffer, libpd_blocksize(), outChannels, mOutputBufSamples);

        for (int i=0; i < 2; ++i) {
            mOutputBuf[i] = new int16_t[mOutputBufSamples];
            mInputBuf[i]  = new int16_t[mInputBufSamples];
        }
    }


    //!  Start the audio system playing
    void play()
    {
        // set the player's state to playing
        CI_LOGD("Pd: play()");
        SLresult result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
        assert(SL_RESULT_SUCCESS == result);

        //  Start playback by queuing an initial buffer (empty)
        {
            mInputReady    = true;
            mOutputReady   = true;
            mPlayerRunning = true;
            for (int i=0; i < 2; ++i) {
                memset(mOutputBuf[i], 0, sizeof(int16_t) * mOutputBufSamples);
                memset(mInputBuf[i],  0, sizeof(int16_t) * mInputBufSamples);
            }
            mMixerThread   = shared_ptr<thread>(new thread(&OpenSL::playerLoop, this));
        }
        mOutputBufReady.notify_one();
        if (mInputChannels > 0)
            mInputBufReady.notify_one();
    }

    //!  Pause the audio system
    void pause()
    {
        CI_LOGD("Pd: pause()");
        {
            unique_lock<mutex> lock(mPlayerLock);
            mPlayerRunning = false;
            mMixerThread->interrupt();
        }
        SLresult result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PAUSED);
        assert(SL_RESULT_SUCCESS == result);
    }

    //!  Shut down audio system
    void close()
    {
        // destroy buffer queue audio player object, and invalidate all associated interfaces
        if (bqPlayerObject != NULL) {
            (*bqPlayerObject)->Destroy(bqPlayerObject);
            bqPlayerObject = NULL;
            bqPlayerPlay = NULL;
            bqPlayerBufferQueue = NULL;
        }

        // destroy output mix object, and invalidate all associated interfaces
        if (mOutputMixObject != NULL) {
            (*mOutputMixObject)->Destroy(mOutputMixObject);
            mOutputMixObject = NULL;
        }

        // destroy engine object, and invalidate all associated interfaces
        if (mEngineObject != NULL) {
            (*mEngineObject)->Destroy(mEngineObject);
            mEngineObject = NULL;
            mEngineEngine = NULL;
        }
    }

    //!  Returns last error code
    AudioError_t error()
    {
    }

    // typedef std::unique_lock<Pd> Lock;

    ~OpenSL()
    {
        delete[] mOutputBuf[0];
        delete[] mOutputBuf[1];
        delete[] mInputBuf[0];
        delete[] mInputBuf[1];
    }

  protected:
    Pd&         mPd;
    SLObjectItf mEngineObject;
    SLEngineItf mEngineEngine;
    SLObjectItf mOutputMixObject;

    SLObjectItf bqRecorderObject;
    SLRecordItf bqRecorderRecord;
    SLAndroidSimpleBufferQueueItf bqRecorderBufferQueue;;

    SLObjectItf                   bqPlayerObject;
    SLPlayItf                     bqPlayerPlay;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
    SLEffectSendItf               bqPlayerEffectSend;
    SLMuteSoloItf                 bqPlayerMuteSolo;
    SLVolumeItf                   bqPlayerVolume;

    std::mutex                   mPlayerLock;
    std::condition_variable      mInputBufReady;
    std::condition_variable      mOutputBufReady;
    std::shared_ptr<std::thread> mMixerThread;

    AudioError_t mError;

    // Pd(int inChannels, int outChannels, int sampleRate);

    void initSL(int inChannels, int outChannels, int sampleRate)
    {
        CI_LOGD("OSL: initializing");

        SLresult result;

        result = slCreateEngine(&mEngineObject, 0, NULL, 0, NULL, NULL);
        assert(SL_RESULT_SUCCESS == result);

        result = (*mEngineObject)->Realize(mEngineObject, SL_BOOLEAN_FALSE);
        assert(SL_RESULT_SUCCESS == result);

        result = (*mEngineObject)->GetInterface(mEngineObject, SL_IID_ENGINE, &mEngineEngine);
        assert(SL_RESULT_SUCCESS == result);

        if (inChannels) {
            initInput(inChannels, sampleRate);
        }
        initOutput(outChannels, sampleRate);

        CI_LOGD("OSL: completed initialization");
    }

    void initInput(int channels, int sampleRate)
    {
        SLresult result;
        SLuint32 slrate = slSampleRate(sampleRate);
        assert(slrate != -1);

        // configure audio source
        SLDataLocator_IODevice loc_dev = { 
            SL_DATALOCATOR_IODEVICE, 
            SL_IODEVICE_AUDIOINPUT, 
            SL_DEFAULTDEVICEID_AUDIOINPUT, 
            NULL
        };
        SLDataSource audioSrc = { &loc_dev, NULL };

        // configure audio sink
        int speakers = channels > 1 ? SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT : SL_SPEAKER_FRONT_CENTER;

        SLDataLocator_AndroidSimpleBufferQueue loc_bq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };

        SLDataFormat_PCM format_pcm = { 
            SL_DATAFORMAT_PCM, channels, slrate,
            SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
            speakers, SL_BYTEORDER_LITTLEENDIAN
        };
        SLDataSink audioSnk = {&loc_bq, &format_pcm};

        // create audio recorder (requires the RECORD_AUDIO permission)
        const SLInterfaceID id[1] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
        const SLboolean req[1]    = { SL_BOOLEAN_TRUE };
        result = (*mEngineEngine)->CreateAudioRecorder(mEngineEngine, &bqRecorderObject, &audioSrc, &audioSnk, 1, id, req);
        assert(SL_RESULT_SUCCESS == result);

        // realize the audio recorder
        result = (*bqRecorderObject)->Realize(bqRecorderObject, SL_BOOLEAN_FALSE);
        assert(SL_RESULT_SUCCESS == result);

        // get the record interface
        result = (*bqRecorderObject)->GetInterface(bqRecorderObject, SL_IID_RECORD, &bqRecorderRecord);
        assert(SL_RESULT_SUCCESS == result);

        // get the buffer queue interface
        result = (*bqRecorderObject)->GetInterface(bqRecorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &(bqRecorderBufferQueue));
        assert(SL_RESULT_SUCCESS == result);

        // register callback on the buffer queue
        result = (*bqRecorderBufferQueue)->RegisterCallback(bqRecorderBufferQueue, bqRecorderCallback, (void*) this);
        assert(SL_RESULT_SUCCESS == result);
        result = (*bqRecorderRecord)->SetRecordState(bqRecorderRecord, SL_RECORDSTATE_RECORDING);

    }

    void initOutput(int channels, int sampleRate)
    {
        SLresult result;
        SLuint32 slrate = slSampleRate(sampleRate);
        assert(slrate != -1);

        const SLInterfaceID ids[] = { SL_IID_VOLUME };
        const SLboolean req[] = { SL_BOOLEAN_FALSE };
        result = (*mEngineEngine)->CreateOutputMix(mEngineEngine, &mOutputMixObject, 1, ids, req);

        assert(SL_RESULT_SUCCESS == result);

        result = (*mOutputMixObject)->Realize(mOutputMixObject, SL_BOOLEAN_FALSE);
        assert(SL_RESULT_SUCCESS == result);

        int speakers = (channels > 1) ? SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT : SL_SPEAKER_FRONT_CENTER;
        SLDataLocator_AndroidSimpleBufferQueue loc_bufq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
        SLDataFormat_PCM format_pcm = {
            SL_DATAFORMAT_PCM, 
            channels, 
            slrate,
            SL_PCMSAMPLEFORMAT_FIXED_16, 
            SL_PCMSAMPLEFORMAT_FIXED_16,
            speakers, 
            SL_BYTEORDER_LITTLEENDIAN
        };
        SLDataSource audioSrc = {&loc_bufq, &format_pcm};

        // configure audio sink
        SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mOutputMixObject};
        SLDataSink audioSnk = {&loc_outmix, NULL};

        // create audio player
        const SLInterfaceID idsp[2] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
        const SLboolean reqp[2] = {SL_BOOLEAN_TRUE};
        result = (*mEngineEngine)->CreateAudioPlayer(mEngineEngine, 
                &bqPlayerObject, &audioSrc, &audioSnk, 1, idsp, reqp);
        assert(SL_RESULT_SUCCESS == result);

        // realize the player
        result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE); 
        assert(SL_RESULT_SUCCESS == result);

        // get the play interface
        result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
        assert(SL_RESULT_SUCCESS == result);

        // get the buffer queue interface
        result = (*bqPlayerObject)->GetInterface(bqPlayerObject, 
                SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &bqPlayerBufferQueue);
        assert(SL_RESULT_SUCCESS == result);

        // register callback on the buffer queue
        result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, (void*) this);
        assert(SL_RESULT_SUCCESS == result);
    }

    void setError(AudioError_t error)
    {
        mError = error;
    }

    void playerLoop()
    {
        while (mPlayerRunning)
        {
            // CI_LOGD("OSL: Player running");
            if (mInputChannels > 0) {
                enqueueRecorder();
            }

            {
                Pd::Lock lock(mPd);
                libpd_process_short(kBufferSamples / libpd_blocksize(), mInputBuf[mInputBufIndex], mOutputBuf[mOutputBufIndex]);
            }

            enqueuePlayer();

            mInputBufIndex  ^= 1;
            mOutputBufIndex ^= 1;
        }
    }

    void enqueueRecorder()
    {
        {
            unique_lock<mutex> lock(mPlayerLock);

            while (!mInputReady) {
                mInputBufReady.wait(lock);
            }
            mInputReady = false;
        }

        SLresult result = (*bqRecorderBufferQueue)->Enqueue(bqRecorderBufferQueue, 
                mInputBuf[mInputBufIndex], mInputBufSamples * sizeof(int16_t));
        // for (int i=0; i < 100; ++i) {
        //     CI_LOGD("  rec buffer[%d] : %d", i, mInputBuf[mInputBufIndex][i]);
        // }

        assert(SL_RESULT_SUCCESS == result);
    }

    void enqueuePlayer()
    {
        {
            unique_lock<mutex> lock(mPlayerLock);

            while (!mOutputReady && mPlayerRunning) {
                mOutputBufReady.wait(lock);
            }
            mOutputReady = false;
        }
        SLresult result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, 
                mOutputBuf[mOutputBufIndex], mOutputBufSamples * sizeof(int16_t));
        // for (int i=0; i < 100; ++i) {
        //     CI_LOGD("  play buffer[%d] : %d", i, mOutputBuf[mOutputBufIndex][i]);
        // }
        assert(SL_RESULT_SUCCESS == result);
    }

    bool mPlayerRunning;
    bool mRecorderRunning;
    bool mOutputReady;
    bool mInputReady;

    int      mOutputBufIndex;
    int16_t* mOutputBuf[2];
    int      mInputBufIndex;
    int16_t* mInputBuf[2];

    int      mOutputBufSamples;
    int      mInputBufSamples;

    int mInputChannels;
    int mOutputChannels;

    static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *audioPtr)
    {
        // notify player thread we're ready to enqueue another buffer
        OpenSL* sl = (OpenSL*) audioPtr;
        {
            unique_lock<mutex> lock(sl->mPlayerLock);
            sl->mOutputReady = true;
            // CI_LOGD("bqPlayerCallback");
        }
        (sl->mOutputBufReady).notify_one();
    }

    static void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *audioPtr)
    {
        // notify player thread we're ready to enqueue another buffer
        OpenSL* sl = (OpenSL*) audioPtr;
        {
            unique_lock<mutex> lock(sl->mPlayerLock);
            sl->mInputReady = true;
            // CI_LOGD("bqRecorderCallback");
        }
        (sl->mInputBufReady).notify_one();
    }
};

PdAudioRef PdAudio::create(Pd& pd, int inChannels, int outChannels, int sampleRate)
{
    return PdAudioRef(new OpenSL(pd, inChannels, outChannels, sampleRate));
}


