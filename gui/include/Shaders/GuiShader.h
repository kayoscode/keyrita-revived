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
      void SetUniformLocations() override
      {
         DefaultGuiShader::SetUniformLocations();
         mLocationMousePos = GetUniformLocation("MousePos");
      }

      void LoadMousePos(int posX, int posY)
      {
         LoadUniformVec2f(mLocationMousePos, Vector2f(posX, posY));
      }

   protected:
      int mLocationMousePos = 0;
   };
}
