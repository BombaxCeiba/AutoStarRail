#ifndef ASR_PLUGINS_ADBCAPTUREIMPL_H
#define ASR_PLUGINS_ADBCAPTUREIMPL_H

#include <AutoStarRail/AsrConfig.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/Utils/Expected.h>
#include <filesystem>
#include <cstdint>

ASR_NS_BEGIN

class AdbCapture final : public IAsrCapture
{
private:
    ASR::Utils::RefCounter<AdbCapture> ref_counter_{};
    std::string                        capture_png_command_;
    std::string                        capture_raw_by_nc_command_;
    std::string                        capture_gzip_raw_command_;
    std::string                        get_screen_size_command_;

    enum class Type
    {
        Png,
        RawByNc,
        RawWithGZip,
        Raw
    };

    Type type_{Type::RawWithGZip};

public:
    struct Size
    {
        int32_t width;
        int32_t height;
    };

private:
    Size adb_device_screen_size_{};

    ASR::Utils::Expected<Size> GetDeviceSize() const;

    void CaptureRawWithGZip();
    void CaptureRaw();
    void CapturePng();
    void CaptureRawByNc();
    void AutoDetectType();

public:
    AdbCapture(
        const std::filesystem::path& adb_path,
        std::string_view             adb_device_serial);
    ~AdbCapture() = default;
    int64_t   AddRef() override;
    int64_t   Release() override;
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    AsrResult Capture(IAsrImage** pp_out_image) override;
};

ASR_NS_END

#endif
