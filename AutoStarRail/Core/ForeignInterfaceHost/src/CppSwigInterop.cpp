#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/QueryInterfaceImpl.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <boost/bimap.hpp>
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

class CppSwigIidBiMap
{
    using BiMap = boost::bimaps::bimap<AsrGuid, AsrGuid>;

    /**
     * @brief Left side is cpp interface id, right side is swig interface id.
     *
     * @return BiMap
     */
    static BiMap InitializeBiMap()
    {
        std::initializer_list<typename BiMap::value_type> init_list = {
            {ASR_IID_BASE, ASR_IID_SWIG_BASE},
            {ASR_IID_PLUGIN, ASR_IID_SWIG_PLUGIN}};
        return BiMap{init_list.begin(), init_list.end()};
    }

    const boost::bimaps::bimap<AsrGuid, AsrGuid> bi_map_ = InitializeBiMap();

public:
    std::optional<AsrGuid> FromSwig(const AsrGuid& from) const
    {
        if (const auto it = bi_map_.right.find(from); it != bi_map_.right.end())
        {
            return {it->second};
        }
        return {};
    }

    std::optional<AsrGuid> FromCpp(const AsrGuid& from) const
    {
        if (const auto it = bi_map_.left.find(from); it != bi_map_.left.end())
        {
            return {it->second};
        }
        return {};
    }

    template <is_asr_interface T>
    AsrGuid FromCpp() const
    {
        return bi_map_.left.at(AsrIidOf<T>());
    }

    template <is_asr_swig_interface T>
    AsrGuid FromSwig() const
    {
        return bi_map_.right.at(AsrIidOf<T>());
    }
};

const CppSwigIidBiMap cpp_swig_iid_bi_map{};

template <class T>
class SwigToCpp;

template <>
class SwigToCpp<IAsrSwigBase> final : public IAsrBase
{
private:
    std::shared_ptr<IAsrSwigBase> sp_impl_;

public:
    template <class Other>
    explicit SwigToCpp(SwigToCpp<Other> other) : sp_impl_{other.sp_impl_}
    {
    }
    ~SwigToCpp() = default;
    ASR_CORE_FROEIGNINTERFACEHOST_IASRBASE_AUTO_IMPL(SwigToCpp);
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override
    {
        return ASR::Core::ForeignInterfaceHost::QueryInterface<IAsrBase>(
            this,
            iid,
            pp_out_object);
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
    AsrResult GetInterface(
        const AsrGuid& feature_iid,
        IAsrBase**     pp_out_feature_interface) override
    {
        auto swig_result = sp_impl_->GetInterface(feature_iid);

        auto result = swig_result.error_code;
        if (ASR::IsOk(result))
        {
            const auto opt_swig_iid = cpp_swig_iid_bi_map.FromCpp(feature_iid);
            if (opt_swig_iid)
            {
                const auto swig_iid = opt_swig_iid.value();
                if (const auto is_feature_existence =
                        swig_result.value->IsCastAvailable(swig_iid);
                    ASR::IsOk(is_feature_existence))
                {
                }
            }
            // TODO: Throw exception if swig iid is not found.
        }
        else
        {
            *pp_out_feature_interface = nullptr;
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
