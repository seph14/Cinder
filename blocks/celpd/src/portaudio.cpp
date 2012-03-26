#include "celpd.h"
#include "portaudio.h"
#include "z_libpd.h"

using namespace ci;
using namespace cel;
using namespace cel::pd;

using std::shared_ptr;
using std::unique_lock;

const int      kTicksPerBuffer = 1;
const uint32_t kBufferSamples  = 512;  // must be a multiple of libpd block size (ie 64)
#define PA_SAMPLE_TYPE      paFloat32

static int pdCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    Pd* pd = (Pd*) userData;
    {
        Pd::Lock lock(*pd);
        libpd_process_float(kBufferSamples / libpd_blocksize(), (float*) inputBuffer, (float*) outputBuffer);
    }
    return 0;
}

class PortAudio : public PdAudio
{
  public:
    PortAudio(Pd& pd, int inChannels, int outChannels, int sampleRate) 
        : mPd(pd), mInputChannels(inChannels), mOutputChannels(outChannels), mError(ERROR_NONE)
    {
        PaError err;

        err = Pa_Initialize();
        if (err != paNoError) {
            setError(ERROR_INIT_AUDIO);
            // Log("PortAudio init error: %s", Pa_GetErrorText(err));
            goto error;
        }
        libpd_init_audio(inChannels, outChannels, sampleRate);

        PaStreamParameters inputParameters, outputParameters;
        if (inChannels > 0) {
            inputParameters.device = Pa_GetDefaultInputDevice();
            if (inputParameters.device == paNoDevice) {
                setError(ERROR_OPEN_AUDIO);
                goto error;
            }

            inputParameters.channelCount = inChannels;
            inputParameters.sampleFormat = PA_SAMPLE_TYPE;
            inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
            inputParameters.hostApiSpecificStreamInfo = NULL;
        }
        if (outChannels > 0) {
            outputParameters.device = Pa_GetDefaultOutputDevice();
            if (outputParameters.device == paNoDevice) {
                setError(ERROR_OPEN_AUDIO);
                goto error;
            }
            outputParameters.channelCount = outChannels;
            outputParameters.sampleFormat = PA_SAMPLE_TYPE;
            outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
            outputParameters.hostApiSpecificStreamInfo = NULL;
        }

        err = Pa_OpenStream( 
                &mStream,
                inChannels > 0  ? &inputParameters : NULL,
                outChannels > 0 ? &outputParameters : NULL,
                sampleRate, 
                kBufferSamples,
                0,
                pdCallback,
                &pd );

        if( err != paNoError )  {
            setError(ERROR_OPEN_AUDIO);
            goto error;
        }

        return;

    error:
        Pa_Terminate();
    }


    //!  Start the audio system playing
    void play()
    {
        // set the player's state to playing
        // CI_LOGD("Pd: play()");
        Pa_StartStream(mStream);
    }

    //!  Pause the audio system
    void pause()
    {
        // CI_LOGD("Pd: pause()");
        Pa_StopStream(mStream);
    }

    //!  Shut down audio system
    void close()
    {
        PaError err;
        err = Pa_CloseStream(mStream);
        err = Pa_Terminate();
    }

    //!  Returns last error code
    AudioError_t error()
    {
        return mError;
    }

    // typedef std::unique_lock<Pd> Lock;

    ~PortAudio()
    {
    }

  protected:
    Pd&       mPd;
    PaStream *mStream;

    AudioError_t mError;

    int mInputChannels;
    int mOutputChannels;

    void setError(AudioError_t error)
    {
        mError = error;
    }
};

PdAudioRef PdAudio::create(Pd& pd, int inChannels, int outChannels, int sampleRate)
{
    return PdAudioRef(new PortAudio(pd, inChannels, outChannels, sampleRate));
}

