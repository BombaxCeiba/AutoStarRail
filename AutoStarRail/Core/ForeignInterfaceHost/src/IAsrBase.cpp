#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <array>
#include <cstring>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>

AsrRetGuid AsrMakeAsrGuid(const char* p_guid_string)
{
    ASR_CORE_TRACE_SCOPE;

    AsrRetGuid result;
    try
    {
        result.guid =
            ASR::Core::ForeignInterfaceHost::MakeAsrGuid(p_guid_string);
        result.error_code = ASR_S_OK;
    }
    catch (const ASR::Core::InvalidGuidStringException& ex)
    {
        ASR_CORE_LOG_EXCEPTION(ex);
        result = {ASR_E_INVALID_STRING, ASR_IID_BASE};
    }
    catch (const ASR::Core::InvalidGuidStringSizeException& ex)
    {
        ASR_CORE_LOG_EXCEPTION(ex);
        result = {ASR_E_INVALID_STRING_SIZE, ASR_IID_BASE};
    }
    return result;
}