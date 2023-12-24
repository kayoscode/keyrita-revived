#pragma once

#include "GL/glew.h"
#include "GLMath.h"

#include <string>
#include <map>
#include <vector>
#include <cassert>

/**
 * Class for dealing with shaders
 * This shader baseclass is responsible for loading shader data,
 * linking vertex and fragment shaders, and getting/setting uniform/attribute data
 * @author Bryce Young
 * */
class Shader 
{
public:
   Shader();
   ~Shader();

   bool LoadShader(const std::string& shaderPath, int type, std::string& errorMessage);

   GLuint GetShader() const
   {
      return mShader;
   }

protected:
   GLuint mShader;

private:
};

/**
 * A class that holds a linked vertex and fragment shader
 * @author Bryce Young 12/23/2023
 * */
class ShaderBase 
{
public:
   /// <summary>
   /// Standard constructor.
   /// </summary>
   /// <param name="mAttributes"></param>
   ShaderBase(const std::vector<std::string>& mAttributes);

   /// <summary>
   /// Destroy shaders, clean up memory.
   /// </summary>
   ~ShaderBase();

   /**
    * Prepares the shader for use
    * */
   static void Bind(GLuint shaderProgram)
   {
      glUseProgram(shaderProgram);
   }

   static void Unbind()
   {
      glUseProgram(0);
   }

   virtual void SetUniformLocations() = 0;
   virtual void LoadShader() = 0;

   GLuint GetVertexShader() const { return mVertexShader.GetShader(); }
   GLuint GetFragmentShader() const { return mFragmentShader.GetShader(); }
   GLuint GetShaderProgram() const { return mShaderProgram; }

   GLuint GetAttribLocation(const std::string& attribName)
   {
      if (mAttributes.find(attribName) != mAttributes.end())
      {
         return mAttributes[attribName];
      }

      assert(false);
   }

protected:
   /// <summary>
   /// Loads the vertex and fragment shaders into this object.
   /// </summary>
   /// <param name="vertexShaderPath"></param>
   /// <param name="fragmentShaderPath"></param>
   /// <returns></returns>
   bool LoadShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

   /// <summary>
   /// Functions to load and set uniforms 
   /// </summary>
   /// <param name="location"></param>
   /// <param name="value"></param>
   static inline void LoadUniformf(unsigned int location, float value)
   {
      glUniform1f(location, value);
   }

   static inline void LoadUniformVec2f(unsigned int location, const Vector2f& value)
   {
      glUniform2f(location, value.x, value.y);
   }

   static inline void LoadUniformVec3f(unsigned int location, const Vector3f& value)
   {
      glUniform3f(location, value.x, value.y, value.z);
   }

   static inline void LoadUniformVec4f(unsigned int location, const Vector4f& value)
   {
      glUniform4f(location, value.x, value.y, value.z, value.w);
   }

   // Doubles
   static inline void LoadUniformd(unsigned int location, double value)
   {
      glUniform1d(location, value);
   }

   static inline void LoadUniformVec2d(unsigned int location, const Vector2d& value)
   {
      glUniform2d(location, value.x, value.y);
   }

   static inline void LoadUniformVec3d(unsigned int location, const Vector3d& value)
   {
      glUniform3d(location, value.x, value.y, value.z);
   }

   static inline void LoadUniformVec4d(unsigned int location, const Vector4d& value)
   {
      glUniform4d(location, value.x, value.y, value.z, value.w);
   }

   // Integers
   static inline void LoadUniformi(unsigned int location, int value)
   {
      glUniform1i(location, value);
   }

   static inline void LoadUniformVec2i(unsigned int location, const Vector2i& value)
   {
      glUniform2i(location, value.x, value.y);
   }

   static inline void LoadUniformVec3i(unsigned int location, const Vector3i& value)
   {
      glUniform3i(location, value.x, value.y, value.z);
   }

   static inline void LoadUniformVec4i(unsigned int location, const Vector4i& value)
   {
      glUniform4i(location, value.x, value.y, value.z, value.w);
   }

   // Array
   static inline void LoadUniformArrayf(unsigned int location, float* values, int count)
   {
      glUniform1fv(location, count, values);
   }

   static inline void LoadUniformArrayd(unsigned int location, double* values, int count)
   {
      glUniform1dv(location, count, values);
   }

   static inline void LoadUniformArrayi(unsigned int location, int* values, int count)
   {
      glUniform1iv(location, count, values);
   }

   // Matrix22
   static inline void LoadMatrix22f(unsigned int location, const Matrix22f& value)
   {
      glUniformMatrix2fv(location, 1, false, &value.data[0][0]);
   }

   static inline void LoadMatrix22d(unsigned int location, const Matrix22d& value)
   {
      glUniformMatrix2dv(location, 1, false, &value.data[0][0]);
   }

   // Matrix33
   static inline void LoadMatrix33f(unsigned int location, const Matrix33f& value)
   {
      glUniformMatrix3fv(location, 1, false, &value.data[0][0]);
   }

   static inline void LoadMatrix33d(unsigned int location, const Matrix33d& value)
   {
      glUniformMatrix3dv(location, 1, false, &value.data[0][0]);
   }

   // Matrix44
   static inline void LoadMatrix44f(unsigned int location, const Matrix44f& value)
   {
      glUniformMatrix4fv(location, 1, false, &value.data[0][0]);
   }

   static inline void LoadMatrix44d(unsigned int location, const Matrix44d& value)
   {
      glUniformMatrix4dv(location, 1, false, &value.data[0][0]);
   }

   /// <summary>
   /// Returns the location of an attribute.
   /// Do not call this every frame since it has a little overhead.
   /// </summary>
   /// <param name="name"></param>
   /// <returns></returns>
   int GetAttributeLocation(const std::string& name)
   {
      std::map<std::string, int>::iterator i = mAttributes.find(name);

      if (i != mAttributes.end()) 
      {
         return i->second;
      }

      return -1;
   }

   /// <summary>
   /// Returns the index location of a uniform by name.
   /// </summary>
   /// <param name="name"></param>
   /// <returns></returns>
   int GetUniformLocation(const std::string& name)
   {
      GLuint ret;
      ret = glGetUniformLocation(this->mShaderProgram, name.c_str());
      return (int)ret;
   }

   Shader mVertexShader;
   Shader mFragmentShader;
   GLuint mShaderProgram;
   std::map<std::string, int> mAttributes;
};