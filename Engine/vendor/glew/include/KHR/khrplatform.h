/* Minimal khrplatform.h to satisfy GLEW/EGL includes for the project build.
   This provides commonly used Khronos typedefs and API macros. */
#ifndef KHRONOS_KHRPLATFORM_H
#define KHRONOS_KHRPLATFORM_H

#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
#else
#include <stdint.h>
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef intptr_t khronos_intptr_t;
typedef uintptr_t khronos_uintptr_t;
typedef ptrdiff_t khronos_ssize_t;

typedef int64_t khronos_int64_t;
typedef uint64_t khronos_uint64_t;

/* Common time types used by some EGL headers */
typedef khronos_uint64_t khronos_utime_nanoseconds_t;
typedef khronos_int64_t khronos_stime_nanoseconds_t;

/* API decoration macros (left empty for portability) */
#ifndef KHRONOS_APICALL
#define KHRONOS_APICALL
#endif

#ifndef KHRONOS_APIENTRY
#define KHRONOS_APIENTRY
#endif

#ifndef KHRONOS_APIATTRIBUTES
#define KHRONOS_APIATTRIBUTES
#endif

#ifdef __cplusplus
}
#endif

#endif /* KHRONOS_KHRPLATFORM_H */
