#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/Utils/Utils.hpp>
#include <array>
#include <memory>
#include <unicode/unistr.h>
#include <nlohmann/json_fwd.hpp>

ASR_NS_BEGIN

namespace Details
{
    template <class T>
    class DynamicBuffer
    {
        std::size_t          size_{0};
        std::unique_ptr<T[]> up_data_{nullptr};

    public:
        DynamicBuffer() = default;
        DynamicBuffer(DynamicBuffer&& other) noexcept
            : size_{other.size_}, up_data_{std::move(other.up_data_)}
        {
        }

        static DynamicBuffer Attach(T* p_data, std::size_t size_used)
        {

            auto result = DynamicBuffer();
            result.up_data_ = std::unique_ptr<T[]>(p_data);
            result.size_ = size_used;
        }

        T* DiscardAndGetNullTerminateBufferPointer(std::size_t new_size)
        {
            if (new_size > size_)
            {
                up_data_ = std::make_unique<T[]>(new_size + 1);
                up_data_[new_size] = 0;
                size_ = new_size;
            }
            return up_data_.get();
        }

        T* begin() noexcept { return up_data_.get(); }

        T* end() noexcept { return up_data_.get() + size_; }

        T* cbegin() const noexcept { return up_data_.get(); }

        T* cend() const noexcept { return up_data_.get() + size_; }
    };
}

ASR_NS_END

template <class T>
struct AsrStringCpp
{
};

void from_json(const nlohmann::json& from, AsrString& to);

/**
 * @brief 内部使用icu存储来自各个语言字符串的实现接口
 * ! 除了GetImpl()函数和后续声明的方法外，C++侧不应该调用任何其它方法
 *
 */
class AsrStringCppImpl final : public IAsrString
{
public:
    using ICUString = U_NAMESPACE_QUALIFIER UnicodeString;

    enum class Encode
    {
        U8 = 0,
        // U16 = 1,
        U32 = 2
    };

private:
    ICUString                             impl_{};
    ASR::Details::DynamicBuffer<char16_t> shadow_impl_{};
    /**
     * @brief Notice: Boost regex assume std::string contains
     * utf-8 encoding string.
     * @see
     * https://www.boost.org/doc/libs/1_82_0/libs/regex/doc/html/boost_regex/ref/non_std_strings/icu/unicode_algo.html
     */
    std::string                              cached_utf8_string_{};
    ASR::Details::DynamicBuffer<UChar32>     cached_utf32_string_{};
    ASR::Utils::RefCounter<AsrStringCppImpl> ref_counter_{};

    std::array<bool, 3> is_cache_expired_{false, false, false};

    template <Encode E>
    bool IsCacheExpired() const noexcept
    {
        return is_cache_expired_[static_cast<std::size_t>(E)];
    }

    template <Encode E>
    void ValidateCache()
    {
        is_cache_expired_[static_cast<std::size_t>(E)] = true;
    }

    void InvalidateCache();
    void UpdateUtf32Cache();

public:
    AsrStringCppImpl();
    explicit AsrStringCppImpl(const U_NAMESPACE_QUALIFIER UnicodeString& impl);
    explicit AsrStringCppImpl(U_NAMESPACE_QUALIFIER UnicodeString&& impl);
    ~AsrStringCppImpl();
    // * IAsrBase
    int64_t   AddRef() override;
    int64_t   Release() override;
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_object) override;
    // * IAsrReadOnlyString
    const UChar32* CBegin() override;
    const UChar32* CEnd() override;
    // * IAsrString
    AsrResult SetUtf8(const char* p_string) override;
    AsrResult GetUtf8(const char** out_string) override;
    AsrResult SetUtf16(const char16_t* p_string, size_t length) override;
    AsrResult GetUtf16(
        const char16_t** out_string,
        size_t*          out_string_size) noexcept override;
    /**
     * @brief 接受一串外部为wchar_t的UTF-16编码的字符串
     *
     * @param p_string
     * @return ASR_METHOD
     */
    AsrResult SetSwigW(const wchar_t* p_string) override;
    AsrResult SetW(const wchar_t* p_string, size_t length) override;
    /**
     * @brief 在Windows下返回UTF-16 ，在Linux下返回UTF-32的字符串
     * * C# is using this function.
     *
     * @param p_string
     * @return ASR_METHOD
     */
    AsrResult GetW(const wchar_t** out_wstring) override;
    // * AsrStringCppImpl
    AsrResult GetImpl(ICUString** out_icu_string) noexcept;
    AsrResult GetImpl(const ICUString** out_icu_string) const noexcept;
};
