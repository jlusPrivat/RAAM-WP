#include "audiomanager.h"



AudioManager::AudioManager (QObject *parent)
    : QObject(parent) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    this->init();
}



AudioManager::~AudioManager () {
    pDeviceEnumerator->UnregisterEndpointNotificationCallback(this);
    SafeRelease(&pDeviceEnumerator);
    CoUninitialize();
}



HRESULT AudioManager::getInternalStatus () {
    return hr;
}



HRESULT __stdcall AudioManager::OnDefaultDeviceChanged (EDataFlow flow,
                                              ERole role, LPCWSTR deviceId) {
    // only use incoming data from console and ignore other default outputs
    if (flow == eRender && role == eConsole) {
        // reset old default OutputDevice to non default
        OutputDevice *device = nullptr;
        if (findDefaultOutputDevice(&device))
            device->setIsDefaultOutput(false);
        // set new default OutputDevice, if provided
        if (deviceId) {
            device = nullptr;
            if (findOutputDevice(deviceId, &device))
                device->setIsDefaultOutput(true);
        }
    }
    return S_OK;
}



HRESULT __stdcall AudioManager::OnDeviceAdded (LPCWSTR deviceId) {
    IMMDevice *pNewMMDevice = nullptr;
    OutputDevice *pNewOutputDevice = nullptr;

    // find new device, create object
    hr = pDeviceEnumerator->GetDevice(deviceId, &pNewMMDevice);
    FAILCATCH;
    pNewOutputDevice = new OutputDevice(pNewMMDevice, this);

    // check object integrity and insert into list
    hr = pNewOutputDevice->getInternalStatus();
    FAILCATCH_DELETE(pNewOutputDevice);
    outputDevices.append(pNewOutputDevice);

done:
    SafeRelease(&pNewMMDevice);
    return S_OK;
}



HRESULT __stdcall AudioManager::OnDeviceRemoved (LPCWSTR deviceId) {
    OutputDevice *pOutputDevice = nullptr;
    if (findOutputDevice(deviceId, &pOutputDevice)) {
        delete(pOutputDevice);
        outputDevices.removeAll(pOutputDevice);
    }
    return S_OK;
}



HRESULT __stdcall AudioManager::OnDeviceStateChanged (LPCWSTR deviceId, DWORD newState) {
    // only update, if new state is active or unplugged
    if (newState & (DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED)) {
        OutputDevice *device = nullptr;
        // update available device
        if (findOutputDevice(deviceId, &device))
            device->setState(newState);
        // create new device
        else
            OnDeviceAdded(deviceId);
    }
    // otherwise delete OutputDevice, because it is not present or disabled
    else
        OnDeviceRemoved(deviceId);
    return S_OK;
}



HRESULT __stdcall AudioManager::OnPropertyValueChanged (LPCWSTR deviceId,
                                              const PROPERTYKEY propKey) {
    OutputDevice *device = nullptr;
    // update found device
    if (findOutputDevice(deviceId, &device)) {
        device->updateProperty(propKey);
    }
    // device not found, try recovering from invalid state by adding it
    else
        OnDeviceAdded(deviceId);
    return S_OK;
}



ULONG __stdcall AudioManager::AddRef () {
    return 1;
}



ULONG __stdcall AudioManager::Release () {
    return 0;
}



HRESULT __stdcall AudioManager::QueryInterface (REFIID, void** ppvInterface) {
    *ppvInterface = nullptr;
    return E_NOINTERFACE;
}



HRESULT AudioManager::init () {
    // initialize needed vars
    IMMDevice *pDefaultDevice = nullptr;
    LPWSTR pDefaultDeviceId = nullptr;
    IMMDeviceCollection *pDeviceCollection = nullptr;
    uint devicesCount = 0;
    IMMDevice *pCDevice = nullptr;
    OutputDevice *outputDevice = nullptr;

    // get device enumerator
    hr = CoCreateInstance(
                __uuidof(MMDeviceEnumerator),
                nullptr,
                CLSCTX_ALL,
                IID_PPV_ARGS(&pDeviceEnumerator)
                );
    FAILCATCH;

    // get all devices and create instances for them
    // only instances, that are either active or unplugged
    // => no deactivated or unavailable interfaces
    hr = pDeviceEnumerator->EnumAudioEndpoints(
                eRender,
                DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,
                &pDeviceCollection);
    FAILCATCH;
    hr = pDeviceCollection->GetCount(&devicesCount);
    FAILCATCH;
    for (uint i = 0; i < devicesCount; i++) {
        // get device and create instance; check, if it is the default output
        // then release the IMMDevice again, as it is being hold by the newly
        // created instance
        hr = pDeviceCollection->Item(i, &pCDevice);
        FAILCATCH;
        outputDevice = new OutputDevice(pCDevice, this);
        SafeRelease(&pCDevice);
        hr = outputDevice->getInternalStatus();
        FAILCATCH_DELETE(outputDevice);

        // insert instance to list and set isDefaultOutput property
        outputDevices.append(outputDevice);
    }

    // get the default audio output (for consoles) and its Id
    hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDefaultDevice);
    FAILCATCH;
    hr = pDefaultDevice->GetId(&pDefaultDeviceId);
    FAILCATCH;

    // find the default device and set it as standard
    outputDevice = nullptr;
    if (findOutputDevice(pDefaultDeviceId, &outputDevice))
        outputDevice->setIsDefaultOutput(true);

    // register itself as MMNotificationClient
    pDeviceEnumerator->RegisterEndpointNotificationCallback(this);

done:
    SafeRelease(&pDefaultDevice);
    SafeRelease(&pDeviceCollection);
    if (pDefaultDeviceId)
        CoTaskMemFree(pDefaultDeviceId);
    return hr;
}



bool AudioManager::findOutputDevice (LPCWSTR endpointId, OutputDevice **ppOutputDevice) {
    uint count = outputDevices.count();
    for (uint i = 0; i < count; i++) {
        OutputDevice *ce = outputDevices.at(i);
        if (ce->compareEndpointId(endpointId)) {
            *ppOutputDevice = ce;
            return true;
        }
    }
    return false;
}



bool AudioManager::findDefaultOutputDevice (OutputDevice **ppOutputDevice) {
    uint count = outputDevices.count();
    for (uint i = 0; i < count; i++) {
        OutputDevice *ce = outputDevices.at(i);
        if (ce->getIsDefaultOutput()) {
            *ppOutputDevice = ce;
            return true;
        }
    }
    return false;
}
