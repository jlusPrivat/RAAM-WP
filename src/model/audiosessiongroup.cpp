#include "audiosessiongroup.h"



AudioSessionGroup::AudioSessionGroup (IAudioSessionControl *control, QObject *parent)
    : QObject(parent) {
    control->GetGroupingParam(&groupingParam);
    addSession(control);
    connectAllSignals(audioSessions.first());
}



bool AudioSessionGroup::compareGroupingParam (GUID other) {
    if (IsEqualGUID(groupingParam, GUID_NULL))
        return false;
    return IsEqualGUID(groupingParam, other);
}



QString AudioSessionGroup::getGroupingParam () {
    return QUuid(groupingParam).toString();
}



HRESULT AudioSessionGroup::addSession (IAudioSessionControl *control) {
    hr = S_OK;
    AudioSession *session = new AudioSession(control, this);
    return addSession(session);
}



HRESULT AudioSessionGroup::addSession (AudioSession *session) {
    audioSessions.append(session);

    // connect some important signals for all sessions
    connect(session, &AudioSession::sigErrored,
            this, &AudioSessionGroup::sigErrored);
    connect(session, &AudioSession::sigGroupingParamChanged,
            this, [&](AudioSession *sess){
        sigSessionChangedGroupingParam(this, sess);
    });
    connect(session, &AudioSession::sigSessionRemoved,
            this, [&](AudioSession *sess){
        audioSessions.removeAll(sess);
        sess->deleteLater();
        if (audioSessions.isEmpty())
            sigLastSessionClosed(this);
    });

    return session->getInternalState();
}



void AudioSessionGroup::removeSession (AudioSession *session) {
    bool wasFirst = (audioSessions.first() == session);
    session->disconnect();
    audioSessions.removeAll(session);
    if (audioSessions.isEmpty())
        sigLastSessionClosed(this);
    else if (wasFirst)
        connectAllSignals(audioSessions.first());
}



void AudioSessionGroup::setVolume (float vol) {
    int count = audioSessions.count();
    for (int i = 0; i < count; i++) {
        audioSessions.at(i)->setVolume(vol);
    }
}



float AudioSessionGroup::getVolume () {
    if (audioSessions.isEmpty())
        return 0;
    return audioSessions.first()->getVolume();
}



void AudioSessionGroup::setMute (bool m) {
    int count = audioSessions.count();
    for (int i = 0; i < count; i++) {
        audioSessions.at(i)->setMute(m);
    }
}



bool AudioSessionGroup::getMute () {
    if (audioSessions.isEmpty())
        return true;
    return audioSessions.first()->getMute();
}



QString AudioSessionGroup::getDisplayName () {
    if (audioSessions.isEmpty())
        return "";
    return audioSessions.first()->getDisplayName();
}



QString AudioSessionGroup::getIconPath () {
    if (audioSessions.isEmpty())
        return "";
    return audioSessions.first()->getIconPath();
}



bool AudioSessionGroup::isSystemSoundSession () {
    if (audioSessions.isEmpty())
        return false;
    return audioSessions.first()->isSystemSoundSession();
}



void AudioSessionGroup::connectAllSignals (AudioSession *session) {
    connect(session, &AudioSession::sigDisplayNameChanged, this, [=]{
        sigDisplayNameChanged(this);
    });
    connect(session, &AudioSession::sigIconPathChanged, this, [=]{
        sigIconPathChanged(this);
    });
    connect(session, &AudioSession::sigVolumeOrMuteChanged, this, [=]{
        sigVolumeOrMuteChanged(this);
    });
}
