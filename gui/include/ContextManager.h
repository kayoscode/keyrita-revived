#pragma once

#include <functional>
#include <memory>

#include "include_nuk.h"

namespace wgui
{
   /// <summary>
   /// Class to help manage global libraries.
   /// Glfw and glew both have global states that need to be managed. 
   /// These should be handled via constructor and destructor to 
   /// ensure that resources get allocated and destroyed correctly. 
   /// </summary>
   class GlfwContextManager
   {
   public:
      ~GlfwContextManager();

      bool InitContext();

   private:
      static constexpr std::tuple<int, int> GlfwVersion = std::make_tuple(3, 3);
   };

   class GlewContextManager
   {
   public:
      bool InitContext();
   };

   class NuklearGlfwContextManager
   {
   public:
      NuklearGlfwContextManager();
      ~NuklearGlfwContextManager();

      bool InitWindowContext();
      bool InitGlewContext();
      bool InitNkContext(GLFWwindow* window);
      inline nk_context const* GetContext() const { return mNkContext; }
      inline nk_context* GetContext() { return mNkContext; }

      inline nk_glfw const* GetGlfw() const { return mNkGlfw.get(); }
      inline nk_glfw* GetGlfw() { return mNkGlfw.get(); }

   private:
      GlfwContextManager mGlfwContext;
      GlewContextManager mGlewContext;

      nk_context* mNkContext;
      std::unique_ptr<nk_glfw> mNkGlfw;
   };
}
