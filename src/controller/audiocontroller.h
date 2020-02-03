#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <mmdeviceapi.h>
#include <QList>
#include "notifier.h"
#include "utils/SafeRelease.h"
#include "utils/makros.h"
#include "model/outputdevice.h"
#include "model/client.h"
#include "model/command.h"



/**
 * \brief The AudioManager class
 *
 * Manages and creates all OutputDevices, AudioSessionGroups and AudioSessions.
 * Notifies about all changes in the audio system.
 */
class AudioController: public QObject {
    Q_OBJECT


public:
    AudioController(QObject* = nullptr);
    ~AudioController();
    HRESULT getInternalStatus();
    void clientCommanded(Client*, Command&);


signals:
    void broadcastCommand(Command&);


public slots:
    void defaultDeviceChanged(Notifier*);
    void deviceAdded(Notifier*);
    void deviceRemoved(Notifier*);
    void deviceStateChanged(Notifier*);
    void propertyValueChanged(Notifier*);


private:
    bool disableSignalling = false;
    HRESULT hr = S_OK;

    // pointers
    IMMDeviceEnumerator *pDeviceEnumerator = nullptr;
    Notifier *systemNotifier = nullptr;

    // properties
    QList<OutputDevice*> outputDevices;

    // methods
    HRESULT init();
    void connectDeviceSignals(OutputDevice*);
    bool findOutputDevice(LPCWSTR, OutputDevice**);
    bool findOutputDevice(QString, OutputDevice**);
    bool findDefaultOutputDevice(OutputDevice**);
    void addDevice(LPCWSTR);
    void removeDevice(LPCWSTR);
    /// Fills the preset attributes for a controller to the desired values
    void fillCommand(Command&, OutputDevice*);
    static QString formFactorToStr(EndpointFormFactor);


};

#endif // AUDIOMANAGER_H
