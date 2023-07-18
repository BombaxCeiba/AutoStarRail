#include "AutoStarRail/IAsrBase.h"
#include <cstdint>
#ifdef ASR_EXPORT_PYTHON

#include "PythonHost.h"
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Core/Exceptions/PythonException.h>
#include <Python.h>
#include <utility>
#include <string_view>
#include <stdexcept>
#include <tuple>

static_assert(
    std::is_same_v<_object*, PyObject*>,
    "Type PyObject is not type _object. Consider to check if Python.h change "
    "the type declaration.");

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_PYTHONHOST_BEGIN

PyObjectPtr::PyObjectPtr(
    PyObject* ptr,
    [[maybe_unused]] PyObjectPtr::AttachOnly) noexcept
    : ptr_(ptr)
{
}

PyObjectPtr::PyObjectPtr(decltype(nullptr) p) noexcept : ptr_(p) {}

PyObjectPtr::PyObjectPtr(PyObject* ptr) : ptr_(ptr) { Py_INCREF(ptr_); }

PyObjectPtr::PyObjectPtr(const PyObjectPtr& other) : ptr_(other.ptr_)
{
    Py_XINCREF(ptr_);
}

PyObjectPtr::PyObjectPtr(PyObjectPtr&& other) noexcept
    : ptr_(std::exchange(other.ptr_, nullptr))
{
}

PyObjectPtr::~PyObjectPtr() { Py_XDECREF(ptr_); }

PyObjectPtr PyObjectPtr::Attach(PyObject* ptr)
{
    return PyObjectPtr(ptr, AttachOnly{});
}

PyObject** PyObjectPtr::Put() noexcept
{
    Py_XDECREF(ptr_);
    ptr_ = nullptr;
    return &ptr_;
}

bool PyObjectPtr::operator==(const PyObject* const p) const noexcept
{
    return p == ptr_;
}

bool PyObjectPtr::operator==(PyObjectPtr p) const noexcept
{
    return p.Get() == ptr_;
}

PyObject* PyObjectPtr::Get() const noexcept { return ptr_; }

PyObjectPtr::operator bool() const noexcept
{
    return Get() == PyObjectPtr() ? false : true;
}

PyObject* PyObjectPtr::Detach() noexcept
{
    return std::exchange(ptr_, nullptr);
}

ASR_NS_ANONYMOUS_DETAILS_BEGIN

PyObjectPtr PyUnicodeFromWString(const std::wstring_view std_wstring_view)
{
    return PyObjectPtr::Attach(PyUnicode_FromWideChar(
        std_wstring_view.data(),
        std_wstring_view.size()));
}

ASR_NS_ANONYMOUS_DETAILS_END

class PythonResult : public ASR::Utils::NonCopyableAndNonMovable
{
    PyObjectPtr ptr_;
    bool        owned_;

    void DetachIfNotOwned()
    {
        if (!owned_)
        {
            std::ignore = ptr_.Detach();
        }
    }

    void CheckPointer() const
    {
        if (!ptr_)
        {
            RaiseIfError();
        }
    }

    constexpr static PyObject* TEST_PTR = nullptr;

public:
    explicit PythonResult(PyObjectPtr ptr) : ptr_{ptr}, owned_{true}
    {
        CheckPointer();
    }
    explicit PythonResult(PyObject* ptr) : ptr_{ptr}, owned_{false}
    {
        CheckPointer();
    }
    ~PythonResult() { DetachIfNotOwned(); }
    /**
     * @brief 验证构造此方法的指针是否为空，若为空，抛出
     PythonError异常； 否则将指针作为参数执行传入的函数
     *
     * @tparam F Callable对象的类型
     * @param func Callable对象，接收一个类型为PyObject*的输入，
        输出为PyObject*时，暗示这一用户不具有对象所有权；
        输出为PyObjectPtr时，暗示用户已经妥善处理的对象引用问题；
        输出为其它时，暗示下一次调用then时仍然使用当前的PyObject。
     * @return PythonResult& 本对象的引用
     */
    template <class F>
    PythonResult& then(F func)
    {
        if constexpr (std::is_same_v<decltype(func(TEST_PTR)), PyObject*>)
        {
            auto func_result = func(ptr_.Get());
            DetachIfNotOwned();
            ptr_ = PyObjectPtr::Attach(func_result);
            owned_ = false;
        }
        else if constexpr (std::
                               is_same_v<decltype(func(TEST_PTR)), PyObjectPtr>)
        {
            auto func_result = func(ptr_.Get());
            DetachIfNotOwned();
            ptr_ = func_result;
            owned_ = true;
        }
        else
        {
            func(ptr_.Get());
        }

        CheckPointer();
        return *this;
    }

    static void RaiseIfError()
    {
        PyObject*   p_type{nullptr};
        PyObject*   p_value{nullptr};
        PyObject*   p_trace_back{nullptr};
        const char* p_py_error_msg{nullptr};
        Py_ssize_t  error_text_size{0};

        PyErr_Fetch(&p_type, &p_value, &p_trace_back);
        if (p_type == nullptr)
        {
            goto on_error_not_found;
        }
        p_py_error_msg = PyUnicode_AsUTF8AndSize(p_value, &error_text_size);
        if (p_py_error_msg)
        {
            goto on_error_not_found;
        }
        ASR_CORE_LOG_ERROR(p_py_error_msg);
        return;

    on_error_not_found:
        ASR_CORE_LOG_WARN("Error happened when calling python code,"
                          "but it seems that no error is set in Python.");
    }
};

class PyInterpreter
{
    PyObjectPtr sys_module_;

    struct InitException : std::runtime_error
    {
        using Base = std::runtime_error;
        InitException()
            : Base{"Error occurred when initialize Python interpreter."}
        {
        }
        [[noreturn]]
        static void Raise()
        {
            throw InitException();
        }
    };

    void Init()
    {
        if (Py_IsInitialized())
        {
            return;
        }
        Py_Initialize();
        if (!Py_IsInitialized())
        {
            InitException::Raise();
        }
        auto py_str_sys = Details::PyUnicodeFromWString(L"sys");
        sys_module_ = PyObjectPtr::Attach(PyImport_Import(py_str_sys.Get()));
        PythonResult(sys_module_)
            .then(
                [](auto sys_module) {
                    return PyObjectPtr::Attach(
                        PyObject_GetAttrString(sys_module, "path"));
                })
            .then(
                [](auto sys_path)
                {
                    const auto plugin_folder =
                        std::wstring{L"sys.path.insert(0,r\""}
                        + std::filesystem::current_path().wstring()
                        + std::wstring{L"\")"};
                    const auto py_import_path =
                        Details::PyUnicodeFromWString(plugin_folder);
                    const auto py_zero =
                        PyObjectPtr::Attach(PyLong_FromLong(0));

                    PythonResult(PyObjectPtr::Attach(PyTuple_New(2)))
                        .then(
                            [value_py_import_path = py_import_path,
                             value_py_zero = py_zero](auto arguments)
                            {
                                int set_tuple_result{0};
                                set_tuple_result = PyTuple_SetItem(
                                    arguments,
                                    0,
                                    value_py_zero.Get());
                                if (set_tuple_result == -1)
                                {
                                    throw ASR::Core::PythonException{""};
                                }
                                set_tuple_result = PyTuple_SetItem(
                                    arguments,
                                    1,
                                    value_py_import_path.Get());
                                if (set_tuple_result == -1)
                                {
                                }
                            })
                        .then(
                            [value_sys_path = sys_path](auto arguments)
                            {
                                return PyObjectPtr::Attach(PyObject_Call(
                                    value_sys_path,
                                    arguments,
                                    nullptr));
                            });
                });
    }
};

class PythonRuntime final : public IForeignLanguageRuntime
{
private:
    PyObjectPtr p_plugin_module;

public:
    PythonRuntime();

    int64_t   AddRef() override { return 1; };
    int64_t   Release() override { return 1; };
    AsrResult QueryInterface(const AsrGuid&, void**) override
    {
        return ASR_E_NO_IMPLEMENTATION;
    }
    auto LoadPlugin(const std::filesystem::path& path)
        -> ASR::Utils::Expected<CommonPluginPtr> override;
};

ASR_NS_PYTHONHOST_END

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_EXPORT_PYTHON