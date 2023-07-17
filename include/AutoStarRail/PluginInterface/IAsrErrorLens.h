#ifndef ASR_IERRORLENS_H
#define ASR_IERRORLENS_H

#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/AsrString.hpp>
#include <memory>

// {10963BC9-72FD-4B57-A7BA-98F431C234E4}
ASR_DEFINE_GUID(
    ASR_IID_ERROR_LENS,
    IAsrErrorLens,
    0x10963bc9,
    0x72fd,
    0x4b57,
    0xa7,
    0xba,
    0x98,
    0xf4,
    0x31,
    0xc2,
    0x34,
    0xe4);
SWIG_IGNORE(IAsrErrorLens)
ASR_INTERFACE IAsrErrorLens : public IAsrBase
{
    ASR_METHOD TranslateError(
        IAsrReadOnlyString * local_name,
        AsrResult error_code,
        IAsrReadOnlyString * *out_string) = 0;
};

// {0B9B86B2-F8A6-4EA4-90CF-3756AD318B83}
ASR_DEFINE_GUID(
    ASR_IID_SWIG_ERROR_LENS,
    IAsrSwigErrorLens,
    0xb9b86b2,
    0xf8a6,
    0x4ea4,
    0x90,
    0xcf,
    0x37,
    0x56,
    0xad,
    0x31,
    0x8b,
    0x83);
SWIG_ENABLE_SHARED_PTR(IAsrSwigErrorLens)
SWIG_ENABLE_DIRECTOR(IAsrSwigErrorLens)
ASR_INTERFACE IAsrSwigErrorLens : public IAsrSwigBase
{
    virtual AsrRetString TranslateError(
        const AsrString local_name,
        AsrResult       error_code) = 0;
};

SWIG_IGNORE(GetErrorExplanation)
/**
 * @brief Get the error explanation. If return value is not ASR_S_OK,
        then the pp_out_error_explanation points to a string
        that explains why calling this function failed
 * @param p_error_generating_interface
 * @param local_name
 * @param error_code
 * @param pp_out_error_explanation It is an Error message when this function
  success. Oterhwise it is a string that explains why calling this function
  failed.
 * @return AsrResult
 */
ASR_C_API AsrResult GetErrorExplanation(
    IAsrBase*            p_error_generating_interface,
    IAsrReadOnlyString*  local_name,
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_error_explanation);

/**
 * @brief See GetErrorExplanation
 * 
 * @param p_error_generating_interface 
 * @param local_name 
 * @param error_code 
 * @return AsrRetString 
 */
ASR_API AsrRetString GetErrorExplanation(
    std::shared_ptr<IAsrSwigBase> p_error_generating_interface,
    AsrString                     local_name,
    AsrResult                     error_code);

#endif // ASR_IERRORLENS_H
