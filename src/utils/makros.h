#ifndef MAKROS_H
#define MAKROS_H

#define VERSION "1.0.0 NONFINAL"

#define FAILCATCH if (FAILED(hr)) goto done

#define FAILCATCH_DELETE(pointer) if (FAILED(hr)) {\
        delete(pointer);\
        goto done;\
    }

#endif // MAKROS_H
