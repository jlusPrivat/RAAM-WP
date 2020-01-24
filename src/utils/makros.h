#ifndef MAKROS_H
#define MAKROS_H

#define SOFTWARE_NAME "RAAM-WP"

#define VERSION "0.0.0 NONFINAL"

#define SPECS_VERSION "1.0.0"

#define VERSION_DESCRIPTION_URL \
    "https://jlus.de/projekte/downloads/raam/raam-wp_version_description"

#define FAILCATCH if (FAILED(hr)) goto done

#define FAILCATCH_DELETE(pointer) if (FAILED(hr)) {\
        delete(pointer);\
        goto done;\
    }

#endif // MAKROS_H
