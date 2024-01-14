#pragma once

#include "OperatingSystem.h"
#include <iostream>
#include <string>

class PlatformBase
{
 public:
   virtual bool Initialize() = 0;
   virtual ~PlatformBase() = default;
};

class PlatformWindows : public PlatformBase
{
 public:
   ~PlatformWindows() override;
   bool Initialize() override;
};

class PlatformLinux : public PlatformBase
{
 public:
   // Linux doesn't need anything special for now :D
   bool Initialize() override
   {
      return true;
   }
};
