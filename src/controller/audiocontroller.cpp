#include "audiocontroller.h"



AudioController::AudioController (QObject *parent)
    : QObject(parent) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    init();
}



AudioController::~AudioController () {
    if (systemNotifier)
        pDeviceEnumerator->UnregisterEndpointNotificationCallback(systemNotifier);
    SafeRelease(&pDeviceEnumerator);
    CoUninitialize();
}



HRESULT AudioController::getInternalStatus () {
    return hr;
}



void AudioController::clientCommanded (Client *client, Command &command)  {
    // enumerate all output devices
    if (command.getAction() == "enumDevices") {
        if (command.containsKey("di")) {
            Command c("dev", Command::E_OUTBOUND);

            OutputDevice *device = nullptr;
            if (findOutputDevice(command.value("di"), &device))
                // send all information regarding only a single device
                c << "dc" << "ddl" << "dds" << "dff" << "m" << "l";
            else
                // send, that device was not found
                c << "dc";

            fillCommand(c, device);
            c.put("di", command.value("di"));
            client->sendCommand(c);
        }
        else {
            // send all information regarding all devices
            int count = outputDevices.count();
            for (int i = 0; i < count; i++) {
                OutputDevice *dev = outputDevices.at(i);
                Command c("dev", Command::E_OUTBOUND);
                c << "di" << "dc" << "ddl" << "dds" << "dff" << "m" << "l";
                fillCommand(c, dev);
                client->sendCommand(c);
            }
        }
    }

    // update device volume or mute
    else if (command.getAction() == "dev") {
        OutputDevice *device = nullptr;
        if (findOutputDevice(command.value("di"), &device)) {
            if (command.containsKey("m"))
                device->setMute(command.value("m")=="t");
            if (command.containsKey("l")) {
                float val = command.value("l").toFloat() / 100;
                if (val >= 0 && val <= 1)
                    device->setVolume(val);
            }
        }
        else {
            Command c("dev", Command::E_OUTBOUND);
            c.put("di", command.value("di"));
            c.put("dc", "0");
            client->sendCommand(c);
        }
    }

    // perform any session operation
    else if (command.getAction() == "enumSessions" || command.getAction() == "sess") {
        OutputDevice *device = nullptr;
        if (findOutputDevice(command.value("di"), &device)) {

            // simply enumerate all sessions with all parameters
            if (command.getAction() == "enumSessions") {
                int count = device->audioSessionGroups.count();
                for (int i = 0; i < count; i++) {
                    // fill the command
                    Command c("sess", Command::E_OUTBOUND);
                    c << "di" << "si" << "sc" << "sdn" << "m" << "l";
                    fillCommand(c, device);
                    fillCommand(c, device->audioSessionGroups.at(i));
                    client->sendCommand(c);
                }
            }

            // change the session volume or something like that
            else if (command.getAction() == "sess") {
                // find the session
                AudioSessionGroup *sess = nullptr;
                if (device->findAudioSessionGroup(command.value("si"), &sess)) {
                    if (command.containsKey("m"))
                        sess->setMute(command.value("m")=="t");
                    if (command.containsKey("l")) {
                        float val = command.value("l").toFloat() / 100;
                        if (val >= 0 && val <= 1)
                            sess->setVolume(val);
                    }
                }
                else {
                    // session not found
                    Command c("sess", Command::E_OUTBOUND);
                    c.put("di", command.value("di"));
                    c.put("si", command.value("si"));
                    c.put("sc", "0");
                    client->sendCommand(c);
                }
            }

        }
        else {
            // send device not found or removed
            Command c("dev", Command::E_OUTBOUND);
            c.put("di", command.value("di"));
            c.put("dc", "0");
            client->sendCommand(c);
        }
    }
}



void AudioController::defaultDeviceChanged (Notifier* notifier) {
    EDataFlow flow = notifier->propEDataFlow.get();
    ERole role = notifier->propERole.get();
    LPCWSTR deviceId = notifier->propLPCWSTR.get();
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
}



void AudioController::deviceAdded (Notifier* notifier) {
    LPCWSTR deviceId = notifier->propLPCWSTR.get();
    addDevice(deviceId);
}



void AudioController::deviceRemoved (Notifier* notifier) {
    LPCWSTR deviceId = notifier->propLPCWSTR.get();
    removeDevice(deviceId);
}



void AudioController::deviceStateChanged (Notifier* notifier) {
    LPCWSTR deviceId = notifier->propLPCWSTR.get();
    DWORD newState = notifier->propDWORD.get();

    // only update, if new state is active or unplugged
    if (newState & (DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED)) {
        OutputDevice *device = nullptr;
        // update available device
        if (findOutputDevice(deviceId, &device))
            device->setState(newState);
        // create new device
        else
            addDevice(deviceId);
    }
    // otherwise delete OutputDevice, because it is not present or disabled
    else
        removeDevice(deviceId);
}



void AudioController::propertyValueChanged (Notifier* notifier) {
    LPCWSTR deviceId = notifier->propLPCWSTR.get();
    const PROPERTYKEY propKey = notifier->propPROPERTYKEY.get();

    OutputDevice *device = nullptr;
    // update found device
    if (findOutputDevice(deviceId, &device)) {
        device->updateProperty(propKey);
    }
    // device not found, try recovering from invalid state by adding it
    else
        addDevice(deviceId);
}



void AudioController::connectDeviceSignals (OutputDevice *device) {
    connect(device, &OutputDevice::sigEndpointIdChanged, this, [=]{
        Command c("enumDevices", Command::E_OUTBOUND);
        broadcastCommand(c);
    });
    connect(device, &OutputDevice::sigStatusChanged, this, [=]{
        // ask for full reload, because a previously active could be unplugged
        Command c("enumDevices", Command::E_OUTBOUND);
        broadcastCommand(c);
    });
    connect(device, &OutputDevice::sigDescriptionLongChanged, this, [=]{
        Command c("dev", Command::E_OUTBOUND);
        fillCommand(c << "di" << "dc" << "ddl", device);
        broadcastCommand(c);
    });
    connect(device, &OutputDevice::sigDescriptionShortChanged, this, [=]{
        Command c("dev", Command::E_OUTBOUND);
        fillCommand(c << "di" << "dc" << "dds", device);
        broadcastCommand(c);
    });
    connect(device, &OutputDevice::sigIsDefaultOutputChanged, this, [=]{
        Command c("dev", Command::E_OUTBOUND);
        fillCommand(c << "di" << "dc", device);
        broadcastCommand(c);
    });
    connect(device, &OutputDevice::sigFormFactorChanged, this, [=]{
        Command c("dev", Command::E_OUTBOUND);
        fillCommand(c << "di" << "dc" << "dff", device);
        broadcastCommand(c);
    });
    connect(device, &OutputDevice::sigVolumeOrMuteChanged, this, [=]{
        Command c("dev", Command::E_OUTBOUND);
        fillCommand(c << "di" << "dc" << "l" << "m", device);
        broadcastCommand(c);
    });
}



HRESULT AudioController::init () {
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
        connectDeviceSignals(outputDevice);
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
    if (!systemNotifier) {
        systemNotifier = new Notifier(this);
        connect(systemNotifier, &Notifier::sigDefaultDeviceChanged,
                this, &AudioController::defaultDeviceChanged,
                Qt::BlockingQueuedConnection);
        connect(systemNotifier, &Notifier::sigDeviceAdded,
                this, &AudioController::deviceAdded,
                Qt::BlockingQueuedConnection);
        connect(systemNotifier, &Notifier::sigDeviceRemoved,
                this, &AudioController::deviceRemoved,
                Qt::BlockingQueuedConnection);
        connect(systemNotifier, &Notifier::sigDeviceStateChanged,
                this, &AudioController::deviceStateChanged,
                Qt::BlockingQueuedConnection);
        connect(systemNotifier, &Notifier::sigPropertyValueChanged,
                this, &AudioController::propertyValueChanged,
                Qt::BlockingQueuedConnection);
        pDeviceEnumerator->RegisterEndpointNotificationCallback(systemNotifier);
    }

done:
    SafeRelease(&pDefaultDevice);
    SafeRelease(&pDeviceCollection);
    if (pDefaultDeviceId)
        CoTaskMemFree(pDefaultDeviceId);
    return hr;
}



bool AudioController::findOutputDevice (LPCWSTR endpointId,
                                        OutputDevice **ppOutputDevice) {
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



bool AudioController::findOutputDevice (QString di, OutputDevice **ppOD) {
    // get the LPCWSTR of the device id
    wchar_t pDi[di.size() + 1];
    di.toWCharArray(pDi);
    pDi[di.size()] = '\0';
    return findOutputDevice(pDi, ppOD);
}



void AudioController::addDevice (LPCWSTR deviceId) {
    IMMDevice *pNewMMDevice = nullptr;
    OutputDevice *pNewOutputDevice = nullptr;
    Command c("dev", Command::E_OUTBOUND);

    // find new device, create object
    hr = pDeviceEnumerator->GetDevice(deviceId, &pNewMMDevice);
    FAILCATCH;
    pNewOutputDevice = new OutputDevice(pNewMMDevice, this);
    connectDeviceSignals(pNewOutputDevice);

    // check object integrity and insert into list
    hr = pNewOutputDevice->getInternalStatus();
    FAILCATCH_DELETE(pNewOutputDevice);
    outputDevices.append(pNewOutputDevice);

    // inform clients
    c << "dc" << "ddl" << "dds" << "dff" << "m" << "l";
    fillCommand(c, pNewOutputDevice);
    broadcastCommand(c);

done:
    SafeRelease(&pNewMMDevice);
}



void AudioController::removeDevice (LPCWSTR deviceId) {
    OutputDevice *pOutputDevice = nullptr;
    if (findOutputDevice(deviceId, &pOutputDevice)) {
        delete(pOutputDevice);
        outputDevices.removeAll(pOutputDevice);
        Command c("dev", Command::E_OUTBOUND);
        c.put("di", QString::fromWCharArray(deviceId));
        c.put("dc", "0");
        broadcastCommand(c);
    }
}



bool AudioController::findDefaultOutputDevice (OutputDevice **ppOutputDevice) {
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



void AudioController::fillCommand (Command &command, OutputDevice *device) {
    if (!device) {
        if (command.containsKey("dc"))
            command.put("dc", "0");
        return;
    }

    if (command.containsKey("di"))
        command.put("di", QString::fromWCharArray(device->pEndpointId));
    if (command.containsKey("dc")) {
        QString str = "o";
        if (device->getIsDefaultOutput())
            str.append("d");
        if (device->getState() == DEVICE_STATE_UNPLUGGED)
            str.append("u");
        else if (device->getState() == DEVICE_STATE_ACTIVE)
            str.append("a");
        command.put("dc", str);
    }
    if (command.containsKey("ddl"))
        command.put("ddl", device->getDescriptionLong());
    if (command.containsKey("dds"))
        command.put("dds", device->getDescriptionShort());
    if (command.containsKey("dff"))
        command.put("dff", formFactorToStr(device->getFormFactor()));
    if (command.containsKey("m"))
        command.put("m", device->getMute()?"t":"f");
    if (command.containsKey("l"))
        command.put("l", QString::number(static_cast<int>(device->getVolume()*100)));
}



void AudioController::fillCommand (Command &command, AudioSessionGroup *session) {
    if (!session) {
        if (command.containsKey("sc"))
            command.put("sc", "0");
        return;
    }

    if (command.containsKey("si"))
        command.put("si", session->getGroupingParam());
    if (command.containsKey("sc"))
        command.put("sc", session->isSystemSoundSession()?"s":"");
    if (command.containsKey("sdn"))
        command.put("sdn", session->getDisplayName());
    if (command.containsKey("m"))
        command.put("m", session->getMute()?"t":"f");
    if (command.containsKey("l"))
        command.put("l", QString::number(static_cast<int>(session->getVolume()*100)));
}



QString AudioController::formFactorToStr (EndpointFormFactor ff) {
    switch (ff) {
    case RemoteNetworkDevice:
        return "RemoteNetworkDevice";
    case Speakers:
        return "Speakers";
    case LineLevel:
        return "LineLevel";
    case Headphones:
        return "Headphones";
    case Microphone:
        return "Microphone";
    case Headset:
        return "Headset";
    case Handset:
        return "Handset";
    case UnknownDigitalPassthrough:
        return "UnknownDigitalPassthrough";
    case SPDIF:
        return "SPDIF";
    case DigitalAudioDisplayDevice:
        return "DigitalAudioDisplayDevice";
    default:
        return "UnknownFormFactor";
    }
}
