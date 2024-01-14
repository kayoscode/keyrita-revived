#pragma once

namespace wgui
{
enum class eOsType
{
   Windows,
   Linux,
   // Unsupported
   MacOS
};

class OperatingSystem
{
 public:
   static constexpr eOsType GetOperatingSystem()
   {
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(__WIN32)
#define OS_WINDOWS
      return eOsType::Windows;
#endif

#if defined(__linux__)
#define OS_LINUX
      return eOsType::Linux;
#endif

#if defined(__APPLE__) || defined(__MACH__)
#define OS_MAC
#error MacOS untested and unsupported;
#endif
      return eOsType::MacOS;
   }
};
} // namespace wgui