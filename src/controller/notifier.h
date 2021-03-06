#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <QObject>
#include <QMutex>



/**
 * @brief The Notifier class
 *
 * Is used for handling callbacks by windows.
 * Its signals must be connected using Qt::BlockingQueuedConnection
 */
class Notifier: public QObject,
        public IMMNotificationClient,
        public IAudioEndpointVolumeCallback,
        public IAudioSessionNotification,
        public IAudioSessionEvents {
    Q_OBJECT


public:
    // classes
    template <typename T> struct PropWrapper {
        PropWrapper(T initial): prop(initial) {}
        T prop;
        bool isSet = false;

        void set (T nv) {prop = nv; isSet = true;}
        T get () {if (!isSet) qWarning("undefined property value"); return prop;}
    };

    Notifier(QObject* = nullptr);

    // properties
    QMutex mutex;
    PropWrapper<EDataFlow> propEDataFlow;
    PropWrapper<ERole> propERole;
    PropWrapper<LPCWSTR> propLPCWSTR;
    PropWrapper<DWORD> propDWORD;
    PropWrapper<PROPERTYKEY> propPROPERTYKEY;
    PropWrapper<IAudioSessionControl*> propAudioSessionControl;

    // For IMMNotificationClient
    HRESULT __stdcall OnDefaultDeviceChanged(EDataFlow, ERole, LPCWSTR) override;
    HRESULT __stdcall OnDeviceAdded(LPCWSTR) override;
    HRESULT __stdcall OnDeviceRemoved(LPCWSTR) override;
    HRESULT __stdcall OnDeviceStateChanged(LPCWSTR, DWORD) override;
    HRESULT __stdcall OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override;

    // for the IAudioEndpointVolumeCallback
    HRESULT __stdcall OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override;

    // for the IAudioSessionNotification
    HRESULT __stdcall OnSessionCreated(IAudioSessionControl*) override;

    // for the IAudioSessionEvents
    HRESULT __stdcall OnChannelVolumeChanged(DWORD, float*, DWORD, LPCGUID) override;
    HRESULT __stdcall OnDisplayNameChanged(LPCWSTR, LPCGUID) override;
    HRESULT __stdcall OnGroupingParamChanged(LPCGUID, LPCGUID) override;
    HRESULT __stdcall OnIconPathChanged(LPCWSTR, LPCGUID) override;
    HRESULT __stdcall OnSessionDisconnected(AudioSessionDisconnectReason) override;
    HRESULT __stdcall OnSimpleVolumeChanged(float, WINBOOL, LPCGUID) override;
    HRESULT __stdcall OnStateChanged(AudioSessionState) override;

    // Only for fulfilling the specs - Not doing a thing
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;
    HRESULT __stdcall QueryInterface(REFIID, void**) override;


signals:
    void sigDefaultDeviceChanged(Notifier*);
    void sigDeviceAdded(Notifier*);
    void sigDeviceRemoved(Notifier*);
    void sigDeviceStateChanged(Notifier*);
    void sigPropertyValueChanged(Notifier*);
    void sigDeviceVolumeOrMuteChanged(Notifier*);
    void sigSessionCreated(Notifier*);
    void sigSessionDisplayNameChanged();
    void sigSessionGroupingParamChanged();
    void sigSessionIconPathChanged();
    void sigSessionDisconnected();
    void sigSessionVolumeOrMuteChanged();
    void sigSessionStateChanged();


private:
    void resetProperties();


};



#endif // NOTIFIER_H
