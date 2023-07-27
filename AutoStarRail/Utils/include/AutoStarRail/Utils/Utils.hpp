#ifndef ASR_UTILS_UTILS_HPP
#define ASR_UTILS_UTILS_HPP

#include <utility>
#include <atomic>
#include <memory>
#include <AutoStarRail/Utils/Config.h>

ASR_UTILS_NS_BEGIN

struct NonCopyable
{
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

struct NonMovable
{
    NonMovable() = default;
    ~NonMovable() = default;
    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;
};

struct NonCopyableAndNonMovable
{
    NonCopyableAndNonMovable() = default;
    ~NonCopyableAndNonMovable() = default;
    NonCopyableAndNonMovable(const NonCopyableAndNonMovable&) = delete;
    NonCopyableAndNonMovable& operator=(const NonCopyableAndNonMovable&) =
        delete;
    NonCopyableAndNonMovable(NonCopyableAndNonMovable&&) = delete;
    NonCopyableAndNonMovable& operator=(NonCopyableAndNonMovable&&) = delete;
};

template <class T, class... Args>
T MakeObjectLike(const T&, Args&&... args)
{
    T result{std::forward<Args>(args)...};
    return result;
}

template <class OnExitFunc>
class OnExit : public NonCopyableAndNonMovable
{
    ASR_USING_BASE_CTOR(NonCopyableAndNonMovable);

    [[no_unique_address]] OnExitFunc on_exit_func_;

public:
    template <class F>
    OnExit(F&& on_exit_func) : on_exit_func_{std::forward<F>(on_exit_func)}
    {
    }

    OnExit() : on_exit_func_{} {}

    ~OnExit() { on_exit_func_(); }
};
template <class OnExitFunc>
OnExit(OnExitFunc) -> OnExit<OnExitFunc>;

template <class T, class OnExitFunc>
class ScopeGuard : public NonCopyableAndNonMovable
{
    ASR_USING_BASE_CTOR(NonCopyableAndNonMovable);

    T                                value_;
    [[no_unique_address]] OnExitFunc on_exit_func_;

public:
    template <class F>
    ScopeGuard(const T& value, F&& on_exit_func)
        : value_{value}, on_exit_func_{std::forward<F>(on_exit_func)}
    {
    }

    template <class F>
    ScopeGuard(const T& value) : value_{value}, on_exit_func_{}
    {
    }

    ~ScopeGuard() { on_exit_func_(value_); }
};

template <class OnExitFunc>
class ScopeGuardVoid : public NonCopyableAndNonMovable
{
    ASR_USING_BASE_CTOR(NonCopyableAndNonMovable);

    [[no_unique_address]] OnExitFunc on_exit_func_;

public:
    template <class FInit, class FDestroy>
    ScopeGuardVoid(FInit on_init_func, FDestroy&& on_exit_func)
        : on_exit_func_{std::forward<FDestroy>(on_exit_func)}
    {
        on_init_func();
    }
    template <class FInit>
    ScopeGuardVoid(FInit on_init_func) : on_exit_func_{}
    {
        on_init_func();
    }
    ~ScopeGuardVoid() { on_exit_func_(); }
};
template <class FInit, class FDestroy>
ScopeGuardVoid(FInit, FDestroy&&) -> ScopeGuardVoid<FDestroy>;

template <class T>
class IfNotNull : public NonCopyableAndNonMovable
{
private:
    T value_;

public:
    IfNotNull(T value) : value_{value} {}
    template <class F>
    IfNotNull& operator>>(F f)
    {
        if (value_ != nullptr)
        {
            f(value_);
        }
        return *this;
    }
};

template <class T>
class RefCounter
{
private:
    std::atomic_uint64_t ref_count_{0};

public:
    RefCounter() = default;
    ~RefCounter() = default;
    int64_t AddRef()
    {
        ref_count_ += 1;
        return ref_count_;
    }
    int64_t Release(T* p_managed_object)
    {
        ref_count_ -= 1;
        if (ref_count_ == 0)
        {
            delete p_managed_object;
        }
        return 0;
    }
};

template <class T>
T MakeObjectAndSetSize()
{
    T result{};
    result.size = sizeof(T);
    return result;
}

template <typename T, class InitFunc>
class Singleton : public NonCopyableAndNonMovable
{
public:
    Singleton() = delete;
    ~Singleton() = delete;

    static T& instance()
    {
        static T object{InitFunc{}()};
        return object;
    }
};

template <class C>
C MakeEmptyContainer()
{
    using Allocator = typename C::allocator_type;
    C result{std::size_t{0}, {}, Allocator{}};
    return result;
}

template <class C>
C MakeContainerOfSize(const std::size_t size)
{
    using Allocator = typename C::allocator_type;
    C result{size, {}, Allocator{}};
    return result;
}

template <auto FirstValue, auto... Values>
decltype(FirstValue) get_first_element()
{
    return FirstValue;
}

template <auto... Values>
constexpr bool IsMatchOneOf(const auto value)
{
    if constexpr (std::is_pointer_v<decltype(get_first_element<Values...>())>)
    {
        return ((value == *Values) || ...);
    }
    else
    {
        return ((value == Values) || ...);
    }
}

ASR_UTILS_NS_END

#endif // ASR_UTILS_UTILS_HPP
