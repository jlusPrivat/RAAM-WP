#ifndef AUDIOSESSION_H
#define AUDIOSESSION_H

#include <audiopolicy.h>
#include <QObject>
#include "controller/notifier.h"
#include "utils/SafeRelease.h"
#include "utils/makros.h"



class AudioSession: public QObject {
    Q_OBJECT


public:
    // properties
    IAudioSessionControl *control = nullptr;
    ISimpleAudioVolume *simpleVolume = nullptr;
    Notifier *systemNotifier = nullptr;

    // methods
    AudioSession(IAudioSessionControl*, QObject* = nullptr);
    ~AudioSession();
    HRESULT getInternalState();
    void setVolume(float);
    float getVolume();
    void setMute(bool);
    bool getMute();
    GUID getGroupingParam();
    QString getDisplayName();
    QString getIconPath();
    bool isSystemSoundSession();


signals:
    void sigDisplayNameChanged(AudioSession*);
    void sigGroupingParamChanged(AudioSession*);
    void sigIconPathChanged(AudioSession*);
    void sigSessionRemoved(AudioSession*);
    void sigVolumeOrMuteChanged(AudioSession*);
    void sigErrored(HRESULT);


private:
    HRESULT hr = S_OK;
    bool systemSoundSession = false;


};



#endif // AUDIOSESSION_H
