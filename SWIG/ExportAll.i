%module(directors="1") AutoStarRail

%{
#include <AutoStarRail/AsrExport.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/AsrString.hpp>

#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>

#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/ExportInterface/AsrLogger.h>
%}

%include <stdint.i>
%include <typemaps.i>
%include <cpointer.i>
%include <std_string.i>
%include <wchar.i>
%include <std_wstring.i>
%include <std_shared_ptr.i>

#ifdef SWIGJAVA
%typemap(jni) char16_t* "jstring"
%typemap(jtype) char16_t* "String"
%typemap(jstype) char16_t* "String"
%typemap(javadirectorin) char16_t* "$jniinput"
%typemap(javadirectorout) char16_t* "$javacall"

%typemap(jni) void AsrString::GetUtf16 "jstring"
%typemap(jtype) void AsrString::GetUtf16 "String"
%typemap(jstype) void AsrString::GetUtf16 "String"
%typemap(javaout) void AsrString::GetUtf16 {
    return $jnicall;
}

%typemap(in, numinputs=0) (const char16_t** out_string, size_t* out_string_size) %{
    char16_t* p_u16string;
    $1 = &p_u16string;

    size_t u16string_size;
    $2 = &u16string_size;
%}

%typemap(argout) (const char16_t** out_string, size_t* out_string_size) {
    if($1 && $2)
    {
        jsize j_length = (jsize)u16string_size;
        $result = jenv->NewString((jchar*)p_u16string, j_length);
    }
    else
    {
        jclass null_pointer_exception = jenv->FindClass("java/lang/NullPointerException");
        jenv->ThrowNew(null_pointer_exception, "Input pointer is null");
    }
}

%typemap(javain) (const char16_t* p_u16string, size_t length) "p_u16string"

%typemap(in, numinputs=1) (const char16_t* p_u16string, size_t length) %{
    const jchar* p_jstring = jenv->GetStringChars(jarg1, nullptr);
    const jsize string_length = jenv->GetStringLength(jarg1);
    static_assert(sizeof(jchar) == sizeof(char16_t), "Size of jchar is NOT equal to size of char16_t.");
    jchar* p_non_const_jstring = const_cast<jchar*>(p_jstring);
    $1 = reinterpret_cast<decltype($1)>(p_non_const_jstring);
    $2 = string_length;
%}

%typemap(out) (const char16_t* p_u16string, size_t length) {
    jenv->ReleaseStringChars(jarg1, p_jstring);
}

#endif

%include <AutoStarRail/AsrExport.h>
%include <AutoStarRail/IAsrBase.h>
%include <AutoStarRail/AsrString.hpp>

%include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
%include <AutoStarRail/PluginInterface/IAsrPlugin.h>

%include <AutoStarRail/ExportInterface/IAsrSettings.h>
%include <AutoStarRail/ExportInterface/AsrLogger.h>