#ifndef ASR_UTILS_IASRBASEADAPTERUTILS_H
#define ASR_UTILS_IASRBASEADAPTERUTILS_H

#include <AutoStarRail/Utils/Config.h>
#include <AutoStarRail/IAsrBase.h>
#include <memory>

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
    T*        pointer_;
    AsrGuid   expected_guid_;
    void**    pp_object_;
    AsrResult result_{ASR_S_OK};

public:
    explicit AsrInterfaceConverter(
        T*             pointer,
        const AsrGuid& expected_iid,
        void**         pp_object)
        : pointer_{pointer}, expected_guid_{expected_iid}, pp_object_{pp_object}
    {
    }
    template <class Other>
    AsrInterfaceConverter& IsExpected(const AsrGuid& convertible_guid)
    {
        if (!pointer_)
        {
            return *this;
        }
        if (convertible_guid == expected_guid_)
        {
            *pp_object_ = static_cast<Other*>(pointer_);
            pointer_->AddRef();
            result_ = ASR_S_OK;
            pointer_ = nullptr;
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

ASR_UTILS_NS_END

#endif // ASR_UTILS_IASRBASEADAPTERUTILS_H
