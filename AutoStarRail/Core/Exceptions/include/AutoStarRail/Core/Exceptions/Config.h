/**
 * @file Config.h
 * @author Dusk_NM02
 * @brief Notice: Every exception class should return an UTF-8 string.
 *
 * @copyright Copyright (c) 2023 Dusk.
 *
 */

#ifndef ASR_CORE_EXCEPTION_CONFIG_H
#define ASR_CORE_EXCEPTION_CONFIG_H

#include <AutoStarRail/AsrConfig.h>
#include <stdexcept>
#include <cstdint>

#define ASR_CORE_EXCEPTIONS_NS_BEGIN                                           \
    ASR_NS_BEGIN                                                               \
    namespace Core                                                             \
    {                                                                          \
        namespace Exceptions                                                   \
        {

#define ASR_CORE_EXCEPTIONS_NS_END                                             \
    }                                                                          \
    using namespace ASR::Core::Exceptions;                                     \
    }                                                                          \
    ASR_NS_END

#endif // ASR_CORE_EXCEPTION_CONFIG_H
