#include "audiomanager.h"


AudioManager::AudioManager () {
    status = this->init();
}



AudioManager::~AudioManager () {
    // delete all active sessions
    int size = sessions.size();
    for (int i = 0; i < size; i++)
        delete(sessions.at(i));
}



HRESULT AudioManager::getStatus () {
    return status;
}



HRESULT AudioManager::addSession (IAudioSessionControl *pSessControl) {
    HRESULT hr = S_OK;

    // initialize needed vars
    IAudioSessionControl2 *pSessControl2 = nullptr;
    ISimpleAudioVolume *pSimpleAudioVol = nullptr;

    // get the AudioSessionControl2
    hr = pSessControl->QueryInterface(
                __uuidof(IAudioSessionControl2),
                reinterpret_cast<void**>(&pSessControl2));
    FAILCATCH;

    // get the SimpleAudioVolume
    hr = pSessControl2->QueryInterface(
                __uuidof(ISimpleAudioVolume),
                reinterpret_cast<void**>(&pSimpleAudioVol));

done:
    SafeRelease(&pSessControl2);
    SafeRelease(&pSimpleAudioVol);
    return hr;
}



HRESULT AudioManager::init () {
    HRESULT hr = S_OK;

    // initialize needed vars
    IMMDeviceEnumerator *pDeviceEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;
    IAudioSessionManager2 *pSessionManager2 = nullptr;
    IAudioSessionEnumerator *pSessionEnumerator = nullptr;
    int countSessions;
    IAudioSessionControl *pSessControl = nullptr;

    // get device enumerator
    hr = CoCreateInstance(
                __uuidof(MMDeviceEnumerator),
                nullptr,
                CLSCTX_ALL,
                IID_PPV_ARGS(&pDeviceEnumerator)
                );
    FAILCATCH;

    // get the default audio output (for consoles)
    // !!! only for consoles? (games, system, etc.), no  music, video
    pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    FAILCATCH;

    // get the session manager2
    hr = pDevice->Activate(
                __uuidof(IAudioSessionManager2),
                CLSCTX_ALL,
                nullptr,
                reinterpret_cast<void**>(&pSessionManager2));
    FAILCATCH;

    // iterate over all sessions
    hr = pSessionManager2->GetSessionEnumerator(&pSessionEnumerator);
    FAILCATCH;
    hr = pSessionEnumerator->GetCount(&countSessions);
    FAILCATCH;
    for (int i = 0; i < countSessions; i++) {
        // get the AudioSessionControl
        SafeRelease(&pSessControl);
        hr = pSessionEnumerator->GetSession(i, &pSessControl);
        FAILCATCH;

        // create new session
        addSession(pSessControl);
    }

done:
    SafeRelease(&pDeviceEnumerator);
    SafeRelease(&pDevice);
    SafeRelease(&pSessionManager2);
    SafeRelease(&pSessionEnumerator);
    SafeRelease(&pSessControl);
    return hr;
}
