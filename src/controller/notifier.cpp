#include "notifier.h"



Notifier::Notifier (QObject* parent)
    : QObject(parent),
      propEDataFlow(eAll),
      propERole(eConsole),
      propLPCWSTR(nullptr),
      propDWORD(0),
      propPROPERTYKEY(_tagpropertykey()),
      propAudioSessionControl(nullptr) {
}



HRESULT __stdcall Notifier::OnDefaultDeviceChanged (EDataFlow a, ERole b, LPCWSTR c) {
    mutex.lock();
    propEDataFlow.set(a);
    propERole.set(b);
    propLPCWSTR.set(c);
    sigDefaultDeviceChanged(this);
    resetProperties();
    mutex.unlock();
    return S_OK;
}



HRESULT __stdcall Notifier::OnDeviceAdded (LPCWSTR a) {
    mutex.lock();
    propLPCWSTR.set(a);
    sigDeviceAdded(this);
    resetProperties();
    mutex.unlock();
    return S_OK;
}



HRESULT __stdcall Notifier::OnDeviceRemoved (LPCWSTR a) {
    mutex.lock();
    propLPCWSTR.set(a);
    sigDeviceRemoved(this);
    resetProperties();
    mutex.unlock();
    return S_OK;
}



HRESULT __stdcall Notifier::OnDeviceStateChanged (LPCWSTR a, DWORD b) {
    mutex.lock();
    propLPCWSTR.set(a);
    propDWORD.set(b);
    sigDeviceStateChanged(this);
    resetProperties();
    mutex.unlock();
    return S_OK;
}



HRESULT __stdcall Notifier::OnPropertyValueChanged (LPCWSTR a, const PROPERTYKEY b) {
    mutex.lock();
    propLPCWSTR.set(a);
    propPROPERTYKEY.set(b);
    sigPropertyValueChanged(this);
    resetProperties();
    mutex.unlock();
    return S_OK;
}



HRESULT __stdcall Notifier::OnNotify (PAUDIO_VOLUME_NOTIFICATION_DATA) {
    mutex.lock();
    sigDeviceVolumeOrMuteChanged(this);
    mutex.unlock();
    return S_OK;
}



HRESULT __stdcall Notifier::OnSessionCreated (IAudioSessionControl *control) {
    mutex.lock();
    propAudioSessionControl.set(control);
    sigSessionCreated(this);
    resetProperties();
    mutex.unlock();
    return S_OK;
}



HRESULT __stdcall Notifier::OnChannelVolumeChanged (DWORD, float*, DWORD, LPCGUID) {
    // ignore this event
    return S_OK;
}



HRESULT __stdcall Notifier::OnDisplayNameChanged (LPCWSTR, LPCGUID) {
    sigSessionDisplayNameChanged();
    return S_OK;
}



HRESULT __stdcall Notifier::OnGroupingParamChanged (LPCGUID, LPCGUID) {
    sigSessionGroupingParamChanged();
    return S_OK;
}



HRESULT __stdcall Notifier::OnIconPathChanged (LPCWSTR, LPCGUID) {
    sigSessionIconPathChanged();
    return S_OK;
}



HRESULT __stdcall Notifier::OnSessionDisconnected (AudioSessionDisconnectReason) {
    sigSessionDisconnected();
    return S_OK;
}



HRESULT __stdcall Notifier::OnSimpleVolumeChanged (float, WINBOOL, LPCGUID) {
    sigSessionVolumeOrMuteChanged();
    return S_OK;
}



HRESULT __stdcall Notifier::OnStateChanged (AudioSessionState) {
    sigSessionStateChanged();
    return S_OK;
}



ULONG __stdcall Notifier::AddRef () {
    return 1;
}



ULONG __stdcall Notifier::Release () {
    return 0;
}



HRESULT __stdcall Notifier::QueryInterface (REFIID, void** ppvInterface) {
    *ppvInterface = nullptr;
    return E_NOINTERFACE;
}



void Notifier::resetProperties () {
    propEDataFlow.isSet = false;
    propERole.isSet = false;
    propLPCWSTR.isSet = false;
    propDWORD.isSet = false;
    propPROPERTYKEY.isSet = false;
    propAudioSessionControl.isSet = false;
}
