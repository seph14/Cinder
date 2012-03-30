#include "celpd.h"
#include "cinder/CinderMath.h"

#if defined( LIBPD_DYNAMIC_LOAD )
  #include "libpd_dl.h"
  #include "m_pd.h"
#else
  #include "z_libpd.h"
#endif

#include <cassert>
#include <map>
#include <utility>

using namespace ci;

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

namespace cel { namespace pd {

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

Chain::Chain(PdClient& pd) : mPd(pd)
{ 
}

Chain::~Chain()
{
}

SendChain::SendChain(PdClient& pd, const string& recv) 
    : Chain(pd), mRecv(recv)
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

MessageChain::MessageChain(PdClient& pd, const string& recv, const string& msg) 
    : Chain(pd), mRecv(recv), mMsg(msg)
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

SubscribeChain::SubscribeChain(PdClient& pd, Dispatcher& dispatcher, Receiver& receiver, Subscribe_t mode)
    : Chain(pd), mDispatcher(dispatcher), mReceiver(receiver), mMode(mode)
{
}

SubscribeChain& SubscribeChain::operator<<(const std::string& dest)
{
    if (mMode == SubscribeChain::SUBSCRIBE) {
        mDispatcher.subscribe(mReceiver, dest);
    }
    else if (mMode == SubscribeChain::UNSUBSCRIBE) {
        mDispatcher.unsubscribe(mReceiver, dest);
    }

    return *this;
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

static void cel_noteon(int channel, int pitch, int velocity)
{
}

static void cel_controlchange(int channel, int controller, int velocity)
{
}

static void cel_programchange(int channel, int value)
{
}

static void cel_pitchbend(int channel, int value)
{
}

static void cel_aftertouch(int channel, int value)
{
}

static void cel_polyaftertouch(int channel, int pitch, int value)
{
}

static void cel_midibyte(int port, int byte) 
{
}

void Dispatcher::onPrint(const std::string& msg)
{
    // CI_LOGD("PD: %s", msg.c_str());
}

void Dispatcher::onBang(const std::string& dest)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        it->second.onBang(dest);
    }
}

void Dispatcher::onFloat(const std::string& dest, float value)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        it->second.onFloat(dest, value);
    }
}

void Dispatcher::onSymbol(const std::string& dest, const std::string& symbol)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        it->second.onSymbol(dest, symbol);
    }
}

void Dispatcher::onList(const std::string& dest, const List& list)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        it->second.onList(dest, list);
    }
}

void Dispatcher::onMessage(const std::string& dest, const std::string& msg, const Message& list)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        it->second.onMessage(dest, msg, list);
    }
}

void Dispatcher::subscribe(Receiver& receiver, const string& dest)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);

    if (found.first == found.second) {
        //  No existing subscriber, new bind
        mBinders[dest] = libpd_bind(dest.c_str());
    }

    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        if (&(it->second) == &(receiver)) {
            //  Already subscribed
            return;
        }
    }

    mSubs.insert(make_pair(dest, boost::ref(receiver)));
}

void Dispatcher::unsubscribe(Receiver& receiver, const string& dest)
{
    std::pair<SubsMap::iterator, SubsMap::iterator> found = mSubs.equal_range(dest);
    for (SubsMap::iterator it = found.first; it != found.second; ++it) {
        if (&(it->second) == &receiver) {
            mSubs.erase(it);
            break;
        }
    }

        
    if (mSubs.find(dest) == mSubs.end()) {
        std::map<string, void*>::iterator bind = mBinders.find(dest);
        if (bind != mBinders.end()) {
            libpd_unbind(bind->second);
            mBinders.erase(bind);
        }
    }
}

void Dispatcher::unsubscribeAll()
{
    mSubs.clear();
}

/**  Wraps PdClient calls with a lock on the Pd object  */
class PdLockingClient : public PdClient
{
protected:
    Pd& mPd;

public:
    PdLockingClient(Pd& pd) : mPd(pd) { }

    virtual void* openFile(const char* filename, const ci::fs::path& dir)
    {
        Pd::Lock lock(mPd);
        return PdClient::openFile(filename, dir);
    }

    virtual void addToSearchPath(const ci::fs::path& path)
    {
        Pd::Lock lock(mPd);
        PdClient::addToSearchPath(path);
    }

    virtual int sendBang(const std::string& recv)
    {
        Pd::Lock lock(mPd);
        return PdClient::sendBang(recv);
    }

    virtual int sendFloat(const std::string& recv, float x)
    {
        Pd::Lock lock(mPd);
        return PdClient::sendFloat(recv, x);
    }

    virtual int sendSymbol(const std::string& recv, const std::string& sym)
    {
        Pd::Lock lock(mPd);
        return PdClient::sendSymbol(recv, sym);
    }

    virtual int sendList(const std::string& recv, AtomList& list)
    {
        Pd::Lock lock(mPd);
        return PdClient::sendList(recv, list);
    }

    virtual int sendMessage(const std::string& recv, const std::string& msg, AtomList& list)
    {
        Pd::Lock lock(mPd);
        return PdClient::sendMessage(recv, msg, list);
    }

    virtual void unsubscribeAll()
    {
        Pd::Lock lock(mPd);
        PdClient::unsubscribeAll();
    }
};

DispatcherRef Pd::sDispatcher;

PdRef Pd::create(bool autoLock, const string& dllName)
{
    void* hDLL = NULL;

#if defined( LIBPD_DYNAMIC_LOAD )
    // int err = libpd_dll_load("libpd.dll", &hDLL);
    // int err = libpd_dll_load("/data/data/com.expandingbrain.celaudio/lib/libpdnative.so", &hDLL);
    CI_LOGD("Trying to load PD dll from %s", dllName.c_str());
    int err = libpd_dll_load(dllName.c_str(), &hDLL);
    CI_LOGD("PD dll link errors: %d", err);
#endif

    PdRef pd;
    //  XXX one link error on Android to resolve
    if (err == 0) {
        pd = PdRef(new Pd(autoLock));
        pd->mDLL = hDLL;
    }
    return pd;
}

void Pd::init(int inChannels, int outChannels, int sampleRate)
{
    //  Initialize PD
    libpd_printhook   = (t_libpd_printhook)   cel_printhook;
	libpd_banghook    = (t_libpd_banghook)    cel_banghook;
	libpd_floathook   = (t_libpd_floathook)   cel_floathook;
	libpd_symbolhook  = (t_libpd_symbolhook)  cel_symbolhook;
	libpd_listhook    = (t_libpd_listhook)    cel_listhook;
	libpd_messagehook = (t_libpd_messagehook) cel_messagehook;

	libpd_noteonhook         = (t_libpd_noteonhook)         cel_noteon;
	libpd_controlchangehook  = (t_libpd_controlchangehook)  cel_controlchange;
	libpd_programchangehook  = (t_libpd_programchangehook)  cel_programchange;
	libpd_pitchbendhook      = (t_libpd_pitchbendhook)      cel_pitchbend;
	libpd_aftertouchhook     = (t_libpd_aftertouchhook)     cel_aftertouch;
	libpd_polyaftertouchhook = (t_libpd_polyaftertouchhook) cel_polyaftertouch;
	
	libpd_midibytehook = (t_libpd_midibytehook) cel_midibyte;
	
    libpd_init();
    //sys_debuglevel = 4;
    //sys_verbose = 1;

    // CI_LOGD("PD: Create dispatcher");
    sDispatcher = DispatcherRef(new Dispatcher());

    // CI_LOGD("PD: Create PD");
    // CI_LOGD("PD: Create Audio");
    mAudio = PdAudio::create(*this, inChannels, outChannels, sampleRate);
    // CI_LOGD("PD: Set compute audio to true");
    computeAudio(true);
    // CI_LOGD("PD: Return PD");
}

Pd::Pd(bool autoLock)
{
    mClient = PdClientRef(autoLock ? new PdLockingClient(*this) : new PdClient());
}

PdAudio* Pd::audio()
{
    return mAudio.get();
}

PdClient* Pd::client()
{
    return mClient.get();
}

void PdClient::computeAudio(bool on)
{
    //  Start/stop DSP
    sendMessage("pd", "dsp") << (on ? 1.0f : 0);
}

void* PdClient::openFile(const char* filename, const fs::path& dir)
{
    return dir.empty() ? NULL : libpd_openfile(filename, dir.string().c_str());
}

void PdClient::addToSearchPath(const fs::path& path)
{
    if (!path.empty()) {
        // CI_LOGD("OSL: Adding to PD search path: %s", path.string().c_str());
        libpd_add_to_search_path(path.string().c_str());
    }
}

int PdClient::sendBang(const string& recv)
{
    return libpd_bang(recv.c_str());
}

int PdClient::sendFloat(const string& recv, float x)
{
    return libpd_float(recv.c_str(), x);
}

int PdClient::sendSymbol(const string& recv, const string& sym)
{
    return libpd_symbol(recv.c_str(), sym.c_str());
}

int PdClient::sendList(const std::string& recv, AtomList& list)
{
    return sendMessage(recv, string(), list);
}

int PdClient::sendMessage(const std::string& recv, const std::string& msg, AtomList& list)
{
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

SendChain PdClient::send(const string& recv)
{
    return SendChain(*this, recv);
}

MessageChain PdClient::sendList(const string& recv)
{
    return MessageChain(*this, recv);
}

MessageChain PdClient::sendMessage(const string& recv, const string& msg)
{
    return MessageChain(*this, recv, msg);
}

SubscribeChain PdClient::subscribe(Receiver& receiver)
{
    return SubscribeChain(*this, *Pd::sDispatcher, receiver, SubscribeChain::SUBSCRIBE);
}

SubscribeChain PdClient::unsubscribe(Receiver& receiver)
{
    return SubscribeChain(*this, *Pd::sDispatcher, receiver, SubscribeChain::UNSUBSCRIBE);
}

void PdClient::unsubscribeAll()
{
}

void Pd::lock()
{
    mPdLock.lock();
}

bool Pd::try_lock()
{
    return mPdLock.try_lock();
}

void Pd::unlock()
{
    mPdLock.unlock();
}

Pd::~Pd()
{
}

void Pd::play()
{
    mAudio->play();
}

void Pd::pause()
{
    mAudio->pause();
}

void Pd::close()
{
    mAudio->close();
}

AudioError_t Pd::error()
{
    return mAudio->error();
}

void  Pd::computeAudio(bool on)
{
    mClient->computeAudio(on);
}

void* Pd::openFile(const char* filename, const fs::path& dir)
{
    return mClient->openFile(filename, dir);
}

void Pd::addToSearchPath(const fs::path& path)
{
    mClient->addToSearchPath(path);
}

int Pd::sendBang(const string& recv)
{
    return mClient->sendBang(recv);
}

int Pd::sendFloat(const string& recv, float x)
{
    return mClient->sendFloat(recv, x);
}

int Pd::sendSymbol(const string& recv, const string& sym)
{
    return mClient->sendSymbol(recv, sym);
}

int Pd::sendList(const string& recv, AtomList& list)
{
    return mClient->sendList(recv, list);
}

int Pd::sendMessage(const string& recv, const string& msg, AtomList& list)
{
    return mClient->sendMessage(recv, msg, list);
}

SendChain Pd::send(const string& recv)
{
    return mClient->send(recv);
}

MessageChain Pd::sendList(const string& recv)
{
    return mClient->sendList(recv);
}

MessageChain Pd::sendMessage(const string& recv, const string& msg)
{
    return mClient->sendMessage(recv, msg);
}

SubscribeChain Pd::subscribe(Receiver& receiver)
{
    return mClient->subscribe(receiver);
}

SubscribeChain Pd::unsubscribe(Receiver& receiver)
{
    return mClient->unsubscribe(receiver);
}

void Pd::unsubscribeAll()
{
    mClient->unsubscribeAll();
}

} } // namespace cel::pd
