#include "AutoStarRail/AsrPtr.hpp"
#include "AutoStarRail/IAsrBase.h"
#include "AutoStarRail/PluginInterface/IAsrPlugin.h"
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

AsrResult SwigToCpp<IAsrSwigErrorLens>::TranslateError(
    IAsrReadOnlyString*  local_name,
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_string)
{
    AsrString asr_str_local_name{local_name};
    auto swig_result = sp_impl_->TranslateError(asr_str_local_name, error_code);
    auto result = swig_result.error_code;
    if (ASR::IsOk(result))
    {
        swig_result.value.GetImpl(pp_out_string);
    }
    return result;
}

// AsrResult SwigToCpp<IAsrSwigPlugin>::EnumFeature(
//     const size_t      index,
//     AsrPluginFeature* p_out_feature)
// {
//     auto swig_result = sp_impl_->EnumFeature(index);
//     auto result = swig_result.error_code;
//     *p_out_feature = swig_result.value;
//     return result;
// }

// AsrResult SwigToCpp<IAsrSwigPlugin>::GetFeatureInterface(
//     AsrPluginFeature feature,
//     IAsrBase**       pp_out_object)
// {
//     auto swig_result = sp_impl_->GetFeatureInterface(feature);
//     auto result = swig_result.error_code;
//     if (ASR::IsOk(result))
//     {
//         auto interface = swig_result.value;
//         switch (feature)
//         {
//             using enum AsrPluginFeature;
//         case Capture:

//         case ErrorLens:
//             if (interface->IsCastAvailable(AsrIidOf<IAsrSwigErrorLens>()))
//             {
//                 *pp_out_object = new SwigToCpp<IAsrSwigErrorLens>(
//                     std::static_pointer_cast<IAsrSwigErrorLens>(interface));
//             }
//             break;
//         case Task:
//             if (interface->IsCastAvailable(AsrIidOf<IAsrSwigTask>()))
//             {
//                 *pp_out_object = new SwigToCpp<IAsrSwigErrorLens>(
//                     std::static_pointer_cast<IAsrSwigErrorLens>(interface));
//             }
//             break;
//         default:
//             return ASR_E_NO_INTERFACE;
//         }
//     }
//     return result;
// }

// ---------------------------------------------------------------------------

// AsrResult CppToSwig<IAsrPlugin>::IsCastAvailable(const AsrGuid& iid)
// {
//     return IsCastAvailableImpl(
//         iid,
//         swig_type_inherit_chain_end_of_t<
//             IAsrSwigPlugin,
//             asr_plugin_inherit_chain>{});
// }

// AsrRetPluginFeature CppToSwig<IAsrPlugin>::EnumFeature(const size_t index)
// {
//     AsrRetPluginFeature result;
//     AsrPluginFeature    feature;
//     result.error_code = p_impl_->EnumFeature(index, &feature);
//     if (ASR::IsOk(result.error_code))
//     {
//         result.value = feature;
//     }
//     return result;
// }

// AsrRetSwigBase CppToSwig<IAsrPlugin>::GetFeatureInterface(
//     AsrPluginFeature feature)
// {
//     AsrRetSwigBase        result{};
//     ASR::AsrPtr<IAsrBase> p_result;
//     result.error_code = p_impl_->GetFeatureInterface(feature, p_result.Put());
//     if (ASR::IsOk(result.error_code))
//     {
//         switch (feature)
//         {
//             using enum AsrPluginFeature;

//         case Capture:
//         case ErrorLens:
//         case Task:
//             break;
//         default:
//             result.error_code = ASR_E_NO_INTERFACE;
//         }
//     }
//     return result;
// }

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
