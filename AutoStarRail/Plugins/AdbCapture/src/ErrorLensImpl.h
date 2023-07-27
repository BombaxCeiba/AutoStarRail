#ifndef ASR_PLUGINS_ERRORLENSIMPL_H
#define ASR_PLUGINS_ERRORLENSIMPL_H

#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/Utils/Expected.h>
#include <unordered_map>
#include <string>

struct AsrReadOnlyStringHash
{
    std::size_t operator()(
        const ASR::AsrPtr<IAsrReadOnlyString>& s) const noexcept;
};

ASR_NS_BEGIN

class AdbCaptureErrorLens final : public IAsrErrorLens
{
    using ErrorCodeMap =
        std::unordered_map<AsrResult, AsrPtr<IAsrReadOnlyString>>;
    using LocaleErrorCodeMap = std::unordered_map<
        AsrPtr<IAsrReadOnlyString>,
        ErrorCodeMap,
        AsrReadOnlyStringHash>;
    LocaleErrorCodeMap                          map_;
    ASR::Utils::RefCounter<AdbCaptureErrorLens> ref_counter_;

    static AsrPtr<IAsrReadOnlyString> p_default_locale_name;
    static std::string (*error_code_not_found_explanation_generator)(
        AsrResult,
        IAsrReadOnlyString**);

public:
    // IAsrBase
    int64_t   AddRef() override;
    int64_t   Release() override;
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    // IAsrErrorLens
    AsrResult TranslateError(
        IAsrReadOnlyString*  locale_name,
        AsrResult            error_code,
        IAsrReadOnlyString** out_string) override;
    AsrResult RegisterErrorCode(
        const AsrResult            error_code,
        AsrPtr<IAsrReadOnlyString> locale_name,
        AsrPtr<IAsrReadOnlyString> p_explanation);
};

ASR_NS_END

#endif // ASR_PLUGINS_ERRORLENSIMPL_H