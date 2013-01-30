#include <vector>  
#include <string>

#include "cinder/Cinder.h"
#include "cinder/Thread.h"
#include "cinder/Filesystem.h"

#if defined( CINDER_MSW ) || defined( CINDER_ANDROID )
#define LIBPD_DYNAMIC_LOAD
#endif

namespace cel { namespace pd {

class Pd;
typedef std::shared_ptr<Pd> PdRef;

class PdClient;
typedef std::shared_ptr<PdClient> PdClientRef;

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

//  For chaining subscribe receiver symbols
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
    ERROR_NONE,
    ERROR_INIT_AUDIO,
    ERROR_OPEN_AUDIO,
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

class Pd 
{
  public:
    //!  Create the Pd audio system
    static PdRef create(bool autoLock=true, const std::string& dllName = std::string());

    //!  Initialize 
    void init(int inChannels, int outChannels, int sampleRate);

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
    void*       mDLL;

    std::mutex  mPdLock;

    Pd(bool autoLock);
};

} }
