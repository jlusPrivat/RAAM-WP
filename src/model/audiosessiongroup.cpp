#include "audiosessiongroup.h"

AudioSessionGroup::AudioSessionGroup (IAudioSessionControl *control, QObject *parent)
    : QObject(parent) {
    pSessControl = control;
    control->AddRef();
}



AudioSessionGroup::~AudioSessionGroup () {
    SafeRelease(&pSessControl);
}
