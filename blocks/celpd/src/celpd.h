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

enum AudioError_t {
    NONE,
};

typedef std::shared_ptr<class Pd> PdRef;

class Patch
{
};

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
};

typedef std::shared_ptr<Receiver> ReceiverRef;

// class Dispatcher : public Receiver
// {
//   public:
// 	void subscribe(const std::string& src, Receiver& receiver);
// 	void unsubscribe(const std::string& src, Receiver& receiver);
// 	void unsubscribeAll();
// 
// };

class Pd
{
  public:
    //  Create and initialize the audio system
    static PdRef init(int inChannels, int outChannels, int sampleRate);

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

    ~Pd();

  public:
	static ReceiverRef sReceiver;

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
    std::mutex                   mPdLock;
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
