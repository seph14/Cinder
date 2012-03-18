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


//  For chaining single messages (bang, float, symbol)
class SendChain
{
public:
    SendChain(Pd& pd, const std::string& recv);
    SendChain& operator<<(const Bang& bang);
    SendChain& operator<<(const Atom& atom);
protected:
    Pd& mPd;
    std::string mRecv;
};

//  For chaining lists/typed messages
class MessageChain
{
public:
    MessageChain(Pd& pd, const std::string& recv, const std::string& msg=std::string());
    ~MessageChain();
    MessageChain& operator<<(const Atom& atom);
protected:
    Pd& mPd;
    std::string mRecv;
    std::string mMsg;
    AtomList mList;
};

class SubscribeChain
{
public:
    enum Subscribe_t{
        SUBSCRIBE,
        UNSUBSCRIBE
    };

    SubscribeChain(DispatcherRef dispatcher, Receiver& receiver, Subscribe_t mode);
    SubscribeChain& operator<<(const std::string& dest);

protected:
    DispatcherRef mDispatcher;
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

class PdInterface
{
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

    //! pd.subscribe(receiver) << "pitch";
    virtual SubscribeChain subscribe(Receiver& receiver) = 0;
    //! pd.unsubscribe(receiver) << "pitch";
    virtual SubscribeChain unsubscribe(Receiver& receiver) = 0;
    virtual void unsubscribeAll() = 0;

};

class Pd : public PdInterface
{
  public:
    //  Create and initialize the audio system
    static PdRef init(int inChannels, int outChannels, int sampleRate, bool lockPd=true);

    //  Start the audio system playing
    void play();

    //  Pause the audio system
    void pause();

    //  Shut down audio system
    void close();

    //  Returns last error code
    AudioError_t error();

    //  Pd interface
    void  computeAudio(bool on);
    void* openFile(const char* filename, const ci::fs::path& dir);

    void addToSearchPath(const ci::fs::path& path);

    //! send a bang
    int sendBang(const std::string& recv);
    //! send a float
    int sendFloat(const std::string& recv, float x);
    //! send a symbol
    int sendSymbol(const std::string& recv, const std::string& sym);

    /**
      List aList;
      aList << 100 << 292.99 << 'c' << "string";
      pd.sendList("test", aList);
     */
    int sendList(const std::string& recv, AtomList& list);
    /**
      Message msg;
      msg << 1;
      pd.sendMessage("pd", "dsp", msg);
     */
    int sendMessage(const std::string& recv, const std::string& msg, AtomList& list);

    //! pd.send("test") << Bang() << 100 << "symbol1";
    SendChain    send(const std::string& recv);
    //! pd.sendList("test") << 100 << 292.99 << 'c' << "string";
    MessageChain sendList(const std::string& recv);
    //! pd.sendMessage("pd", "dsp") << 1;
    MessageChain sendMessage(const std::string& recv, const std::string& msg);

    //! pd.subscribe(receiver) << "pitch";
    SubscribeChain subscribe(Receiver& receiver);
    //! pd.unsubscribe(receiver) << "pitch";
    SubscribeChain unsubscribe(Receiver& receiver);
    void unsubscribeAll();

    //  Lockable concept, locks the audio (Pd) thread
    void lock();
    bool try_lock();
    void unlock();

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

    std::mutex                   mPlayerLock;
    std::recursive_mutex         mPdLock;
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

// //  Helper class delegating the receiver interface to another class
// template <typename T>
// class ReceiverDelegate : Receiver
// {
// protected:
//     T& mTarget;
// 
// public:
//     static ReceiverRef create(T& target)
//     {
//         return ReceiverRef(new ReceiverDelegate(target));
//     }
// 
//     ReceiverDelegate(T& target) : mTarget(target)
//     { }
// 
// 	virtual void onPrint(const std::string& msg) 
//     {
//         mTarget.onPrint(msg);
//     }
// 
//     virtual void onBang(const std::string& dest)
//     {
//         mTarget.onBang(dest);
//     }
// 
//     virtual void onFloat(const std::string& dest, float value)
//     {
//         mTarget.onFloat(dest, value);
//     }
// 
//     virtual void onSymbol(const std::string& dest, const std::string& symbol)
//     {
//         mTarget.onSymbol(dest, symbol);
//     }
// 
//     virtual void onList(const std::string& dest, const List& list)
//     {
//         mTarget.onList(dest, list);
//     }
// 
//     virtual void onMessage(const std::string& dest, const std::string& msg, const Message& list)
//     {
//         mTarget.onMessage(dest, msg, list);
//     }
// };


} }
