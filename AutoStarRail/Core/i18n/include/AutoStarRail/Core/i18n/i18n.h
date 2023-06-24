#ifndef ASR_CORE_I18N_I18N_H
#define ASR_CORE_I18N_I18N_H

#include <AutoStarRail/AsrConfig.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <boost/filesystem/path.hpp>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include <map>
#include <string>

#define ASR_CORE_I18N_NS_BEGIN                                                 \
    ASR_NS_BEGIN namespace Core                                                \
    {                                                                          \
        namespace i18n                                                         \
        {

#define ASR_CORE_I18N_NS_END                                                   \
    }                                                                          \
    }                                                                          \
    ASR_NS_END

ASR_CORE_I18N_NS_BEGIN

template <class T>
using TranslateItemMap = std::map<T, std::u8string>;

template <class T>
using TranslateResources =
    std::unordered_map<std::u8string, TranslateItemMap<T>>;

template <class T>
struct TranslateJsonResource
{
    AsrType               type;
    TranslateResources<T> resources;
};

/**
 * @brief 使用前必须在i18n.h文件中显式实例化
 *
 */
template <class T>
class I18n
{
    using ConstLocaleToTranslateIt =
        typename TranslateResources<T>::const_iterator;

    TranslateResources<T>    translate_resource_;
    ConstLocaleToTranslateIt it_default_translate_map_;
    std::u8string            default_locale_;

public:
    explicit I18n(const boost::filesystem::path& json_path);
    explicit I18n(const nlohmann::json& json);
    explicit I18n(const TranslateResources<T>& translate_resource);
    explicit I18n(TranslateResources<T>&& translate_resource);
    ~I18n();

    AsrResult     SetDefaultLocale(const char8_t* const default_locale);
    std::u8string GetDefaultLocale() const;
    AsrResult     GetErrorDescription(
            const T&       result,
            std::u8string* out_error_description) const;
};

template class I18n<AsrResult>;

ASR_CORE_I18N_NS_END

#endif // ASR_CORE_I18N_I18N_H