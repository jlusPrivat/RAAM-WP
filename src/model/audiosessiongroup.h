#ifndef AUDIOSESSIONGROUP_H
#define AUDIOSESSIONGROUP_H

#include <audiopolicy.h>
#include <QObject>
#include <QList>
#include <QUuid>
#include "audiosession.h"
#include "utils/SafeRelease.h"



class AudioSessionGroup: public QObject {
    Q_OBJECT


public:
    // methods
    AudioSessionGroup(IAudioSessionControl*, QObject* = nullptr);
    /// returns even then false, when both guids are GUID_NULL
    bool compareGroupingParam(GUID);
    QString getGroupingParam();
    HRESULT addSession(IAudioSessionControl*);
    HRESULT addSession(AudioSession*);
    void removeSession(AudioSession*);

    void setVolume(float);
    float getVolume();
    void setMute(bool);
    bool getMute();
    QString getDisplayName();
    QString getIconPath();
    bool isSystemSoundSession();


signals:
    void sigSessionChangedGroupingParam(AudioSessionGroup*, AudioSession*);
    void sigVolumeOrMuteChanged(AudioSessionGroup*);
    void sigLastSessionClosed(AudioSessionGroup*);
    void sigDisplayNameChanged(AudioSessionGroup*);
    void sigIconPathChanged(AudioSessionGroup*);
    void sigErrored(HRESULT);


private:
    // properties
    HRESULT hr = S_OK;
    GUID groupingParam = GUID_NULL;
    QList<AudioSession*> audioSessions;

    // methods
    void connectAllSignals(AudioSession*);


};



#endif // AUDIOSESSION_H
