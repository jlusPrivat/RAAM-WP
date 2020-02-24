#ifndef OUTPUTDEVICE_H
#define OUTPUTDEVICE_H

#include <QObject>
#include <QUuid>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include "controller/notifier.h"
#include "model/audiosessiongroup.h"
#include "utils/SafeRelease.h"
#include "utils/makros.h"

// copied makro from Windows SDK functiondisconverykeys_devpkey.h
// may need to be removed in future versions, when qt header
// files are more complete
DEFINE_PROPERTYKEY(PKEY_Device_DeviceDesc, 0xa45c254e, 0xdf1c, \
                   0x4efd, 0x80, 0x20, 0x67, 0xd1, \
                   0x46, 0xa8, 0x50, 0xe0, 2);
                   // DEVPROP_TYPE_STRING



/**
 * \brief The OutputDevice class
 *
 * An OutputDevice is the model equivalent of the ms
 * Audio Endpoint Device. It therefore enables to
 * get some properties and set mute and volume.
 */
class OutputDevice: public QObject {
    Q_OBJECT


public:
    // properties
    LPWSTR pEndpointId = nullptr;
    QList<AudioSessionGroup*> audioSessionGroups;

    // methods
    /**
     * passed endpoint device must be released by the owner
     * after calling constructor. Does nothing but to keep
     * own reference
     */
    OutputDevice(IMMDevice*, QObject* = nullptr);
    ~OutputDevice();
    bool findAudioSessionGroup(GUID, AudioSessionGroup**);
    bool findAudioSessionGroup(QString, AudioSessionGroup**);

    void setMute(bool);
    bool getMute();
    void setVolume(float);
    float getVolume();
    /// returns the last status issued by the ms core apis
    HRESULT getInternalStatus();
    bool compareEndpointId(LPCWSTR);
    /// only internal model
    void setIsDefaultOutput(bool);
    bool getIsDefaultOutput();
    /// only internal model
    void setState(DWORD);
    DWORD getState();
    /// only internal model
    void updateProperty(const PROPERTYKEY);
    QString getDescriptionLong();
    QString getDescriptionShort();
    EndpointFormFactor getFormFactor();


signals:
    /// sent, whenever an api error occured
    void sigErrored(HRESULT);
    void sigEndpointIdChanged(LPCWSTR);
    void sigStatusChanged(const DWORD*);
    void sigDescriptionLongChanged(const QString*);
    void sigDescriptionShortChanged(const QString*);
    void sigIsDefaultOutputChanged(const bool*);
    void sigFormFactorChanged(const EndpointFormFactor*);
    void sigVolumeOrMuteChanged();
    void sigSessionGroupAdded(AudioSessionGroup*);
    void sigSessionGroupRemoved(AudioSessionGroup*);
    void sigSessionVolumeOrMuteChanged(AudioSessionGroup*);
    void sigSessionDisplayNameChanged(AudioSessionGroup*);
    void sigSessionIconChanged(AudioSessionGroup*);


private:
    bool disableSignalling = false;
    HRESULT hr = S_OK;

    // pointers
    IMMDevice *pMMDevice = nullptr;
    IAudioEndpointVolume *pEndpointVolume = nullptr;
    IPropertyStore *pPropertyStore = nullptr;
    IAudioSessionManager2 *pSessManager2 = nullptr;

    // properties
    Notifier *systemNotifier = nullptr;
    DWORD state = DEVICE_STATE_ACTIVE;
    bool isDefaultOutput = false;
    QString descriptionLong = "";
    QString descriptionShort = "";
    EndpointFormFactor formFactor = UnknownFormFactor; // INOP

    // methods
    HRESULT init();
    HRESULT updateDescriptionLong();
    HRESULT updateDescriptionShort();
    HRESULT updateFormFactor(); // INOP


private slots:
    HRESULT addSession(IAudioSessionControl*);
    void removeSessionGroup(AudioSessionGroup*);


};

#endif // OUTPUTDEVICE_H
