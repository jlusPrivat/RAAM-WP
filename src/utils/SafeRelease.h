#ifndef SAFERELEASE_H
#define SAFERELEASE_H

/* safely releases ressources by calling its release
 * method and setting the pointer to null */
template <typename T> void SafeRelease (T **ppT) {
    if (*ppT) {
        (*ppT)->Release();
        *ppT = nullptr;
    }
}


#endif // SAFERELEASE_H
