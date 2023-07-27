#ifndef ASR_CORE_FOREIGNINTERFACEHOST_QUERYINTERFACEIMPL_HPP
#define ASR_CORE_FOREIGNINTERFACEHOST_QUERYINTERFACEIMPL_HPP

#include "AutoStarRail/PluginInterface/IAsrCapture.h"
#include "AutoStarRail/PluginInterface/IAsrErrorLens.h"
#include "AutoStarRail/PluginInterface/IAsrTask.h"
#include "AutoStarRail/PluginInterface/IAsrPlugin.h"
#include "AutoStarRail/AsrString.hpp"
#include "AutoStarRail/Utils/Config.h"
#include "IAsrBaseAdapterUtils.h"

ASR_UTILS_NS_BEGIN

template <class T>
struct QueryInterfaceImpl;

template <class T, class TImpl>
AsrResult
QueryInterface(TImpl* p_this, const AsrGuid& iid, void** pp_out_object)
{
    return QueryInterfaceImpl<T>{}(p_this, iid, pp_out_object);
}

template <class T, class TImpl>
AsrResult
InternalQueryInterface(TImpl* p_this, const AsrGuid& iid, void** pp_out_object)
{
    if (AsrIidOf<T>() == iid)
    {
        *pp_out_object = static_cast<T*>(p_this);
        p_this->AddRef();
        return ASR_S_OK;
    }
    return ASR_E_NO_INTERFACE;
}

template <>
struct QueryInterfaceImpl<IAsrBase>
{
    template <class T>
    AsrResult operator()(T* p_this, const AsrGuid& iid, void** pp_out_object)
    {
        return InternalQueryInterface<IAsrBase>(p_this, iid, pp_out_object);
    }
};

#define ASR_UTILS_DEFINE_QUERYINTERFACEIMPL(base, derived)                     \
    template <>                                                                \
    struct QueryInterfaceImpl<derived>                                         \
    {                                                                          \
        template <class T>                                                     \
        AsrResult                                                              \
        operator()(T* p_this, const AsrGuid& iid, void** pp_out_object)        \
        {                                                                      \
            return ASR::IsOk(                                                  \
                       QueryInterfaceImpl<base>{}(p_this, iid, pp_out_object)) \
                       ? ASR_S_OK                                              \
                       : InternalQueryInterface<derived>(                      \
                           p_this,                                             \
                           iid,                                                \
                           pp_out_object);                                     \
        }                                                                      \
    }

// AsrString.hpp
ASR_UTILS_DEFINE_QUERYINTERFACEIMPL(IAsrBase, IAsrReadOnlyString);
ASR_UTILS_DEFINE_QUERYINTERFACEIMPL(IAsrReadOnlyString, IAsrString);

// PluginInterface/IAsrCapture.h
ASR_UTILS_DEFINE_QUERYINTERFACEIMPL(IAsrBase, IAsrCapture);
ASR_UTILS_DEFINE_QUERYINTERFACEIMPL(IAsrBase, IAsrCaptureFactory);

// PluginInterface/IAsrPlugin.hpp
ASR_UTILS_DEFINE_QUERYINTERFACEIMPL(IAsrBase, IAsrPlugin);

// PluginInterface/IAsrErrorLens.h
ASR_UTILS_DEFINE_QUERYINTERFACEIMPL(IAsrBase, IAsrErrorLens);

// PluginInterface/IAsrTask.h
ASR_UTILS_DEFINE_QUERYINTERFACEIMPL(IAsrBase, IAsrTask);

ASR_UTILS_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_QUERYINTERFACEIMPL_HPP
