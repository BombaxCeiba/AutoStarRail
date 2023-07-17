#ifndef ASR_ASRPTR_HPP
#define ASR_ASRPTR_HPP

#include <AutoStarRail/IAsrBase.h>
#include <functional>
#include <utility>

ASR_NS_BEGIN

template <class T>
class AsrPtr
{
    template <class U>
    friend class AsrPtr;

protected:
    T* ptr_;

    void InternalAddRef() const
    {
        if (ptr_)
        {
            ptr_->AddRef();
        }
    }

    void InternalRelease() const
    {
        if (ptr_)
        {
            ptr_->Release();
        }
    }

    template <class U>
    void InternalCopy(U* p_other)
    {
        if (p_other != ptr_)
        {
            InternalRelease();
            ptr_ = p_other;
            InternalAddRef();
        }
    }

public:
    AsrPtr() : ptr_(nullptr) {}
    AsrPtr(decltype(nullptr)) : ptr_(nullptr) {}
    AsrPtr(T* p) : ptr_(p) { InternalAddRef(); }
    AsrPtr(const AsrPtr& other) : ptr_(other.Get()) { InternalAddRef(); }
    template <class U>
    AsrPtr(const AsrPtr<U>& other) : ptr_(other.Get())
    {
        InternalAddRef();
    }
    AsrPtr& operator=(const AsrPtr& other)
    {
        InternalCopy(other.ptr_);
        return *this;
    }
    template <class U>
    AsrPtr& operator=(const AsrPtr<U>& other)
    {
        InternalCopy(other.ptr_);
        return *this;
    }
    AsrPtr(AsrPtr&& other) noexcept : ptr_(std::exchange(other.ptr_, nullptr))
    {
    }
    template <class U>
    AsrPtr(AsrPtr<U>&& other) noexcept
        : ptr_(std::exchange(other.ptr_, nullptr))
    {
    }
    AsrPtr& operator=(AsrPtr&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            InternalRelease();
            ptr_ = std::exchange(other.ptr_, nullptr);
        }
        return *this;
    }
    template <class U>
    AsrPtr& operator=(AsrPtr&& other) noexcept
    {
        InternalRelease();
        ptr_ = std::exchange(other.ptr_, nullptr);
        return *this;
    }
    ~AsrPtr() { InternalRelease(); }
    T*   operator->() const { return ptr_; }
    T&   operator*() const { return *ptr_; }
    bool operator==(const AsrPtr<T>& other) const { return ptr_ == other.ptr_; }
    template <class Other>
    Other* As(const AsrGuid& id) const
    {
        void* result = nullptr;
        if (ptr_)
        {
            ptr_->QueryInterface(id, &result);
        }
        return static_cast<Other*>(result);
    }
    T* Reset() const
    {
        InternalRelease();
        return std::exchange(ptr_, nullptr);
    }
    T*  Get() const { return ptr_; }
    T** Put()
    {
        InternalRelease();
        ptr_ = nullptr;
        return &ptr_;
    }
    void**      PutVoid() { return reinterpret_cast<void**>(Put()); }
    friend void swap(AsrPtr& lhs, AsrPtr& rhs) noexcept
    {
        std::swap(lhs.m_ptr, rhs.m_ptr);
    }
    auto operator<=>(const AsrPtr& other) const { return other.ptr_ <=> ptr_; };
    static AsrPtr Attach(T* p)
    {
        AsrPtr result{nullptr};
        *result.Put() = p;
        return result;
    }
};

#if __cplusplus >= 201703L
template <class T>
AsrPtr(T*) -> AsrPtr<T>;
#endif // __cplusplus >= 201703L

ASR_NS_END

template <class T>
struct std::hash<ASR::AsrPtr<T>>
{
    size_t operator()(const ASR::AsrPtr<T>& ptr) const
    {
        return std::hash<T*>()(ptr.Get());
    }
};

#endif // ASR_ASRPTR_HPP
