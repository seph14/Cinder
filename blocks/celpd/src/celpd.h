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
typedef std::shared_ptr<Pd> PdRef;

class PdClient;

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
    Chain(PdClient& pd);
    virtual ~Chain();
  protected:
    PdClient& mPd;
};

//  For chaining single messages (bang, float, symbol)
class SendChain : public Chain
{
  public:
    SendChain(PdClient& pd, const std::string& recv);
    SendChain& operator<<(const Bang& bang);
    SendChain& operator<<(const Atom& atom);
  protected:
    std::string mRecv;
};

//  For chaining lists/typed messages
class MessageChain : public Chain
{
  public:
    MessageChain(PdClient& pd, const std::string& recv, const std::string& msg=std::string());
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

    SubscribeChain(PdClient& pd, Dispatcher& dispatcher, Receiver& receiver, Subscribe_t mode);
    SubscribeChain& operator<<(const std::string& dest);

  protected:
    Dispatcher&   mDispatcher;
    Receiver&     mReceiver;
    Subscribe_t   mMode;
};


enum AudioError_t {
    NONE,
};

class Patch
{
};

/**
  * Audio interface
  */
typedef std::shared_ptr<class PdAudio> PdAudioRef;
class PdAudio
{
  public:
    //!  Start the audio system playing
    virtual void play()          = 0;
    //!  Pause the audio system
    virtual void pause()         = 0;
    //!  Shut down audio system
    virtual void close()         = 0;
    //!  Returns last error code
    virtual AudioError_t error() = 0;

    virtual ~PdAudio() { }

    //  Factory method implemented by platform backend
    static PdAudioRef create(Pd& pd, int inChannels, int outChannels, int sampleRate);
};

/**
 * Client interface to Pd
 */
class PdClient
{
  public:
    //  Pd interface
    virtual void  computeAudio(bool on);
    virtual void* openFile(const char* filename, const ci::fs::path& dir);

    virtual void addToSearchPath(const ci::fs::path& path);

    //! send a bang
    virtual int sendBang(const std::string& recv);
    //! send a float
    virtual int sendFloat(const std::string& recv, float x);
    //! send a symbol
    virtual int sendSymbol(const std::string& recv, const std::string& sym);

    /**
      List aList;
      aList << 100 << 292.99 << 'c' << "string";
      pd.sendList("test", aList);
     */
    virtual int sendList(const std::string& recv, AtomList& list);
    /**
      Message msg;
      msg << 1;
      pd.sendMessage("pd", "dsp", msg);
     */
    virtual int sendMessage(const std::string& recv, const std::string& msg, AtomList& list);

    //! pd.send("test") << Bang() << 100 << "symbol1";
    virtual SendChain    send(const std::string& recv);
    //! pd.sendList("test") << 100 << 292.99 << 'c' << "string";
    virtual MessageChain sendList(const std::string& recv);
    //! pd.sendMessage("pd", "dsp") << 1;
    virtual MessageChain sendMessage(const std::string& recv, const std::string& msg);

    //! pd.subscribe(receiver) << "pitch" << "timer";
    virtual SubscribeChain subscribe(Receiver& receiver);
    //! pd.unsubscribe(receiver) << "pitch" << "timer";
    virtual SubscribeChain unsubscribe(Receiver& receiver);
    virtual void unsubscribeAll();

    virtual ~PdClient() { }
};
typedef std::shared_ptr<PdClient> PdClientRef;

class Pd // : public PdClient
{
  public:
    //!  Create and initialize the audio system
    static PdRef init(int inChannels, int outChannels, int sampleRate, bool autoLock=true);

    PdAudio*  audio();
    PdClient* client();

    //  Lockable concept implementation, locks the audio thread via mPdLock
    void lock();
    bool try_lock();
    void unlock();

    typedef std::unique_lock<Pd> Lock;

    ~Pd();

  public:
    //  Convenience methods for accessing audio and client methods

    //  Audio interface wrapper
    void play();
    void pause();
    void close();
    AudioError_t error();

    //  Client interface wrapper
    void  computeAudio(bool on);
    void* openFile(const char* filename, const ci::fs::path& dir);
    void addToSearchPath(const ci::fs::path& path);
    int sendBang(const std::string& recv);
    int sendFloat(const std::string& recv, float x);
    int sendSymbol(const std::string& recv, const std::string& sym);
    int sendList(const std::string& recv, AtomList& list);
    int sendMessage(const std::string& recv, const std::string& msg, AtomList& list);
    SendChain    send(const std::string& recv);
    MessageChain sendList(const std::string& recv);
    MessageChain sendMessage(const std::string& recv, const std::string& msg);
    SubscribeChain subscribe(Receiver& receiver);
    SubscribeChain unsubscribe(Receiver& receiver);
    void unsubscribeAll();


  public:
	static DispatcherRef sDispatcher;

  protected:
    PdAudioRef  mAudio;
    PdClientRef mClient;

    std::mutex  mPdLock;

    Pd(bool autoLock);
};

} }
