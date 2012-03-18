#include "celpd.h"
#include "cinder/CinderMath.h"

#include "s_stuff.h"

#include <cassert>
#include <map>
#include <utility>

using namespace ci;
using namespace cel;
using namespace cel::pd;

using std::string;
using std::vector;
using std::multimap;
using std::make_pair;
using std::shared_ptr;
using std::thread;
using std::unique_lock;
using std::mutex;

const uint32_t MAXIMUM_CHANNEL_COUNT = 512;

const int      kTicksPerBuffer = 1;
const uint32_t kBufferSamples  = 1024;  // must be a multiple of libpd block size (ie 64)

Atom::Atom(float x) : mType(ATOM_FLOAT), mFloat(x)
{ }

Atom::Atom(const string& sym) : mType(ATOM_SYMBOL), mSymbol(sym)
{ }

AtomList& AtomList::operator<<(const Atom& atom)
{
    // XXX convert to t_atom and push
    atoms.push_back(atom);
    return *this;
}

SendChain::SendChain(Pd& pd, const string& recv) : mPd(pd), mRecv(recv)
{ }

SendChain& SendChain::operator<<(const Bang& bang)
{
    mPd.sendBang(mRecv);
    return *this;
}

SendChain& SendChain::operator<<(const Atom& atom)
{
    if (atom.mType == Atom::ATOM_FLOAT) {
        mPd.sendFloat(mRecv, atom.mFloat);
    }
    else {
        mPd.sendSymbol(mRecv, atom.mSymbol);
    }
    return *this;
}

MessageChain::MessageChain(Pd& pd, const string& recv, const string& msg)
    : mPd(pd), mRecv(recv), mMsg(msg)
{ }

MessageChain::~MessageChain()
{
    if (!mList.atoms.empty())
        mPd.sendMessage(mRecv, mMsg, mList);
}

MessageChain& MessageChain::operator<<(const Atom& atom)
{
    mList << atom;
    return *this;
}

SubscribeChain::SubscribeChain(DispatcherRef dispatcher, ReceiverRef receiver, Subscribe_t mode)
    : mDispatcher(dispatcher), mReceiver(receiver), mMode(mode)
{
}

SubscribeChain& SubscribeChain::operator<<(const std::string& dest)
{
    if (mMode == SubscribeChain::SUBSCRIBE) {
        mDispatcher->subscribe(mReceiver, dest);
    }
    else if (mMode == SubscribeChain::UNSUBSCRIBE) {
        mDispatcher->unsubscribe(mReceiver, dest);
    }
}

static void cel_printhook(const char* msg) 
{
    if (ReceiverRef recv = Pd::sDispatcher)
        recv->onPrint(msg);
}

static void cel_banghook(const char* src) 
{
    if (ReceiverRef recv = Pd::sDispatcher)
        recv->onBang(src);
}

static void cel_floathook(const char* src, float x) 
{
    if (ReceiverRef recv = Pd::sDispatcher)
        recv->onFloat(src, x);
}

static void cel_symbolhook(const char* src, const char* sym)
{
    if (ReceiverRef recv = Pd::sDispatcher)
        recv->onSymbol(src, sym);
}

static void cel_listhook(const char* src, int argc, t_atom* argv)
{
	List list;
	for (int i=0; i < argc; ++i) {
		t_atom a = argv[i];  
		
		if (a.a_type == A_FLOAT) {  
            list << a.a_w.w_float;
		}
		else if (a.a_type == A_SYMBOL) {  
            list << string(a.a_w.w_symbol->s_name);
		}
	}
	
    if (ReceiverRef recv = Pd::sDispatcher)
        recv->onList(src, list);
}

static void cel_messagehook(const char* src, const char *sym, int argc, t_atom* argv)
{
	Message list;
	for (int i=0; i < argc; ++i) {
		t_atom a = argv[i];  
		
		if (a.a_type == A_FLOAT) {  
            list << a.a_w.w_float;
		}
		else if (a.a_type == A_SYMBOL) {  
            list << string(a.a_w.w_symbol->s_name);
		}
	}
	
    if (ReceiverRef recv = Pd::sDispatcher)
        recv->onMessage(src, sym, list);
}

void Dispatcher::onPrint(const std::string& msg)
{
    CI_LOGD("PD: %s", msg.c_str());
}

void Dispatcher::onBang(const std::string& dest)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        it->second->onBang(dest);
    }
}

void Dispatcher::onFloat(const std::string& dest, float value)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        it->second->onFloat(dest, value);
    }
}

void Dispatcher::onSymbol(const std::string& dest, const std::string& symbol)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        it->second->onSymbol(dest, symbol);
    }
}

void Dispatcher::onList(const std::string& dest, const List& list)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        it->second->onList(dest, list);
    }
}

void Dispatcher::onMessage(const std::string& dest, const std::string& msg, const Message& list)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        it->second->onMessage(dest, msg, list);
    }
}

void Dispatcher::subscribe(ReceiverRef receiver, const string& dest)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        if (it->second == receiver) {
            //  Already subscribed
            return;
        }
    }

    mSubs.insert(make_pair(dest, receiver));
}

void Dispatcher::unsubscribe(ReceiverRef receiver, const string& dest)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        if (it->second == receiver) {
            mSubs.erase(it);
            break;
        }
    }
}

void Dispatcher::unsubscribeAll()
{
    mSubs.clear();
}

DispatcherRef Pd::sDispatcher;

PdRef Pd::init(int inChannels, int outChannels, int sampleRate)
{
    sDispatcher = DispatcherRef(new Dispatcher());

    //  Initialize PD
    libpd_printhook   = (t_libpd_printhook) cel_printhook;

	libpd_banghook    = (t_libpd_banghook) cel_banghook;
	libpd_floathook   = (t_libpd_floathook) cel_floathook;
	libpd_symbolhook  = (t_libpd_symbolhook) cel_symbolhook;
	libpd_listhook    = (t_libpd_listhook) cel_listhook;
	libpd_messagehook = (t_libpd_messagehook) cel_messagehook;
	
    libpd_init();
    sys_debuglevel = 4;
    sys_verbose = 1;

    return PdRef(new Pd(inChannels, outChannels, sampleRate));
}

//  Start the audio system
void Pd::play()
{
    // set the player's state to playing
    CI_LOGD("Pd: play()");
    SLresult result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);

    //  Start playback by queuing an initial buffer (empty)
    {
        unique_lock<mutex> lock(mPlayerLock);
        mInputReady    = true;
        mOutputReady   = true;
        mPlayerRunning = true;
        for (int i=0; i < 2; ++i) {
            memset(mOutputBuf[i], 0, sizeof(int16_t) * mOutputBufSamples);
            memset(mInputBuf[i],  0, sizeof(int16_t) * mInputBufSamples);
        }
        mMixerThread   = shared_ptr<thread>(new thread(&Pd::playerLoop, this));
    }
    mOutputBufReady.notify_one();
    if (mInputChannels > 0)
        mInputBufReady.notify_one();

    computeAudio(true);
}

void Pd::computeAudio(bool on)
{
    //  Start/stop DSP
    sendMessage("pd", "dsp") << (on ? 1.0f : 0);
}

void* Pd::openFile(const char* filename, const fs::path& dir)
{
    unique_lock<mutex> lock(mPdLock);
    return dir.empty() ? NULL : libpd_openfile(filename, dir.string().c_str());
}

void Pd::addToSearchPath(const fs::path& path)
{
    unique_lock<mutex> lock(mPdLock);
    if (!path.empty()) {
        CI_LOGD("OSL: Adding to PD search path: %s", path.string().c_str());
        libpd_add_to_search_path(path.string().c_str());
    }
}

int Pd::sendBang(const string& recv)
{
    unique_lock<mutex> lock(mPdLock);
    return libpd_bang(recv.c_str());
}

int Pd::sendFloat(const string& recv, float x)
{
    unique_lock<mutex> lock(mPdLock);
    return libpd_float(recv.c_str(), x);
}

int Pd::sendSymbol(const string& recv, const string& sym)
{
    unique_lock<mutex> lock(mPdLock);
    return libpd_symbol(recv.c_str(), sym.c_str());
}

int Pd::sendList(const std::string& recv, AtomList& list)
{
    return sendMessage(recv, string(), list);
}

int Pd::sendMessage(const std::string& recv, const std::string& msg, AtomList& list)
{
    unique_lock<mutex> lock(mPdLock);

    vector<Atom>& atoms = list.atoms;
    libpd_start_message(atoms.size());
    for (vector<Atom>::iterator it = atoms.begin(); it != atoms.end(); ++it) {
        if (it->mType == Atom::ATOM_FLOAT) {
            libpd_add_float(it->mFloat);
        }
        else {
            libpd_add_symbol(it->mSymbol.c_str());
        }
    }

    if (!msg.empty()) {
        return libpd_finish_message(recv.c_str(), msg.c_str());
    }

    return libpd_finish_list(recv.c_str());
}

SendChain Pd::send(const string& recv)
{
    return SendChain(*this, recv);
}

MessageChain Pd::sendList(const string& recv)
{
    return MessageChain(*this, recv);
}

MessageChain Pd::sendMessage(const string& recv, const string& msg)
{
    return MessageChain(*this, recv, msg);
}

SubscribeChain Pd::subscribe(ReceiverRef receiver)
{
    return SubscribeChain(sDispatcher, receiver, SubscribeChain::SUBSCRIBE);
}

SubscribeChain Pd::unsubscribe(ReceiverRef receiver)
{
    return SubscribeChain(sDispatcher, receiver, SubscribeChain::UNSUBSCRIBE);
}

void Pd::unsubscribeAll()
{
}

//  Stop the audio system
void Pd::pause()
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

void Pd::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *audioPtr)
{
    // notify player thread we're ready to enqueue another buffer
    Pd* audio = (Pd*) audioPtr;
    {
        unique_lock<mutex> lock(audio->mPlayerLock);
        audio->mOutputReady = true;
        // CI_LOGD("bqPlayerCallback");
    }
    (audio->mOutputBufReady).notify_one();
}

void Pd::bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *audioPtr)
{
    // notify player thread we're ready to enqueue another buffer
    Pd* pd = (Pd*) audioPtr;
    {
        unique_lock<mutex> lock(pd->mPlayerLock);
        pd->mInputReady = true;
        // CI_LOGD("bqRecorderCallback");
    }
    (pd->mInputBufReady).notify_one();
}

void Pd::enqueueRecorder()
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

void Pd::enqueuePlayer()
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

//  player thread loop
void Pd::playerLoop()
{
    while (mPlayerRunning)
    {
        // CI_LOGD("OSL: Player running");
        if (mInputChannels > 0) {
            enqueueRecorder();
        }

        {
            unique_lock<mutex> lock(mPdLock);
            libpd_process_short(kBufferSamples / libpd_blocksize(), mInputBuf[mInputBufIndex], mOutputBuf[mOutputBufIndex]);
        }

        enqueuePlayer();

        mInputBufIndex  ^= 1;
        mOutputBufIndex ^= 1;
    }
}

Pd::Pd(int inChannels, int outChannels, int sampleRate) 
    : mEngineObject(NULL), mOutputMixObject(NULL), bqPlayerObject(NULL), 
      mPlayerRunning(false), mRecorderRunning(false), mOutputReady(false),
      mOutputBufIndex(0), mInputBufIndex(0), 
      mInputChannels(inChannels), mOutputChannels(outChannels)
{
    initSL(inChannels, outChannels, sampleRate);
    libpd_init_audio(inChannels, outChannels, sampleRate);
    // mOutputBufSamples = kTicksPerBuffer * libpd_blocksize() * outChannels;
    mOutputBufSamples = kBufferSamples * outChannels;
    mInputBufSamples  = kBufferSamples * inChannels;
    CI_LOGD("OSL: Allocating buffers size kTicks (%d) * blocksize (%d) * channels (%d) = %d",
            kTicksPerBuffer, libpd_blocksize(), outChannels, mOutputBufSamples);

    for (int i=0; i < 2; ++i) {
        mOutputBuf[i] = new int16_t[mOutputBufSamples];
        mInputBuf[i]  = new int16_t[mInputBufSamples];
    }
}

Pd::~Pd()
{
    delete[] mOutputBuf[0];
    delete[] mOutputBuf[1];
    delete[] mInputBuf[0];
    delete[] mInputBuf[1];
}


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

void Pd::initInput(int channels, int sampleRate)
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

void Pd::initOutput(int channels, int sampleRate)
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

void Pd::initSL(int inChannels, int outChannels, int sampleRate)
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

//  Shut down audio system
void Pd::close()
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

//  Returns last error code
AudioError_t Pd::error()
{
}

void Pd::setError(AudioError_t error)
{
    mError = error;
}

