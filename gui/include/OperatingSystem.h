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

    static class OperatingSystem
    {
    public:
        static constexpr eOsType GetOperatingSystem()
        {
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(__WIN32)
            return eOsType::Windows;
#endif

#if defined(__linux__)
            return eOsType::Linux;
#endif

#if defined(__APPLE__) || defined(__MACH__)
#error MacOS untested and unsupported;
#endif
            return eOsType::MacOS;
        }
    };
}