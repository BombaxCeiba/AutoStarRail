#include "AutoStarRail/PluginInterface/IAsrCapture.h"
#include "AutoStarRail/PluginInterface/IAsrErrorLens.h"
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/QueryInterfaceImpl.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <string_view>
#include <unordered_map>
#include <cstdint>
#include <optional>

#define ASR_CORE_FROEIGNINTERFACEHOST_IASRBASE_AUTO_IMPL(class_name)           \
private:                                                                       \
    ASR::Utils::RefCounter<class_name> ref_counter_;                           \
                                                                               \
public:                                                                        \
    int64_t AddRef() override { return ref_counter_.AddRef(); }                \
    int64_t Release() override { return ref_counter_.Release(this); }

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

template <class T>
concept is_asr_swig_interface = std::is_base_of_v<IAsrSwigBase, T>;

template <class T>
concept is_asr_interface = std::is_base_of_v<IAsrBase, T>;

template <class T>
class SwigToCpp;

template <is_asr_swig_interface SwigT, is_asr_interface T>
class SwigToCppBase : public T
{
protected:
    std::shared_ptr<SwigT> sp_impl_;

public:
    template <class Other>
    explicit SwigToCppBase(std::shared_ptr<Other> sp_impl) : sp_impl_{sp_impl}
    {
    }
    template <class Other>
    explicit SwigToCppBase(SwigToCpp<Other> other) : sp_impl_{other.sp_impl_}
    {
    }

    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override
    {
        return ASR::Core::ForeignInterfaceHost::QueryInterface<T>(
            this,
            iid,
            pp_out_object);
    }

    std::shared_ptr<T> GetImpl() const { return sp_impl_; }
};

template <>
class SwigToCpp<IAsrSwigBase> final
    : public SwigToCppBase<IAsrSwigBase, IAsrBase>
{
public:
    ASR_USING_BASE_CTOR(SwigToCppBase);
    ~SwigToCpp() = default;
    ASR_CORE_FROEIGNINTERFACEHOST_IASRBASE_AUTO_IMPL(SwigToCpp);
};

template <>
class SwigToCpp<IAsrSwigErrorLens> final
    : public SwigToCppBase<IAsrSwigErrorLens, IAsrErrorLens>
{
public:
    ASR_USING_BASE_CTOR(SwigToCppBase);
    ~SwigToCpp() = default;
    ASR_CORE_FROEIGNINTERFACEHOST_IASRBASE_AUTO_IMPL(SwigToCpp);

    AsrResult TranslateError(
        IAsrReadOnlyString*  local_name,
        AsrResult            error_code,
        IAsrReadOnlyString** out_string) override
    {
        AsrString asr_str_local_name{local_name};
        auto      swig_result =
            sp_impl_->TranslateError(asr_str_local_name, error_code);
        auto result = swig_result.error_code;
        if (ASR::IsOk(result))
        {
            *out_string = swig_result.value.GetImpl();
        }
        return result;
    }
};

template <>
class SwigToCpp<IAsrSwigPlugin> : public IAsrPlugin
{
private:
    std::shared_ptr<IAsrSwigPlugin> sp_impl_;

public:
    explicit SwigToCpp(std::shared_ptr<IAsrSwigPlugin> sp_impl)
        : sp_impl_{sp_impl}
    {
    }
    ~SwigToCpp() = default;
    ASR_CORE_FROEIGNINTERFACEHOST_IASRBASE_AUTO_IMPL(SwigToCpp);
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override
    {
        return QueryInterface<IAsrPlugin>(this, iid, pp_out_object);
    }
    AsrResult EnumFeature(const size_t index, AsrPluginFeature* p_out_feature)
        override
    {
        auto swig_result = sp_impl_->EnumFeature(index);
        auto result = swig_result.error_code;
        *p_out_feature = swig_result.value;
        return result;
    }
    AsrResult GetFeatureInterface(
        AsrPluginFeature feature,
        IAsrBase**       pp_out_object)
    {
        auto swig_result = sp_impl_->GetFeatureInterface(feature);
        auto result = swig_result.error_code;
        if (ASR::IsOk(result))
        {
            auto interface = swig_result.value;
            if (interface->IsCastAvailable(AsrIidOf<IAsrSwigErrorLens>()))
            {
                *pp_out_object = new SwigToCpp<IAsrSwigErrorLens>(
                    std::static_pointer_cast<IAsrSwigErrorLens>(interface));
            }
            else if (interface->IsCastAvailable(AsrIidOf<IAsrSwigTask>()))
            {
                *pp_out_object = new SwigToCpp<IAsrSwigErrorLens>(
                    std::static_pointer_cast<IAsrSwigErrorLens>(interface));
            }
            else
            {
                return ASR_E_NO_INTERFACE;
            }
        }
        return result;
    }
};

namespace CppToSwig
{
    template <class T>
    concept is_asr_interface = std::is_base_of_v<IAsrBase, T>;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
