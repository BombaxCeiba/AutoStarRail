#ifndef ASR_PLUGINS_ADBCAPTUREFACTORYIMPL_H
#define ASR_PLUGINS_ADBCAPTUREFACTORYIMPL_H

#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include <AutoStarRail/Utils/Utils.hpp>

ASR_NS_BEGIN

class AdbCaptureFactoryImpl final : public IAsrCaptureFactory
{
private:
    ASR::Utils::RefCounter<AdbCaptureFactoryImpl> ref_counter_{};

public:
    int64_t    AddRef() override;
    int64_t    Release() override;
    ASR_METHOD QueryInterface(const AsrGuid& iid, void** pp_object) override;
    /**
     * @brief Require url property
     * 
     * @param p_json_config 
     * @param pp_object 
     * @return ASR_METHOD 
     */
    ASR_METHOD CreateInstance(
        IAsrReadOnlyString* p_json_config,
        IAsrCapture**       pp_object) override;
};

ASR_NS_END

#endif // ASR_PLUGINS_ADBCAPTUREFACTORYIMPL_H
