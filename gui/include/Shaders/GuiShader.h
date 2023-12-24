#pragma once

#include "Shaders/ShaderBase.h"
#include "Vector2.h"
#include "GLMath.h"

namespace wgui
{
   class DefaultGuiShader : public ShaderBase {
   public:
      DefaultGuiShader()
         : ShaderBase({ "Position",
                        "TexCoord",
                        "Color" })
      {
      }

      void LoadShader() override
      {
         LoadShaders("./res/gui/shaders/DefaultShader.vert",
                     "./res/gui/shaders/DefaultShader.frag");
      }

      void SetUniformLocations() override
      {
         mLocationTexture = GetUniformLocation("Texture");
         mLocationProjMatrix = GetUniformLocation("ProjectionMatrix");
      }

      void LoadTexture(const int textureIndex)
      {
         LoadUniformi(mLocationTexture, textureIndex);
      }

      void LoadProjection(const Matrix44f& data)
      {
         LoadMatrix44f(mLocationProjMatrix, data);
      }

   protected:
      int mLocationTexture = 0;
      int mLocationProjMatrix = 0;
   };

   class HighlightGuiShader : public DefaultGuiShader
   {
   public:
      HighlightGuiShader()
      {
      }

      void LoadShader() override
      {
         LoadShaders("./res/gui/shaders/DefaultShader.vert",
                     "./res/gui/shaders/HighlightEffectShader.frag");
      }

      void SetUniformLocations() override
      {
         DefaultGuiShader::SetUniformLocations();
         mLocationMousePos = GetUniformLocation("MousePos");
         mLocationWindowSize = GetUniformLocation("WindowDims");
      }

      void LoadMousePos(int posX, int posY)
      {
         LoadUniformVec2f(mLocationMousePos, Vector2f(posX, posY));
      }

      void LoadWindowSize(int sizeX, int sizeY)
      {
         LoadUniformVec2f(mLocationWindowSize, Vector2f(sizeX, sizeY));
      }

   protected:
      int mLocationMousePos = 0;
      int mLocationWindowSize = 0;
   };
}
