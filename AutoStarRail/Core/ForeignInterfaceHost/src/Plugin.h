#ifndef ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H
#define ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Core/Exceptions/InterfaceNotFoundException.h>
#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/IAsrBase.h>
#include "ForeignInterfaceHostEnum.h"
#include "IForeignLanguageRuntime.h"
#include <stdexcept>
#include <memory>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN
/**
 * @brief
 * 这个wrapper会获取IAsrSwigBase*的所有权，如果接口不能转换为interface_iid对应的接口，则会抛出异常。
 *
 * @tparam T
 * @tparam InterfaceQuerier
 */
template <class T, AsrGuid interface_iid, class InterfaceQuerier>
class SwigInterfaceWrapper : public IAsrBase
{
    T*                                           p_impl_;
    ASR::Utils::RefCounter<SwigInterfaceWrapper> ref_counter_;

    static InterfaceQuerier interface_querier_;

public:
    explicit SwigInterfaceWrapper(IAsrSwigBase* p_impl_base)
    {
        if (const auto is_iid_available =
                p_impl_base->IsCastAvailable(interface_iid);
            is_iid_available == ASR_S_OK)
        {
            throw ASR::Core::Exceptions::InterfaceNotFoundException{
                interface_iid};
        }
        p_impl_ = static_cast<T*>(p_impl_base);
    }

    int64_t AddRef() override { return ref_counter_.AddRef(); }

    int64_t Release() override { return ref_counter_.Release(this); }

    AsrResult QueryInterface(const AsrGuid& iid, void** ppv) override
    {
        return interface_querier_(iid, ppv);
    }
};

/**
 * @brief 这里不使用宏ASR_DEFINE_VARIABLE(...)进行定义，因为可能会搞晕LSP
 *
 * @tparam T
 * @tparam interface_iid
 * @tparam InterfaceQuerier
 */
template <class T, AsrGuid interface_iid, class InterfaceQuerier>
InterfaceQuerier SwigInterfaceWrapper<T, interface_iid, InterfaceQuerier>::
    interface_querier_{};

ASR_NS_ANONYMOUS_DETAILS_END

struct PluginDesc;

class Plugin
{
    ASR::AsrPtr<IAsrPlugin>                  p_impl_;
    std::unique_ptr<IForeignLanguageRuntime> up_runtime_;
    std::unique_ptr<PluginDesc>              up_plugin_desc_;
    ForeignInterfaceLanguage                 language_;

public:
    explicit Plugin(
        IAsrPlugin*                              p_impl,
        std::unique_ptr<IForeignLanguageRuntime> up_runtime,
        std::unique_ptr<PluginDesc>              up_plugin_desc,
        ForeignInterfaceLanguage                 language);
    /**
     * @brief Construct a new Plugin object
     *
     * @tparam T Expected interface type
     * @tparam interface_iid Expected interface iid
     * @tparam InterfaceQuerier The C++ implementation of the interface querier
     * @param swig_impl
     */
    template <class T, AsrGuid interface_iid, class InterfaceQuerier>
    explicit Plugin(
        IAsrPlugin*                              p_swig_impl,
        std::unique_ptr<IForeignLanguageRuntime> up_runtime,
        std::unique_ptr<PluginDesc>              up_plugin_desc,
        ForeignInterfaceLanguage                 language)
        : p_impl_{std::make_unique<Details::SwigInterfaceWrapper<
            T,
            interface_iid,
            InterfaceQuerier>>(p_swig_impl)
                      .release()},
          up_runtime_{std::move(up_runtime)},
          up_plugin_desc_{std::move(up_plugin_desc)}, language_{language}
    {
    }
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H
