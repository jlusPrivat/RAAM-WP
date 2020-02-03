#ifndef AUDIOSESSION_H
#define AUDIOSESSION_H

#include <audiopolicy.h>
#include <QObject>
#include "utils/SafeRelease.h"



class AudioSessionGroup: public QObject {
    Q_OBJECT


public:
    // pointers
    IAudioSessionControl *pSessControl = nullptr;

    // methods
    AudioSessionGroup(IAudioSessionControl*, QObject* = nullptr);
    ~AudioSessionGroup();


};



#endif // AUDIOSESSION_H
