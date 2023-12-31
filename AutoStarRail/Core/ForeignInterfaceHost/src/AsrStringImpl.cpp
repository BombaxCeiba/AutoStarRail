#include "AutoStarRail/AsrConfig.h"
#include "AutoStarRail/Core/ForeignInterfaceHost/Config.h"
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include "AutoStarRail/Utils/QueryInterfaceImpl.hpp"
#include <AutoStarRail/Utils/Utils.hpp>
#include <cstddef>
#include <functional>
#include <unicode/unistr.h>
#include <unicode/uversion.h>
#include <vector>
#include <cwchar>
#include <algorithm>
#include <cstring>
#include <nlohmann/json.hpp>

auto(ASR_FMT_NS::formatter<IAsrReadOnlyString, char>::format)(
    IAsrReadOnlyString* p_string,
    format_context&     ctx) const ->
    typename std::remove_reference_t<decltype(ctx)>::iterator
{
    const char* p_string_data{nullptr};
    const auto  result = p_string->GetUtf8(&p_string_data);
    if (ASR::IsOk(result))
    {
        return ASR_FMT_NS::format_to(ctx.out(), "{}", p_string_data);
    }
    else
    {
        return ASR_FMT_NS::format_to(
            ctx.out(),
            "(An error occurred when getting string, with error code = {})",
            result);
    }
}

ASR_UTILS_NS_BEGIN

ASR_UTILS_DEFINE_QUERYINTERFACEIMPL(IAsrString, AsrStringCppImpl);

ASR_UTILS_NS_END

void AsrStringCppImpl::InvalidateCache()
{
    is_cache_expired_ = {false, false, false};
}

void AsrStringCppImpl::UpdateUtf32Cache()
{
    if (IsCacheExpired<Encode::U32>())
    {
        const auto u32_char_count = impl_.countChar32();
        const auto p_cached_utf32_string =
            cached_utf32_string_.DiscardAndGetNullTerminateBufferPointer(
                u32_char_count);
        UErrorCode status = U_ZERO_ERROR;
        impl_.toUTF32(p_cached_utf32_string, u32_char_count, status);
        ValidateCache<Encode::U32>();
    }
}

AsrStringCppImpl::AsrStringCppImpl() = default;

AsrStringCppImpl::AsrStringCppImpl(
    const U_NAMESPACE_QUALIFIER UnicodeString& impl)
    : impl_{impl}
{
}

AsrStringCppImpl::AsrStringCppImpl(
    U_NAMESPACE_QUALIFIER UnicodeString&& impl) noexcept
    : impl_{std::move(impl)}
{
}

AsrStringCppImpl::~AsrStringCppImpl() = default;

int64_t AsrStringCppImpl::AddRef() { return ref_counter_.AddRef(); }

int64_t AsrStringCppImpl::Release() { return ref_counter_.Release(this); }

AsrResult AsrStringCppImpl::QueryInterface(const AsrGuid& iid, void** pp_object)
{
    return ASR::Utils::QueryInterface<AsrStringCppImpl>(this, iid, pp_object);
}

const UChar32* AsrStringCppImpl::CBegin()
{
    UpdateUtf32Cache();
    return cached_utf32_string_.cbegin();
};

const UChar32* AsrStringCppImpl::CEnd()
{
    UpdateUtf32Cache();
    return cached_utf32_string_.cend();
}

AsrResult AsrStringCppImpl::SetUtf8(const char* p_string)
{
    InvalidateCache();
    impl_ = U_NAMESPACE_QUALIFIER UnicodeString::fromUTF8(p_string);

    cached_utf8_string_ = p_string;
    ValidateCache<Encode::U8>();

    return ASR_S_OK;
};

AsrResult AsrStringCppImpl::GetUtf8(const char** out_string)
{
    if (IsCacheExpired<Encode::U8>())
    {
        impl_.toUTF8String(cached_utf8_string_);
        ValidateCache<Encode::U8>();
    }
    *out_string = cached_utf8_string_.c_str();
    return ASR_S_OK;
};

AsrResult AsrStringCppImpl::SetUtf16(const char16_t* p_string, size_t length)
{
    InvalidateCache();
    const auto int_length = static_cast<int>(length);
    impl_ = {p_string, int_length};
    return ASR_S_OK;
}

AsrResult AsrStringCppImpl::GetUtf16(
    const char16_t** out_string,
    size_t*          out_string_size) noexcept
{
    *out_string = impl_.getBuffer();
    *out_string_size = impl_.length();
    return ASR_S_OK;
}

ASR_NS_ANONYMOUS_DETAILS_BEGIN

template <class T>
auto SetSwigW(const T* p_wstring, auto& shadow_impl)
    -> U_NAMESPACE_QUALIFIER UnicodeString
{
    U_NAMESPACE_QUALIFIER UnicodeString result;
    if constexpr (sizeof(wchar_t) == sizeof(char16_t))
    {
        result = {p_wstring};
    }
    else if constexpr (sizeof(wchar_t) == sizeof(char32_t))
    {
        const auto string_size = std::wcslen(p_wstring);
        const auto p_shadow_string =
            shadow_impl.DiscardAndGetNullTerminateBufferPointer(string_size);
        std::transform(
            p_wstring,
            p_wstring + string_size,
            p_shadow_string,
            [](const wchar_t c)
            {
                char16_t u16_char;
                // Can be replaced to std::bit_cast
                std::memcpy(&u16_char, &c, sizeof(u16_char));
                return u16_char;
            });
        const auto int_length = static_cast<int>(string_size);
        result = {p_shadow_string, int_length, int_length};
    }
    return result;
}

template <class T>
auto SetW(
    const T* p_wstring,
    size_t   length,
    auto&    cached_utf32_string,
    auto     validate_u32_cache) -> U_NAMESPACE_QUALIFIER UnicodeString
{
    const auto                          int_length = static_cast<int>(length);
    U_NAMESPACE_QUALIFIER UnicodeString result;

    if constexpr (sizeof(wchar_t) == sizeof(char16_t))
    {
        result = {p_wstring, int_length};
    }
    else if constexpr (sizeof(wchar_t) == sizeof(char32_t))
    {
        result = U_NAMESPACE_QUALIFIER UnicodeString::fromUTF32(
            (reinterpret_cast<const UChar32*>(p_wstring)),
            int_length);

        const auto p_cached_u32string =
            cached_utf32_string.DiscardAndGetNullTerminateBufferPointer(length);
        std::memcpy(p_cached_u32string, p_wstring, length * sizeof(UChar32));
        validate_u32_cache();
    }

    return result;
}

ASR_NS_ANONYMOUS_DETAILS_END

AsrResult AsrStringCppImpl::SetSwigW(const wchar_t* p_string)
{
    InvalidateCache();

    impl_ = Details::SetSwigW(p_string, shadow_impl_);

    return ASR_S_OK;
}

AsrResult AsrStringCppImpl::SetW(const wchar_t* p_string, size_t length)
{
    InvalidateCache();

    impl_ = Details::SetW(
        p_string,
        length,
        cached_utf32_string_,
        [this]() { ValidateCache<Encode::U32>(); });

    return ASR_S_OK;
}

AsrResult AsrStringCppImpl::GetW(const wchar_t** out_wstring)
{
    if constexpr (sizeof(wchar_t) == sizeof(char16_t))
    {
        *out_wstring =
            reinterpret_cast<const wchar_t*>(impl_.getTerminatedBuffer());
        return ASR_S_OK;
    }
    else if constexpr (sizeof(wchar_t) == sizeof(char32_t))
    {
        UpdateUtf32Cache();
        *out_wstring =
            reinterpret_cast<const wchar_t*>(cached_utf32_string_.begin());
        return ASR_S_OK;
    }
}

/**
 * @brief C++侧使用此接口读取字符串，其它接口均供外部语言调用
 *
 * @return AsrResult
 */
AsrResult AsrStringCppImpl::GetImpl(ICUString** out_icu_string) noexcept
{
    InvalidateCache();
    *out_icu_string = &impl_;
    return ASR_S_OK;
};

AsrResult AsrStringCppImpl::GetImpl(
    const ICUString** out_icu_string) const noexcept
{
    *out_icu_string = &impl_;
    return ASR_S_OK;
};

void from_json(const nlohmann::json& from, AsrString& to)
{
    to = {from.get_ref<const std::string&>().data()};
}

ASR_NS_BEGIN

namespace Details
{
    template <class T>
    using NullString = std::array<T, 2>;

    class NullStringImpl final : public IAsrReadOnlyString
    {
        static std::string          null_u8string_;
        static NullString<wchar_t>  null_wstring_;
        static NullString<char16_t> null_u16string_;
        static NullString<UChar32>  null_u32string_;

    public:
        int64_t   AddRef() override { return 1; }
        int64_t   Release() override { return 1; }
        AsrResult QueryInterface(const AsrGuid& iid, void** pp_object) override
        {
            return ASR::Utils::QueryInterface<IAsrReadOnlyString>(
                this,
                iid,
                pp_object);
        }

        AsrResult GetUtf8(const char** out_string) override
        {
            *out_string = null_u8string_.data();
            return ASR_S_OK;
        }

        AsrResult GetUtf16(
            const char16_t** out_string,
            size_t*          out_string_size) noexcept override
        {
            *out_string = null_u16string_.data();
            *out_string_size = 0;
            return ASR_S_OK;
        };

        AsrResult GetW(const wchar_t** out_wstring) override
        {
            *out_wstring = null_wstring_.data();
            return ASR_S_OK;
        }

        const UChar32* CBegin() override { return null_u32string_.data(); }
        const UChar32* CEnd() override { return null_u32string_.data(); }
    };

    ASR_DEFINE_VARIABLE(NullStringImpl::null_u8string_){};
    ASR_DEFINE_VARIABLE(NullStringImpl::null_wstring_){};
    ASR_DEFINE_VARIABLE(NullStringImpl::null_u16string_){};
    ASR_DEFINE_VARIABLE(NullStringImpl::null_u32string_){};

    NullStringImpl null_asr_string_impl_{};

    AsrPtr<IAsrReadOnlyString> CreateNullAsrString()
    {
        return AsrPtr<IAsrReadOnlyString>(&null_asr_string_impl_);
    }

    AsrPtr<IAsrString> CreateAsrString()
    {
        return AsrPtr<IAsrString>(new AsrStringCppImpl());
    }
}

ASR_NS_END

AsrResult CreateIAsrStringFromUtf8(
    const char*  p_utf8_string,
    IAsrString** pp_out_string)
{
    auto p_string = std::make_unique<AsrStringCppImpl>();
    p_string->SetUtf8(p_utf8_string);
    p_string->AddRef();
    *pp_out_string = p_string.release();
    return ASR_S_OK;
}

AsrResult CreateIAsrReadOnlyStringFromUtf8(
    const char*          p_utf8_string,
    IAsrReadOnlyString** pp_out_string)
{
    IAsrString* p_string = nullptr;
    auto        result = CreateIAsrStringFromUtf8(p_utf8_string, &p_string);
    *pp_out_string = p_string;
    return result;
}

AsrResult CreateIAsrStringFromWChar(
    const wchar_t* p_wstring,
    size_t         size,
    IAsrString**   pp_out_string)
{
    auto p_string = std::make_unique<AsrStringCppImpl>();
    p_string->SetW(p_wstring, size);
    p_string->AddRef();
    *pp_out_string = p_string.release();
    return ASR_S_OK;
}

AsrResult CreateIAsrReadOnlyStringFromWChar(
    const wchar_t*       p_wstring,
    size_t               size,
    IAsrReadOnlyString** pp_out_string)
{
    IAsrString* p_string = nullptr;
    auto        result = CreateIAsrStringFromWChar(p_wstring, size, &p_string);
    *pp_out_string = p_string;
    return result;
}