#ifndef ASR_CORE_FOREIGNINTERFACEHOST_CPPINTERFACE_H
#define ASR_CORE_FOREIGNINTERFACEHOST_CPPINTERFACE_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/IAsrBase.h>
#include "Plugin.h"
#include <boost/dll/shared_library.hpp>

#define ASR_NS_CPPHOST_BEGIN                                                   \
    namespace CppHost                                                          \
    {

#define ASR_NS_CPPHOST_END }

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

class CppPlugin
{
    boost::dll::shared_library plugin_;
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_CPPINTERFACE_H
