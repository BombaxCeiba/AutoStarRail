#ifndef ASR_CORE_LOGGER_LOGGER_H
#define ASR_CORE_LOGGER_LOGGER_H

#include <AutoStarRail/IAsrBase.h>
#include <memory>
#include <spdlog/spdlog.h>

#define ASR_CORE_LOG_INFO(...)                                                 \
    SPDLOG_LOGGER_INFO(ASR::Core::g_logger, __VA_ARGS__)
#define ASR_CORE_LOG_TRACE(...)                                                \
    SPDLOG_LOGGER_TRACE(ASR::Core::g_logger, __VA_ARGS__)
#define ASR_CORE_LOG_DEBUG(...)                                                \
    SPDLOG_LOGGER_DEBUG(ASR::Core::g_logger, __VA_ARGS__)
#define ASR_CORE_LOG_WARN(...)                                                 \
    SPDLOG_LOGGER_WARN(ASR::Core::g_logger, __VA_ARGS__)
#define ASR_CORE_LOG_ERROR(...)                                                \
    SPDLOG_LOGGER_ERROR(ASR::Core::g_logger, __VA_ARGS__)
#define ASR_CORE_LOG_CRITICAL(...)                                             \
    SPDLOG_LOGGER_CRITICAL(ASR::Core::g_logger, x)

#ifdef _MSC_VER
#define ASR_FUNCTION __FUNCSIG__
#else
#define ASR_FUNCTION __PRETTY_FUNCTION__
#endif // _MSC_VER

#define ASR_CORE_TRACE_SCOPE                                                   \
    ASR::Core::TraceScope ASR_TOKEN_PASTE(                                     \
        _asr_reserved_logger_tracer_,                                          \
        __LINE__)                                                              \
    {                                                                          \
        __FILE__, __LINE__, ASR_FUNCTION                                       \
    }

#define ASR_CORE_LOG_EXCEPTION(ex) ASR_CORE_LOG_ERROR(ex.what())

#define ASR_CORE_LOG_JSON_EXCEPTION(ex, key, json)                             \
    ASR_CORE_LOG_ERROR(ex.what());                                             \
    ASR_CORE_LOG_ERROR("JSON Key: {}", key);                                   \
    ASR_CORE_LOG_ERROR("----JSON dump begin----");                             \
    ASR_CORE_LOG_ERROR(json.dump());                                           \
    ASR_CORE_LOG_ERROR("----JSON dump end----")

ASR_NS_BEGIN

namespace Core
{
    extern const std::shared_ptr<spdlog::logger> g_logger;
    extern const char* const                     g_logger_name;

    class TraceScope
    {
        const char* const file_;
        int               line_;
        const char* const func_;

    public:
        TraceScope(const char* const file, int line, const char* const func);
        ~TraceScope();
    };

    template <class T>
    void LogException(const T& ex)
    {
        ASR_CORE_LOG_ERROR(ex.what());
    }

    void LogException(const auto& ex, const auto& json, const auto& key)
    {
        LogException(ex);
        ASR_CORE_LOG_ERROR("JSON Key: {}", key);
        ASR_CORE_LOG_ERROR("----JSON dump begin----");
        ASR_CORE_LOG_ERROR(json.dump());
        ASR_CORE_LOG_ERROR("----JSON dump end----");
    }
}

ASR_NS_END

#endif // ASR_CORE_LOGGER_LOGGER_H
