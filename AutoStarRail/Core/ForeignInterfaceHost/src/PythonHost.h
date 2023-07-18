#ifndef ASR_CORE_FOREIGNINTERFACEHOST_PYTHONHOST_H
#define ASR_CORE_FOREIGNINTERFACEHOST_PYTHONHOST_H

#ifdef ASR_EXPORT_PYTHON

#include "IForeignLanguageRuntime.h"

/**
 * @brief 在Python.h中声明，其中：typedef _object* PyObject;
 *
 */
struct _object;

#define ASR_NS_PYTHONHOST_BEGIN                                                \
    namespace PythonHost                                                       \
    {

#define ASR_NS_PYTHONHOST_END }

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_PYTHONHOST_BEGIN

struct PythonRuntimeDesc : public ForeignLanguageRuntimeFactoryDescBase
{
};

auto CreateForeignLanguageRuntime(const PythonRuntimeDesc& desc)
    -> ASR::Utils::Expected<AsrPtr<IForeignLanguageRuntime>>;

class PyObjectPtr
{
    struct AttachOnly
    {
    };
    _object* ptr_{nullptr};
    explicit PyObjectPtr(_object* ptr, [[maybe_unused]] AttachOnly) noexcept;

public:
    explicit PyObjectPtr(decltype(nullptr) p = nullptr) noexcept;
    explicit PyObjectPtr(_object* ptr);
    PyObjectPtr(const PyObjectPtr& other);
    PyObjectPtr& operator=(const PyObjectPtr& other);
    PyObjectPtr(PyObjectPtr&& other) noexcept;
    PyObjectPtr& operator=(PyObjectPtr&& other) noexcept;
    ~PyObjectPtr();

    [[nodiscard]]
    static PyObjectPtr Attach(_object* ptr);
    [[nodiscard]]
    _object** Put() noexcept;
    _object*  Get() const noexcept;
    [[nodiscard]]
    _object* Detach() noexcept;
    bool     operator==(const _object* const other) const noexcept;
    bool     operator==(PyObjectPtr other) const noexcept;
    explicit operator bool() const noexcept;
};

ASR_NS_PYTHONHOST_END

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_EXPORT_PYTHON

#endif // ASR_CORE_FOREIGNINTERFACEHOST_PYTHONHOST_H
