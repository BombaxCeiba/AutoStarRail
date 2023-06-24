#ifdef ASR_EXPORT_JAVA

#include "JavaHost.h"
#include <jni.h>
#include <AutoStarRail/Utils/Utils.hpp>
#include <boost/dll/shared_library.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_JAVAHOST_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

struct JvmDeleter
{
    void operator()(JavaVM* p_vm) const noexcept { p_vm->DestroyJavaVM(); }
};
using UniqueJvmPointer = std::unique_ptr<JavaVM, JvmDeleter>;

ASR_NS_ANONYMOUS_DETAILS_END

class SingletonJvm
{
    static boost::dll::shared_library jvm_dll_;
    static decltype(&JNI_CreateJavaVM) func_jni_create_jvm_;
    static void LoadJvm(const std::filesystem::path& jvm_path)
    {
        jvm_dll_ = boost::dll::shared_library(jvm_path.wstring());
        func_jni_create_jvm_ =
            jvm_dll_.get<decltype(JNI_CreateJavaVM)>("JNI_CreateJavaVM");
    }
    // static JavaVM* GetJvm() noexcept {}
};

ASR_DEFINE_VARIABLE(SingletonJvm::jvm_dll_){};
ASR_DEFINE_VARIABLE(SingletonJvm::func_jni_create_jvm_){nullptr};

ASR_NS_JAVAHOST_END

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_EXPORT_JAVA