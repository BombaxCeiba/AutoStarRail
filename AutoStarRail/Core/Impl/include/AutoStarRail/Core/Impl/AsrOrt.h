#ifndef ASR_CORE_IMPL_ASRORT_H
#define ASR_CORE_IMPL_ASRORT_H

/**
 * @brief hack onnxruntime header in order to compile on mingw.
 *
 */
#if !defined(_MSC_VER) && defined(_WIN32)
#define _In_
#define _In_z_
#define _In_opt_
#define _In_opt_z_
#define _Out_
#define _Outptr_
#define _Out_opt_
#define _Inout_
#define _Inout_opt_
#define _Frees_ptr_opt_
#define _Ret_maybenull_
#define _Ret_notnull_
#define _Check_return_
#define _Outptr_result_maybenull_
#define _In_reads_(X)
#define _Inout_updates_all_(X)
#define _Out_writes_bytes_all_(X)
#define _Out_writes_all_(X)
#define _Success_(X)
#define _Outptr_result_buffer_maybenull_(X)
#define ORT_ALL_ARGS_NONNULL __attribute__((nonnull))
#define _stdcall __attribute__((__stdcall__))
#endif

#include <onnxruntime_cxx_api.h>

#if !defined(_MSC_VER) && defined(_WIN32)
#undef _stdcall
#endif

#include <AutoStarRail/Core/Impl/Config.h>
#include <AutoStarRail/AsrString.hpp>

ASR_CORE_IMPL_NS_BEGIN

const ORTCHAR_T* ToOrtChar(AsrString string);

const ORTCHAR_T* ToOrtChar(IAsrReadOnlyString* p_string);

class AsrOrt
{
protected:
    Ort::Env env_{};
    Ort::SessionOptions Session_options_{};
    Ort::AllocatorWithDefaultOptions allocator_{};

    static Ort::MemoryInfo default_cpu_memory_info;

public:
    AsrOrt(const char* model_name);
};

ASR_CORE_IMPL_NS_END

#endif // ASR_CORE_IMPL_ASRORT_H
