#include "outputdevice.h"



OutputDevice::OutputDevice (IMMDevice *pMMDevice, QObject *parent)
    : QObject(parent) {
    this->pMMDevice = pMMDevice;
    pMMDevice->AddRef();
    init();
}



OutputDevice::~OutputDevice () {
    SafeRelease(&pMMDevice);
    SafeRelease(&pPropertyStore);
    if (pEndpointId)
        CoTaskMemFree(&pEndpointId);
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



void OutputDevice::updateProperty (const PROPERTYKEY propKey) {
    if (propKey.fmtid == PKEY_Device_FriendlyName.fmtid)
        updateDescriptionLong();
    else if (propKey.fmtid == PKEY_Device_DeviceDesc.fmtid)
        updateDescriptionShort();
    /* Not working, see Github issue
    else if (propKey.fmtid == PKEY_AudioEndpoint_FormFactor.fmtid)
        updateFormFactor();
    else if (propKey.fmtid == PKEY_AudioEndpoint_ControlPanelPageProvider.fmtid)
        updateControlPanelPageProvider();*/
}



HRESULT OutputDevice::init () {
    // get the endpointID
    //hr = pMMDevice->GetId(&pEndpointId); !!!
    FAILCATCH;

    // get the device state
    hr = pMMDevice->GetState(&state);
    FAILCATCH;

    // get the property store
    hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
    FAILCATCH;

    // update the properties
    disableSignalling = true;
    hr = updateDescriptionLong();
    FAILCATCH;
    hr = updateDescriptionShort();
    FAILCATCH;
    hr = updateFormFactor();
    FAILCATCH;
    hr = updateControlPanelPageProvider();

done:
    disableSignalling = false;
    if (!disableSignalling && FAILED(hr))
        sigInternalStatusError(hr);
    else if (!disableSignalling) {
        sigEndpointIdChanged(pEndpointId);
        sigStatusChanged(&state);
    }
    return hr;
}



HRESULT OutputDevice::updateDescriptionLong () {
    PROPVARIANT propvariant;
    PropVariantInit(&propvariant);
    hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propvariant);
    FAILCATCH;
    if (propvariant.vt == VT_LPWSTR && descriptionLong != (*propvariant.pwszVal))
        descriptionLong = *propvariant.pwszVal;

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
        descriptionShort = *propvariant.pwszVal;

done:
    PropVariantClear(&propvariant);
    if (!disableSignalling && FAILED(hr))
        sigInternalStatusError(hr);
    else if (!disableSignalling)
        sigDescriptionShortChanged(&descriptionShort);
    return hr;
}



HRESULT OutputDevice::updateFormFactor () {
    /* Not working, see Github issue
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



HRESULT OutputDevice::updateControlPanelPageProvider () {
    /* Not working, see Github issue
    PROPVARIANT propvariant;
    PropVariantInit(&propvariant);
    CLSID newCPPP = CLSID_NULL;

    hr = pPropertyStore->GetValue(PKEY_AudioEndpoint_ControlPanelPageProvider,
                                  &propvariant);
    FAILCATCH;
    if (propvariant.vt != VT_LPWSTR)
        goto done;
    hr = CLSIDFromString(propvariant.pwszVal, &newCPPP);
    FAILCATCH;
    if (controlPanelPageProvider != newCPPP)
        controlPanelPageProvider = newCPPP;

done:
    PropVariantClear(&propvariant);
    if (!disableSignalling && FAILED(hr))
        sigInternalStatusError(hr);
    else if (!disableSignalling)
        sigControlPanelPageProviderChanged(&controlPanelPageProvider);*/
    return hr;
}
