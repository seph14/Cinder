#pragma once

#include <vector>  
#include <string>

#include "cinder/Cinder.h"
#include "cinder/Thread.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace cel {

enum AudioError_t {
    NONE,
};

typedef std::shared_ptr<class CelPd> CelPdRef;

class CelPd
{
public:
    //  Create and initialize the audio system
    static CelPdRef init(int inChannels, int outChannels, int sampleRate);

    //  Start the audio system playing
    void play();

    //  Pause the audio system
    void pause();

    //  Shut down audio system
    void close();

    //  Returns last error code
    AudioError_t error();

    ~CelPd();

protected:
    SLObjectItf mEngineObject;
    SLEngineItf mEngineEngine;
    SLObjectItf mOutputMixObject;

    SLObjectItf                   bqPlayerObject;
    SLPlayItf                     bqPlayerPlay;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
    SLEffectSendItf               bqPlayerEffectSend;
    SLMuteSoloItf                 bqPlayerMuteSolo;
    SLVolumeItf                   bqPlayerVolume;

    std::mutex                   mMixerLock;
    std::condition_variable      mBufferReady;
    std::shared_ptr<std::thread> mMixerThread;

    AudioError_t mError;

    CelPd(int inChannels, int outChannels, int sampleRate);

    void initSL(int inChannels, int outChannels, int sampleRate);
    void setError(AudioError_t error);
    void mixer();

    bool mMixerRunning;
    bool mOutputReady;

    int       mOutputBufIndex;
    int16_t* mOutputBuf[2];
    int       mInputBufIndex;
    int16_t* mInputBuf[2];

    int       mBufSamples;

    static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
};

}
