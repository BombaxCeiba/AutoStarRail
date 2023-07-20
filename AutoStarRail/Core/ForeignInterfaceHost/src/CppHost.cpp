#include "CppHost.h"
#include "AutoStarRail/AsrPtr.hpp"
#include "AutoStarRail/IAsrBase.h"
#include "AutoStarRail/PluginInterface/IAsrPlugin.h"
#include <AutoStarRail/Utils/Utils.hpp>
#include "tl/expected.hpp"
#include <boost/dll/shared_library.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_CPPHOST_BEGIN

class CppRuntime final : public IForeignLanguageRuntime
{
    ASR::Utils::RefCounter<CppRuntime> ref_counter_{};
    boost::dll::shared_library                      plugin_lib_{};

public:
    int64_t   AddRef() override { return ref_counter_.AddRef(); }
    int64_t   Release() override { return ref_counter_.Release(this); }
    AsrResult QueryInterface(const AsrGuid&, void**) override
    {
        return ASR_E_NO_IMPLEMENTATION;
    }
    auto LoadPlugin(const std::filesystem::path& path)
        -> ASR::Utils::Expected<CommonPluginPtr> override
    {
        plugin_lib_.load(path.c_str());
        // Get function pointer without heap allocation.
        const auto p_init_function =
            plugin_lib_.get<::AsrCoCreatePluginFunction>(
                "AsrCoCreatePluginFunction");
        AsrPtr<IAsrPlugin> p_plugin{};
        const auto         error_code = p_init_function(p_plugin.Put());
        if (ASR::IsOk(error_code))
        {
            return CommonPluginPtr{std::move(p_plugin)};
        }
        else
        {
            return tl::make_unexpected(error_code);
        }
    }
};

auto CreateForeignLanguageRuntime(const ForeignLanguageRuntimeFactoryDescBase&)
    -> ASR::Utils::Expected<AsrPtr<IForeignLanguageRuntime>>
{
    return new CppRuntime{};
}

ASR_NS_CPPHOST_END

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
