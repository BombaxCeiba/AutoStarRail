#ifndef ASR_CORE_FOREIGNINTERFACEHOST_ISCASTAVAILABLE_HPP
#define ASR_CORE_FOREIGNINTERFACEHOST_ISCASTAVAILABLE_HPP

#include "AutoStarRail/IAsrBase.h"
#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/PluginInterface/IAsrTask.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <type_traits>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

template <class... Ts>
class SwigTypeInheritChain
{
};

template <class... Ts>
AsrResult IsCastAvailableImpl(const AsrGuid& iid, SwigTypeInheritChain<Ts...>)
{
    if (((iid == AsrIidOf<Ts>()) || ...))
    {
        return ASR_S_OK;
    }
    else
    {
        return ASR_E_NO_INTERFACE;
    }
}

using asr_task_inherit_chain = SwigTypeInheritChain<IAsrSwigBase, IAsrSwigTask>;
using asr_capture_inherit_chain =
    SwigTypeInheritChain<IAsrSwigBase, IAsrSwigCapture>;
using asr_plugin_inherit_chain =
    SwigTypeInheritChain<IAsrSwigBase, IAsrSwigPlugin>;

template <class EndT, class Arg, class... ResultTs>
auto SwigTypeInheritChainEndOfImpl(
    SwigTypeInheritChain<Arg>,
    SwigTypeInheritChain<ResultTs...>) -> SwigTypeInheritChain<ResultTs..., Arg>
{
    return {};
}

template <class EndT, class Arg, class... Args, class... ResultTs>
auto SwigTypeInheritChainEndOfImpl(
    SwigTypeInheritChain<Arg, Args...>,
    SwigTypeInheritChain<ResultTs...>)
    -> std::conditional_t<
        std::is_same_v<EndT, Arg>,
        SwigTypeInheritChain<ResultTs..., Arg>,
        decltype(SwigTypeInheritChainEndOfImpl<EndT>(
            SwigTypeInheritChain<Args...>{},
            SwigTypeInheritChain<ResultTs..., Arg>{}))>
{
    return {};
}

template <class EndT, class Arg, class... Args>
auto SwigTypeInheritChainEndOfImpl(SwigTypeInheritChain<Arg, Args...>)
    -> std::conditional_t<
        std::is_same_v<EndT, Arg>,
        SwigTypeInheritChain<Arg>,
        decltype(SwigTypeInheritChainEndOfImpl<EndT>(
            SwigTypeInheritChain<Args...>{},
            SwigTypeInheritChain<Arg>{}))>
{
    return {};
}

template <class EndT, class TypeList>
using swig_type_inherit_chain_end_of_t =
    decltype(SwigTypeInheritChainEndOfImpl<EndT>(TypeList{}));

template <class T, class Input>
struct swig_type_inherit_chain_add_type;

template <class T, class... Args>
struct swig_type_inherit_chain_add_type<T, SwigTypeInheritChain<Args...>>
{
    using type = SwigTypeInheritChain<Args..., T>;
};

template <class T, class Input>
using swig_type_inherit_chain_add_type_t =
    typename swig_type_inherit_chain_add_type<T, Input>::type;

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_ISCASTAVAILABLE_HPP
