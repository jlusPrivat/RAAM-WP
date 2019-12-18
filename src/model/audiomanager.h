#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <QList>
#include "utils/SafeRelease.h"
#include "utils/makros.h"
#include "model/outputdevice.h"


class AudioManager: public QObject, IMMNotificationClient {
    Q_OBJECT


public:
    AudioManager(QObject* = nullptr);
    ~AudioManager();
    HRESULT getInternalStatus();

    // For IMMNotificationClient
    HRESULT __stdcall OnDefaultDeviceChanged(EDataFlow, ERole, LPCWSTR) override;
    HRESULT __stdcall OnDeviceAdded(LPCWSTR) override;
    HRESULT __stdcall OnDeviceRemoved(LPCWSTR) override;
    HRESULT __stdcall OnDeviceStateChanged(LPCWSTR, DWORD) override;
    HRESULT __stdcall OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override;
    // I trust Windows enough not to make weird errors, so just simply ignore methods
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;
    HRESULT __stdcall QueryInterface(REFIID, void**) override;


private:
    bool disableSignalling = false;
    HRESULT hr = S_OK;

    // pointers
    IMMDeviceEnumerator *pDeviceEnumerator = nullptr;

    // properties
    QList<OutputDevice*> outputDevices;

    // methods
    HRESULT init();
    bool findOutputDevice(LPCWSTR, OutputDevice**);
    bool findDefaultOutputDevice(OutputDevice**);


};

#endif // AUDIOMANAGER_H
