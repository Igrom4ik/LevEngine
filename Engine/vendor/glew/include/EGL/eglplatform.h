/* Minimal eglplatform.h to satisfy GLEW/EGL includes for the project build.
   This defines the basic native types used by EGL in a platform-agnostic way.
   For proper EGL behavior on specific platforms, replace with platform-specific
   headers from an EGL implementation. */
#ifndef EGL_EGLPLATFORM_H
#define EGL_EGLPLATFORM_H

#include <KHR/khrplatform.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define native display/window/pixmap types as opaque pointers for portability. */
typedef void *EGLNativeDisplayType;
typedef void *EGLNativeWindowType;
typedef void *EGLNativePixmapType;

/* Types used by egl.h/eglew.h */
#ifndef EGLAPIENTRY
#define EGLAPIENTRY
#endif
#ifndef EGLAPI
#define EGLAPI extern
#endif

#ifdef __cplusplus
}
#endif

#endif /* EGL_EGLPLATFORM_H */
