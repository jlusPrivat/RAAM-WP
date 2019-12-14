#include "model/session.h"


Session::~Session () {
    SafeRelease(&pVolumeHandle);
}
