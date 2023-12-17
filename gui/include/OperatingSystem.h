#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(__WIN32)
#define OS_WINDOWS
#endif

#if defined(__linux__)
#define OS_LINUX
#endif

#if defined(__APPLE__) || defined(__MACH__)
#define OS_MAC
#endif

namespace wgui
{
    static class OperatingSystem
    {

    };
}