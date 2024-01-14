#include "PlatformModules.h"

#ifdef OS_WINDOWS

#pragma region DLL hooks

#include "MinHook.h"
#include <Windows.h>
#include <psapi.h>

typedef HMODULE(WINAPI *LOADLIBRARYA)(LPCSTR libraryName);
typedef HMODULE(WINAPI *LOADLIBRARYW)(LPCWSTR libraryName);

// Pointer for calling original LoadLibraryA.
LOADLIBRARYA FPLoadLibraryA = NULL;
LOADLIBRARYW FPLoadLibraryW = NULL;

// Detour function which overrides LoadLibraryA.
HMODULE WINAPI DetourLoadLibraryA(LPCSTR libraryName)
{
   std::string libName(libraryName);
   if (libName.find("ProductInfo.dll") != std::string::npos)
   {
      return nullptr;
   }

   HMODULE result = FPLoadLibraryA(libraryName);
   return result;
}

HMODULE WINAPI DetourLoadLibraryW(LPCWSTR libraryName)
{
   std::wstring libName(libraryName);
   if (libName.find(L"ProductInfo.dll") != std::wstring::npos)
   {
      return nullptr;
   }

   return FPLoadLibraryW(libraryName);
}

template <typename T>
inline MH_STATUS MH_CreateHookApiEx(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T **ppOriginal)
{
   return MH_CreateHookApi(pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID *>(ppOriginal));
}

bool SetupWinHooks()
{
   // Initialize MinHook.
   if (MH_Initialize() != MH_OK)
   {
      return false;
   }

   // Create a hook for load lib, in disabled state.
   if (MH_CreateHookApiEx(L"kernel32", "LoadLibraryA", &DetourLoadLibraryA, &FPLoadLibraryA) != MH_OK)
   {
      return false;
   }

   if (MH_CreateHookApiEx(L"kernel32", "LoadLibraryW", &DetourLoadLibraryW, &FPLoadLibraryW) != MH_OK)
   {
      return false;
   }

   if (MH_EnableHook(&LoadLibraryA) != MH_OK)
   {
      return false;
   }

   if (MH_EnableHook(&LoadLibraryW) != MH_OK)
   {
      return false;
   }

   return true;
}

bool PlatformWindows::Initialize()
{
   return SetupWinHooks();
}

PlatformWindows::~PlatformWindows()
{
   MH_Uninitialize();
}

#pragma endregion

#else

// None of this should ever execute if things are running properly.
bool PlatformWindows::Initialize()
{
   assert(false);
   return false;
}

PlatformWindows::~PlatformWindows()
{
   assert(false);
}

#endif
