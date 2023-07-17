#ifndef ASR_CORE_FOREIGNINTERFACEHOST_CPPSWIGINTEROP_H
#define ASR_CORE_FOREIGNINTERFACEHOST_CPPSWIGINTEROP_H

#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/PluginInterface/IAsrTask.h>
#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/QueryInterfaceImpl.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/IsCastAvailableImpl.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <string_view>
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
        IAsrReadOnlyString** pp_out_string) override;
};

// template <>
// class SwigToCpp<IAsrSwigPlugin> final
//     : public SwigToCppBase<IAsrSwigPlugin, IAsrPlugin>
// {
// public:
//     ASR_USING_BASE_CTOR(SwigToCppBase);
//     ~SwigToCpp() = default;
//     ASR_CORE_FROEIGNINTERFACEHOST_IASRBASE_AUTO_IMPL(SwigToCpp);

//     AsrResult EnumFeature(const size_t index, AsrPluginFeature* p_out_feature)
//         override;
//     AsrResult GetFeatureInterface(
//         AsrPluginFeature feature,
//         IAsrBase**       pp_out_object) override;
//     AsrResult GetDescription(IAsrReadOnlyString** pp_out_description) override;
// };

// ---------------------------------------------------------------------------

template <class SwigT>
class CppToSwig;

template <is_asr_swig_interface SwigT, is_asr_interface T>
class CppToSwigBase : public SwigT
{
protected:
    ASR::AsrPtr<T> p_impl_;

public:
    template <class Other>
    CppToSwigBase(ASR::AsrPtr<Other> p_impl) : p_impl_{p_impl}
    {
    }
    template <class Other>
    explicit CppToSwigBase(CppToSwig<Other> other) : p_impl_{other.p_impl_}
    {
    }
    ~CppToSwigBase() override = default;
};

// template <>
// class CppToSwig<IAsrPlugin> final
//     : public CppToSwigBase<IAsrSwigPlugin, IAsrPlugin>
// {
// public:
//     ASR_USING_BASE_CTOR(CppToSwigBase);
//     ~CppToSwig() override = default;

//     AsrResult           IsCastAvailable(const AsrGuid& iid) override;
//     AsrRetPluginFeature EnumFeature(const size_t index) override;
//     AsrRetSwigBase      GetFeatureInterface(AsrPluginFeature feature) override;
//     AsrRetString        GetDescription() override;
// };

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_CPPSWIGINTEROP_H
