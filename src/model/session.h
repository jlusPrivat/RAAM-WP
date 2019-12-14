#ifndef SESSIONS_H
#define SESSIONS_H

#include <audiopolicy.h>
#include "utils/SafeRelease.h"
#include "utils/makros.h"


class Session {

public:
    ~Session();


private:
    ISimpleAudioVolume *pVolumeHandle = nullptr;
    LPCWSTR displayName = nullptr;
    LPCGUID groupingParam = nullptr;
    bool mute;
    float volume;
    AudioSessionState state;

};

#endif // SESSIONS_H
