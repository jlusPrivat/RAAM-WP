#include "outputdevice.h"



OutputDevice::OutputDevice (IMMDevice *pDevice, QObject *parent)
    : QObject(parent) {
    pMMDevice = pDevice;
    pMMDevice->AddRef();
    init();
}



OutputDevice::~OutputDevice () {
    if (pEndpointVolume)
        pEndpointVolume->UnregisterControlChangeNotify(systemNotifier);
    if (pEndpointId)
        CoTaskMemFree(pEndpointId);
    if (pSessManager2)
        pSessManager2->UnregisterSessionNotification(systemNotifier);
    SafeRelease(&pEndpointVolume);
    SafeRelease(&pSessManager2);
    SafeRelease(&pMMDevice);
    SafeRelease(&pPropertyStore);
}



void OutputDevice::setMute (bool m) {
    pEndpointVolume->SetMute(m, nullptr);
}



bool OutputDevice::getMute () {
    int rv = 1;
    if (pEndpointVolume)
        pEndpointVolume->GetMute(&rv);
    return rv;
}



void OutputDevice::setVolume (float v) {
    if (pEndpointVolume)
        pEndpointVolume->SetMasterVolumeLevelScalar(v, nullptr);
}



float OutputDevice::getVolume () {
    float rv = 0;
    if (pEndpointVolume)
        pEndpointVolume->GetMasterVolumeLevelScalar(&rv);
    return rv;
}



HRESULT OutputDevice::getInternalStatus () {
    return hr;
}



bool OutputDevice::compareEndpointId (LPCWSTR other) {
    return !wcscmp(pEndpointId, other);
}



bool OutputDevice::getIsDefaultOutput () {
    return isDefaultOutput;
}



void OutputDevice::setIsDefaultOutput (bool newVal) {
    if (newVal != isDefaultOutput) {
        isDefaultOutput = newVal;
        sigIsDefaultOutputChanged(&isDefaultOutput);
    }
}



void OutputDevice::setState (DWORD newState) {
    if (newState != state) {
        state = newState;
        sigStatusChanged(&state);
    }
}



DWORD OutputDevice::getState () {
    return state;
}



void OutputDevice::updateProperty (const PROPERTYKEY propKey) {
    if (propKey.fmtid == PKEY_Device_FriendlyName.fmtid)
        updateDescriptionLong();
    else if (propKey.fmtid == PKEY_Device_DeviceDesc.fmtid)
        updateDescriptionShort();
    /* Not working, see Github issue
    else if (propKey.fmtid == PKEY_AudioEndpoint_FormFactor.fmtid)
        updateFormFactor();*/
}



QString OutputDevice::getDescriptionLong () {
    return descriptionLong;
}



QString OutputDevice::getDescriptionShort () {
    return descriptionShort;
}



EndpointFormFactor OutputDevice::getFormFactor () {
    return formFactor;
}



HRESULT OutputDevice::init () {
    IAudioSessionEnumerator *pSessEnumerator = nullptr;
    IAudioSessionControl *pSessControl = nullptr;
    int sessionCounter = 0;

    // create the notifier object
    if (!systemNotifier)
        systemNotifier = new Notifier(this);

    // get the endpointID
    hr = pMMDevice->GetId(&pEndpointId);
    FAILCATCH;

    // get the device state
    hr = pMMDevice->GetState(&state);
    FAILCATCH;

    // get the endpoint volume
    if (state == DEVICE_STATE_ACTIVE && !pEndpointVolume) {
        hr = pMMDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                                 nullptr, reinterpret_cast<void**>(&pEndpointVolume));
        FAILCATCH;
        connect(systemNotifier, &Notifier::sigVolumeOrMuteChanged,
                this, &OutputDevice::sigVolumeOrMuteChanged,
                Qt::QueuedConnection);
        pEndpointVolume->RegisterControlChangeNotify(systemNotifier);
    }

    // get the property store
    hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
    FAILCATCH;

    // update the properties
    // (disable prohibits double fault signalling and false updates)
    disableSignalling = true;
    hr = updateDescriptionLong();
    FAILCATCH;
    hr = updateDescriptionShort();
    FAILCATCH;
    hr = updateFormFactor();
    FAILCATCH;

    // register for new sessions
    hr = pMMDevice->Activate(__uuidof(IAudioSessionManager2),
                             CLSCTX_ALL,
                             nullptr,
                             reinterpret_cast<void**>(&pSessManager2));
    FAILCATCH;
    hr = pSessManager2->RegisterSessionNotification(systemNotifier);
    FAILCATCH;

    // enumerate all devices
    hr = pSessManager2->GetSessionEnumerator(&pSessEnumerator);
    FAILCATCH;
    hr = pSessEnumerator->GetCount(&sessionCounter);
    FAILCATCH;
    for (int i = 0; i < sessionCounter; i++) {
        SafeRelease(&pSessControl);
        hr = pSessEnumerator->GetSession(i, &pSessControl);
        FAILCATCH;
        addSession(pSessControl);
    }

done:
    disableSignalling = false;
    if (!disableSignalling && FAILED(hr))
        sigInternalStatusError(hr);
    else if (!disableSignalling) {
        sigEndpointIdChanged(pEndpointId);
        sigStatusChanged(&state);
    }
    SafeRelease(&pSessManager2);
    SafeRelease(&pSessEnumerator);
    SafeRelease(&pSessControl);
    return hr;
}



HRESULT OutputDevice::updateDescriptionLong () {
    PROPVARIANT propvariant;
    PropVariantInit(&propvariant);
    hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propvariant);
    FAILCATCH;
    if (propvariant.vt == VT_LPWSTR && descriptionLong != (*propvariant.pwszVal))
        descriptionLong = QString::fromWCharArray(propvariant.pwszVal);

done:
    PropVariantClear(&propvariant);
    if (!disableSignalling && FAILED(hr))
        sigInternalStatusError(hr);
    else if (!disableSignalling)
        sigDescriptionLongChanged(&descriptionLong);
    return hr;
}



HRESULT OutputDevice::updateDescriptionShort () {
    PROPVARIANT propvariant;
    PropVariantInit(&propvariant);
    hr = pPropertyStore->GetValue(PKEY_Device_DeviceDesc, &propvariant);
    FAILCATCH;
    if (propvariant.vt == VT_LPWSTR && descriptionShort != (*propvariant.pwszVal))
        descriptionShort = QString::fromWCharArray(propvariant.pwszVal);

done:
    PropVariantClear(&propvariant);
    if (!disableSignalling && FAILED(hr))
        sigInternalStatusError(hr);
    else if (!disableSignalling)
        sigDescriptionShortChanged(&descriptionShort);
    return hr;
}



HRESULT OutputDevice::updateFormFactor () {
    /* Not working, see Github issue https://github.com/jlusPrivat/RAAM-WP/issues/1
    PROPVARIANT propvariant;
    PropVariantInit(&propvariant);
    EndpointFormFactor newFormFactor;

    hr = pPropertyStore->GetValue(PKEY_AudioEndpoint_FormFactor, &propvariant);
    FAILCATCH;
    if (propvariant.vt != VT_UI4)
        goto done;
    newFormFactor = static_cast<EndpointFormFactor>(propvariant.uintVal);
    if (formFactor != newFormFactor)
        formFactor = newFormFactor;

done:
    PropVariantClear(&propvariant);
    if (!disableSignalling && FAILED(hr))
        sigInternalStatusError(hr);
    else if (!disableSignalling)
        sigFormFactorChanged(&formFactor);*/
    return hr;
}



HRESULT OutputDevice::addSession (IAudioSessionControl *control) {
    hr = S_OK;

    // add to session group, if grouping param is known

    if (!disableSignalling)
        sigSessionAdded(new AudioSessionGroup(control, this));

    if (!disableSignalling && FAILED(hr))
        sigInternalStatusError(hr);
    return hr;
}
