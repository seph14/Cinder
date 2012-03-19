#pragma once

#include <vector>  
#include <string>

#include "cinder/Cinder.h"
#include "cinder/Thread.h"
#include "cinder/Filesystem.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "z_libpd.h"

namespace cel { namespace pd {

class Pd;

class Bang { };

//  A float or a symbol, for building lists/messages
class Atom
{
  public:
    enum Atom_t {
        ATOM_FLOAT,
        ATOM_SYMBOL
    };

    Atom(float x);
    Atom(const std::string& sym);

    Atom_t      mType;
    std::string mSymbol;
    float       mFloat;
};

typedef Atom Float;
typedef Atom Symbol;

//  A list of atoms representing a list/message
class AtomList
{
  public:
    AtomList& operator<<(const Atom& atom);
    std::vector<Atom> atoms;
};

typedef AtomList Message;
typedef AtomList List;

class Receiver
{
  public:
	// message handlers
	virtual void onPrint(const std::string& msg) {}
    virtual void onBang(const std::string& dest) {}
    virtual void onFloat(const std::string& dest, float value) {}
    virtual void onSymbol(const std::string& dest, const std::string& symbol) {}
    virtual void onList(const std::string& dest, const List& list) {}
    virtual void onMessage(const std::string& dest, const std::string& msg, const Message& list) {}

    virtual ~Receiver() { }
};
typedef std::shared_ptr<Receiver> ReceiverRef;

class Dispatcher : public Receiver
{
  protected:
    typedef std::multimap<std::string, Receiver&> SubsMap;
    SubsMap mSubs;
    std::map<std::string, void*> mBinders;

  public:
    virtual void subscribe(Receiver& receiver, const std::string& dest);
    virtual void unsubscribe(Receiver& receiver, const std::string& dest);
    virtual void unsubscribeAll();

    virtual void onPrint(const std::string& msg);
    virtual void onBang(const std::string& dest);
    virtual void onFloat(const std::string& dest, float value);
    virtual void onSymbol(const std::string& dest, const std::string& symbol);
    virtual void onList(const std::string& dest, const List& list);
    virtual void onMessage(const std::string& dest, const std::string& msg, const Message& list);

    virtual ~Dispatcher() { }
};
typedef std::shared_ptr<Dispatcher> DispatcherRef;

class Chain
{
  public:
    Chain(Pd& pd, bool lock=false);
    virtual ~Chain();
  protected:
    Pd& mPd;
    bool mLock;
};

//  For chaining single messages (bang, float, symbol)
class SendChain : public Chain
{
  public:
    SendChain(Pd& pd, const std::string& recv, bool lock=false);
    SendChain& operator<<(const Bang& bang);
    SendChain& operator<<(const Atom& atom);
  protected:
    std::string mRecv;
};

//  For chaining lists/typed messages
class MessageChain : public Chain
{
  public:
    MessageChain(Pd& pd, const std::string& recv, const std::string& msg=std::string(), bool lock=false);
    ~MessageChain();
    MessageChain& operator<<(const Atom& atom);
  protected:
    std::string mRecv;
    std::string mMsg;
    AtomList mList;
};

class SubscribeChain : public Chain
{
  public:
    enum Subscribe_t{
        SUBSCRIBE,
        UNSUBSCRIBE
    };

    SubscribeChain(Pd& pd, Dispatcher& dispatcher, Receiver& receiver, Subscribe_t mode, bool lock=false);
    SubscribeChain& operator<<(const std::string& dest);

  protected:
    Dispatcher&   mDispatcher;
    Receiver&     mReceiver;
    Subscribe_t   mMode;
};


enum AudioError_t {
    NONE,
};

typedef std::shared_ptr<class Pd> PdRef;

class Patch
{
};

/**
 * Client interface to Pd
 */
class PdClient
{
  public:
    //  Pd interface
    virtual void  computeAudio(bool on) = 0;
    virtual void* openFile(const char* filename, const ci::fs::path& dir) = 0;

    virtual void addToSearchPath(const ci::fs::path& path) = 0;

    //! send a bang
    virtual int sendBang(const std::string& recv) = 0;
    //! send a float
    virtual int sendFloat(const std::string& recv, float x) = 0;
    //! send a symbol
    virtual int sendSymbol(const std::string& recv, const std::string& sym) = 0;

    /**
      List aList;
      aList << 100 << 292.99 << 'c' << "string";
      pd.sendList("test", aList);
     */
    virtual int sendList(const std::string& recv, AtomList& list) = 0;
    /**
      Message msg;
      msg << 1;
      pd.sendMessage("pd", "dsp", msg);
     */
    virtual int sendMessage(const std::string& recv, const std::string& msg, AtomList& list) = 0;

    //! pd.send("test") << Bang() << 100 << "symbol1";
    virtual SendChain    send(const std::string& recv) = 0;
    //! pd.sendList("test") << 100 << 292.99 << 'c' << "string";
    virtual MessageChain sendList(const std::string& recv) = 0;
    //! pd.sendMessage("pd", "dsp") << 1;
    virtual MessageChain sendMessage(const std::string& recv, const std::string& msg) = 0;

    //! pd.subscribe(receiver) << "pitch" << "timer";
    virtual SubscribeChain subscribe(Receiver& receiver) = 0;
    //! pd.unsubscribe(receiver) << "pitch" << "timer";
    virtual SubscribeChain unsubscribe(Receiver& receiver) = 0;
    virtual void unsubscribeAll() = 0;
};
typedef std::shared_ptr<PdClient> PdClientRef;

class Pd : public PdClient
{
  public:
    //!  Create and initialize the audio system
    static PdRef init(int inChannels, int outChannels, int sampleRate);

    //!  Start the audio system playing
    void play();

    //!  Pause the audio system
    void pause();

    //!  Shut down audio system
    void close();

    //!  Returns last error code
    AudioError_t error();

    //!  Returns an auto-locking Pd client interface
    PdClientRef getLockingClient();

    //  Pd client interface implementation (no locking)
    virtual void           computeAudio(bool on);
    virtual void*          openFile(const char* filename, const ci::fs::path& dir);
    virtual void           addToSearchPath(const ci::fs::path& path);
    virtual int            sendBang(const std::string& recv);
    virtual int            sendFloat(const std::string& recv, float x);
    virtual int            sendSymbol(const std::string& recv, const std::string& sym);
    virtual int            sendList(const std::string& recv, AtomList& list);
    virtual int            sendMessage(const std::string& recv, const std::string& msg, AtomList& list);
    virtual SendChain      send(const std::string& recv);
    virtual MessageChain   sendList(const std::string& recv);
    virtual MessageChain   sendMessage(const std::string& recv, const std::string& msg);
    virtual SubscribeChain subscribe(Receiver& receiver);
    virtual SubscribeChain unsubscribe(Receiver& receiver);
    virtual void           unsubscribeAll();

    //  Lockable concept implementation, locks the audio thread via mPdLock
    void lock();
    bool try_lock();
    void unlock();

    typedef std::unique_lock<Pd> Lock;

    ~Pd();

  public:
	static DispatcherRef sDispatcher;

  protected:
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

    std::mutex                   mPdLock;
    std::mutex                   mPlayerLock;
    std::condition_variable      mInputBufReady;
    std::condition_variable      mOutputBufReady;
    std::shared_ptr<std::thread> mMixerThread;

    AudioError_t mError;

    Pd(int inChannels, int outChannels, int sampleRate);

    void initSL(int inChannels, int outChannels, int sampleRate);
    void initInput(int channels, int sampleRate);
    void initOutput(int channels, int sampleRate);
    void setError(AudioError_t error);

    void playerLoop();
    void enqueueRecorder();
    void enqueuePlayer();

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

    static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
    static void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
};

} }
