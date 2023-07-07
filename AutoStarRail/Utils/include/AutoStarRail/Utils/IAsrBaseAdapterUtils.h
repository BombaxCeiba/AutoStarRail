#ifndef ASR_UTILS_IASRBASEADAPTERUTILS_H
#define ASR_UTILS_IASRBASEADAPTERUTILS_H

#include <AutoStarRail/Utils/Config.h>
#include <AutoStarRail/IAsrBase.h>
#include <memory>

ASR_NS_BEGIN

namespace Core::ForeignInterfaceHost
{
    template <class T>
    class SwigToCpp;
}

ASR_NS_END

ASR_UTILS_NS_BEGIN

template <class T>
    requires std::is_base_of_v<IAsrBase, T>
std::shared_ptr<T> MakeSharedPtr(T* pointer)
{
    auto result = std::shared_ptr<T>(pointer, IAsrBaseDeleter{});
    pointer->AddRef();
    return result;
}

template <class T>
    requires std::is_base_of_v<IAsrBase, T>
class AsrInterfaceConverter
{
    T*        input_this_;
    AsrGuid   expected_guid_;
    void**    pp_object_;
    AsrResult result_{ASR_S_OK};

public:
    explicit AsrInterfaceConverter(
        T*             input_this,
        const AsrGuid& expected_iid,
        void**         pp_object)
        : input_this_{input_this}, expected_guid_{expected_iid},
          pp_object_{pp_object}
    {
    }
    template <class Other>
    AsrInterfaceConverter& TryConvert()
    {
        if (!input_this_)
        {
            return *this;
        }
        if (AsrIidOf<Other>() == expected_guid_)
        {
            *pp_object_ = static_cast<Other*>(input_this_);
            input_this_->AddRef();
            result_ = ASR_S_OK;
            input_this_ = nullptr;
            return *this;
        }
        else
        {
            result_ = ASR_E_NO_INTERFACE;
            return *this;
        }
    }
    AsrResult GetResult() const
    {
        if (result_ != ASR_S_OK)
        {
            *pp_object_ = nullptr;
        }
        return result_;
    };
};
template <class T>
AsrInterfaceConverter(T*, const AsrGuid&, void**) -> AsrInterfaceConverter<T>;

template <class T>
class AsrSwigToCppInterfaceConverter
{
    T*        input_this_;
    AsrGuid   expected_guid_;
    void**    pp_object_;
    AsrResult result_{ASR_S_OK};

public:
    explicit AsrSwigToCppInterfaceConverter(
        T*             input_this,
        const AsrGuid& expected_iid,
        void**         pp_object)
        : input_this_{input_this}, expected_guid_{expected_iid},
          pp_object_{pp_object}
    {
    }
    template <class To, class SwigTo>
    AsrSwigToCppInterfaceConverter& TryConvert()
    {
        if (!input_this_)
        {
            return *this;
        }
        if (AsrIidOf<To>() == expected_guid_)
        {
            if constexpr (std::is_base_of_v<To, T>)
            {
                input_this_->AddRef();
                *pp_object_ = static_cast<To*>(input_this_);
            }
            else
            {
                auto up_result = std::make_unique<
                    ASR::Core::ForeignInterfaceHost::SwigToCpp<SwigTo>>(
                    input_this_);
                input_this_->AddRef();
                *pp_object_ = up_result.release();
            }
            result_ = ASR_S_OK;
            input_this_ = nullptr;
            return *this;
        }
        else
        {
            result_ = ASR_E_NO_INTERFACE;
            return *this;
        }
    }
    AsrResult GetResult() const
    {
        if (result_ != ASR_S_OK)
        {
            *pp_object_ = nullptr;
        }
        return result_;
    };
};
template <class T>
AsrSwigToCppInterfaceConverter(T*, const AsrGuid&, void**)
    -> AsrSwigToCppInterfaceConverter<T>;

ASR_UTILS_NS_END

#endif // ASR_UTILS_IASRBASEADAPTERUTILS_H
