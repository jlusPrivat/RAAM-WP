#include "audiosession.h"



AudioSession::AudioSession (IAudioSessionControl* c, QObject *parent)
    : QObject(parent) {
    IAudioSessionControl2 *pSessControl2 = nullptr;

    // handle the incoming IAudioSessionControl
    c->AddRef();
    control = c;
    systemNotifier = new Notifier(this);
    hr = control->RegisterAudioSessionNotification(systemNotifier);
    FAILCATCH;

    // acquire the ISimpleAudioVolume interface
    hr = control->QueryInterface(
                __uuidof(IAudioSessionControl2),
                reinterpret_cast<void**>(&pSessControl2));
    FAILCATCH;
    hr = pSessControl2->QueryInterface(
                __uuidof(ISimpleAudioVolume),
                reinterpret_cast<void**>(&simpleVolume));
    FAILCATCH;
    systemSoundSession = (pSessControl2->IsSystemSoundsSession() == S_OK);
    hr = S_OK;

    // connect all signals
    connect(systemNotifier, &Notifier::sigSessionDisplayNameChanged, this, [=]{
        sigDisplayNameChanged(this);
    }, Qt::BlockingQueuedConnection);
    connect(systemNotifier, &Notifier::sigSessionGroupingParamChanged, this, [=]{
        sigGroupingParamChanged(this);
    }, Qt::BlockingQueuedConnection);
    connect(systemNotifier, &Notifier::sigSessionIconPathChanged, this, [=]{
        sigIconPathChanged(this);
    }, Qt::BlockingQueuedConnection);
    connect(systemNotifier, &Notifier::sigSessionDisconnected, this, [=]{
        sigSessionRemoved(this);
    }, Qt::BlockingQueuedConnection);
    connect(systemNotifier, &Notifier::sigSessionVolumeOrMuteChanged, this, [=]{
        sigVolumeOrMuteChanged(this);
    }, Qt::BlockingQueuedConnection);
    connect(systemNotifier, &Notifier::sigSessionStateChanged, this, [=]{
        AudioSessionState state;
        control->GetState(&state);
        if (state == AudioSessionStateExpired)
            sigSessionRemoved(this);
    }, Qt::BlockingQueuedConnection);

done:
    SafeRelease(&pSessControl2);
}



AudioSession::~AudioSession () {
    control->UnregisterAudioSessionNotification(systemNotifier);
    SafeRelease(&control);
    SafeRelease(&simpleVolume);
}



HRESULT AudioSession::getInternalState () {
    return hr;
}



void AudioSession::setVolume (float vol) {
    simpleVolume->SetMasterVolume(vol, nullptr);
}



float AudioSession::getVolume () {
    float rv = 0;
    hr = simpleVolume->GetMasterVolume(&rv);
    if (FAILED(hr))
        sigErrored(hr);
    return rv;
}



void AudioSession::setMute (bool m) {
    hr = simpleVolume->SetMute(m, nullptr);
    if (FAILED(hr))
        sigErrored(hr);
}



bool AudioSession::getMute () {
    int rv = 1;
    hr = simpleVolume->GetMute(&rv);
    if (FAILED(hr))
        sigErrored(hr);
    return rv;
}



GUID AudioSession::getGroupingParam () {
    GUID rv = GUID_NULL;
    hr = control->GetGroupingParam(&rv);
    if (FAILED(hr))
        sigErrored(hr);
    return rv;
}



QString AudioSession::getDisplayName () {
    LPWSTR rv = nullptr;
    hr = control->GetDisplayName(&rv);
    if (FAILED(hr))
        sigErrored(hr);
    return QString::fromWCharArray(rv);
}



QString AudioSession::getIconPath () {
    LPWSTR rv = nullptr;
    hr = control->GetIconPath(&rv);
    if (FAILED(hr))
        sigErrored(hr);
    return QString::fromWCharArray(rv);
}



bool AudioSession::isSystemSoundSession () {
    return systemSoundSession;
}
