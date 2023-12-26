#include <iostream>
#include "NuklearWindowRenderer.h"
#include "OperatingSystem.h"

#ifdef OS_WINDOWS

#include <Windows.h>
#include <psapi.h>

#undef CreateWindow

using namespace wgui;

#pragma region DLL hooks

#include "MinHook.h"

typedef HMODULE (WINAPI* LOADLIBRARYA)(LPCSTR libraryName);
typedef HMODULE (WINAPI* LOADLIBRARYW)(LPCWSTR libraryName);

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
inline MH_STATUS MH_CreateHookApiEx(
   LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T** ppOriginal)
{
   return MH_CreateHookApi(
      pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

void SetupWinHooks()
{
   // Initialize MinHook.
   if (MH_Initialize() != MH_OK)
   {
      exit(1);
   }

   // Create a hook for load lib, in disabled state.
   if (MH_CreateHookApiEx(L"kernel32", "LoadLibraryA", &DetourLoadLibraryA, &FPLoadLibraryA) != MH_OK)
   {
      exit(1);
   }

   if (MH_CreateHookApiEx(L"kernel32", "LoadLibraryW", &DetourLoadLibraryW, &FPLoadLibraryW) != MH_OK)
   {
      exit(1);
   }

   if (MH_EnableHook(&LoadLibraryA) != MH_OK)
   {
      exit(1);
   }

   if (MH_EnableHook(&LoadLibraryW) != MH_OK)
   {
      exit(1);
   }
}

#pragma endregion

#endif

int main()
{
#ifdef OS_WINDOWS
   SetupWinHooks();
#endif

   MainWindow mainWindow;
   mainWindow.CreateWindow("Keyrita", 1600, 1200, false, true, true, false);
   mainWindow.SetWindowSizeLimits(1200, 900);

   bool resiable = false;

   XmlLoadedWindowRenderer renderer;
   renderer.ConstructLayoutFromXmlFile("./res/gui/Keyrita.guix");

   mainWindow.SetRenderer(&renderer);

   Timer t;
   int frameCount = 0;
   while (!mainWindow.Closing())
   {
      mainWindow.Update();
      mainWindow.Render();

      if (t.milliseconds() >= 5000)
      {
         std::cout << "Fps: " << frameCount / 5 << "\n";
         frameCount = 0;
         t.reset();
      }

      frameCount++;
   }

   if (MH_Uninitialize() != MH_OK)
   {
      exit(1);
   }

   return 0;
}