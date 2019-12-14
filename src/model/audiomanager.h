#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <QList>
#include "model/session.h"
#include "utils/SafeRelease.h"
#include "utils/makros.h"


class AudioManager {

public:
    AudioManager ();
    ~AudioManager ();
    HRESULT getStatus ();
    HRESULT addSession (IAudioSessionControl*);


private:
    HRESULT status = S_OK;
    QList<Session*> sessions;

    HRESULT init();

};

#endif // AUDIOMANAGER_H
