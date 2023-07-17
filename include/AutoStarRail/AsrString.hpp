#ifndef ASR_STRING_HPP
#define ASR_STRING_HPP

#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/AsrPtr.hpp>
#include <utility>
#include <string>

// {C09E276A-B824-4667-A504-7609B4B7DD28}
ASR_DEFINE_GUID(
    ASR_IID_READ_ONLY_STRING,
    IAsrReadOnlyString,
    0xc09e276a,
    0xb824,
    0x4667,
    0xa5,
    0x4,
    0x76,
    0x9,
    0xb4,
    0xb7,
    0xdd,
    0x28)
SWIG_IGNORE(IAsrReadOnlyString)
ASR_INTERFACE IAsrReadOnlyString : public IAsrBase
{
    // * Python
    ASR_METHOD GetUtf8(const char** out_string) = 0;
    // * Java
    ASR_METHOD GetUtf16(
        const char16_t** out_string,
        size_t*          out_string_size) noexcept = 0;
    // * C#
    /**
     * @brief 在Windows下返回UTF-16字符串，在Linux下返回UTF-32字符串
     *
     * @param p_string
     * @return ASR_METHOD
     */
    ASR_METHOD GetW(const wchar_t**) = 0;
    // * C++
    virtual const int32_t* CBegin() = 0;
    virtual const int32_t* CEnd() = 0;
};

// {B1F93FD0-B818-448D-A58C-493DCBDFB781}
ASR_DEFINE_GUID(
    ASR_IID_STRING,
    IAsrString,
    0xb1f93fd0,
    0xb818,
    0x448d,
    0xa5,
    0x8c,
    0x49,
    0x3d,
    0xcb,
    0xdf,
    0xb7,
    0x81);
SWIG_IGNORE(IAsrString)
ASR_INTERFACE IAsrString : public IAsrReadOnlyString
{
    // * Python
    ASR_METHOD SetUtf8(const char* p_string) = 0;
    // * Java
    ASR_METHOD SetUtf16(const char16_t* p_string, size_t length) = 0;
    // * C#
    /**
     * @brief 接受一串外部为wchar_t的UTF-16编码的字符串
     *
     * @param p_string
     * @return ASR_METHOD
     */
    ASR_METHOD SetSwigW(const wchar_t* p_string) = 0;
    // * C++
    /**
     * @brief 在Windows下接收UTF-16字符串，在Linux下接收UTF-32字符串
     *
     * @param p_string
     * @return ASR_METHOD
     */
    ASR_METHOD SetW(const wchar_t* p_string, size_t length) = 0;
};

#ifndef SWIG

ASR_NS_BEGIN

namespace Details
{
    ASR_API AsrPtr<IAsrReadOnlyString> CreateNullAsrString();
    ASR_API AsrPtr<IAsrString> CreateAsrString();
}

ASR_NS_END

#endif // SWIG

#ifndef SWIG
#define ASR_STRING_ENABLE_WHEN_CPP
#endif

/**
 * @brief
 * ! 此类通过宏定义控制对SWIG公开的函数，
 *
 */
class AsrString
{
    ASR::AsrPtr<IAsrReadOnlyString> p_impl_{
        ASR::Details::CreateNullAsrString()};

public:
    AsrString() = default;

    SWIG_IGNORE(AsrString(ASR::AsrPtr<IAsrString> p_impl))
    explicit AsrString(ASR::AsrPtr<IAsrString> p_impl) : p_impl_{p_impl} {}
    SWIG_IGNORE(AsrString(IAsrReadOnlyString* p_impl))
    explicit AsrString(IAsrReadOnlyString* p_impl) : p_impl_{p_impl} {}

    /**
     * @brief
     * 从其它语言运行时获得UTF-8字符串
        Get时也使用UTF-8字符串
        当前使用这一策略的语言：Python
     *
     */
#if defined(ASR_STRING_ENABLE_WHEN_CPP) || defined(SWIGPYTHON)
    AsrString(const char* p_utf8_string)
        : p_impl_{ASR::Details::CreateAsrString()}
    {
        static_cast<IAsrString*>(p_impl_.Get())->SetUtf8(p_utf8_string);
    }

    const char* GetUtf8() const
    {
        const char* result;
        p_impl_->GetUtf8(&result);
        return result;
    }
#endif // defined(ASR_STRING_ENABLE_WHEN_CPP) || defined(SWIGPYTHON)

    /**
     * @brief
     * 从其它语言运行时获得UTF-16的字符串，但是外层使用wchar_t包装。
        Get时在Win32环境下返回UTF-16，在Linux环境下返回UTF-32。
        当前使用这一策略的语言：C#
     *
     */
#if defined(ASR_STRING_ENABLE_WHEN_CPP) || defined(SWIGCSHARP)
    AsrString(const wchar_t* p_wstring)
        : p_impl_{ASR::Details::CreateAsrString()}
    {
        static_cast<IAsrString*>(p_impl_.Get())->SetSwigW(p_wstring);
    }

    const wchar_t* GetW() const
    {
        const wchar_t* p_wstring = nullptr;
        p_impl_->GetW(&p_wstring);
        return p_wstring;
    }

#endif // defined(ASR_STRING_ENABLE_WHEN_CPP) || defined(SWIGCSHARP)

    /**
     * @brief 由于SWIG对于Java支持可能存在缺陷，这一功能由本项目实现
     *
     */
#if defined(ASR_STRING_ENABLE_WHEN_CPP) || defined(SWIGJAVA)
    AsrString(const char16_t* p_u16string, size_t length)
        : p_impl_{ASR::Details::CreateAsrString()}
    {
        static_cast<IAsrString*>(p_impl_.Get())->SetUtf16(p_u16string, length);
    }

    void GetUtf16(const char16_t** out_string, size_t* out_string_size) const
    {
        p_impl_->GetUtf16(out_string, out_string_size);
    }
#endif

    SWIG_IGNORE(cbegin)
    const int32_t* cbegin() const { return p_impl_->CBegin(); };
    SWIG_IGNORE(cend)
    const int32_t* cend() const { return p_impl_->CEnd(); };
    SWIG_IGNORE(GetImpl)
    void GetImpl(IAsrReadOnlyString** pp_out_readonly_string) const
    {
        const auto result = p_impl_.Get();
        *pp_out_readonly_string = result;
        result->AddRef();
    };
};

#ifndef SWIG
ASR_C_API AsrResult
CreateIAsrStringFromUtf8(const char* p_utf8_string, IAsrString** pp_out_string);

/**
 * @brief same as ASR_METHOD IAsrString::SetW(const wchar_t* p_string, size_t
 * length) = 0
 *
 * @param p_wstring
 * @param size
 * @param pp_out_string
 * @return ASR_C_API
 */
ASR_C_API AsrResult CreateIAsrStringFromWChar(
    const wchar_t* p_wstring,
    size_t         size,
    IAsrString**   pp_out_string);
#endif // SWIG

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetString)
    AsrString value;
ASR_RET_TYPE_DECLARE_END

ASR_API AsrString AsrGuidToString(const AsrGuid& guid);

#endif // ASR_STRING_HPP