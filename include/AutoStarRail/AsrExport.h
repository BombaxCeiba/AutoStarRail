#ifndef ASR_EXPORT_H
#define ASR_EXPORT_H

#if (defined _WIN32 || defined __CYGWIN__)
#define ASR_DLL_IMPORT __declspec(dllimport)
#define ASR_DLL_EXPORT __declspec(dllexport)
#define ASR_DLL_LOCAL
#elif __GNUC__ >= 4
#define ASR_DLL_IMPORT __attribute__((visibility("default")))
#define ASR_DLL_EXPORT __attribute__((visibility("default")))
#define ASR_DLL_LOCAL __attribute__((visibility("hidden")))
#else
#define ASR_DLL_IMPORT
#define ASR_DLL_EXPORT
#define ASR_DLL_LOCAL
#endif

#ifdef ASR_BUILD_SHARED
#define ASR_EXPORT ASR_DLL_EXPORT
#else
#define ASR_EXPORT ASR_DLL_IMPORT
#endif

#ifdef __cplusplus
#define ASR_EXTERN_C extern "C"
#else
#define ASR_EXTERN_C
#endif

#define ASR_C_API ASR_EXTERN_C ASR_EXPORT
#define ASR_API ASR_DLL_EXPORT

#endif // ASR_EXPORT_H
