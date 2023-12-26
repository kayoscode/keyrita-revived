#include "Shaders/ShaderBase.h"

#include <fstream>
#include <iostream>

Shader::Shader() 
   : mShader(0)
{

}

Shader::~Shader() 
{
}

static bool LoadShaderi(const std::string& fileName, std::string& shaderString) 
{
   std::ifstream file;
   file.open(fileName);

   std::string output;
   std::string line;

   if (file.is_open()) 
   {
      while (file.good()) 
      {
         getline(file, line);
         shaderString.append(line + "\n");
      }
   }
   else 
   {
      return false;
   }

   return true;
}

/**
 * Attach shaders, but in this case, add future compatibility for geometry shader if needed
 * */
static bool AttachShaders(GLuint mShaderProgram, GLuint mVertexShader, GLuint mFragmentShader, GLuint geometryShader = 0) 
{
   if (mShaderProgram > 0)
   {
      if (mVertexShader > 0) 
      {
         glAttachShader(mShaderProgram, mVertexShader);
      }

      if (mFragmentShader > 0) 
      {
         glAttachShader(mShaderProgram, mFragmentShader);
      }

      if (geometryShader > 0) 
      {
         glAttachShader(mShaderProgram, geometryShader);
      }
   }
   else
   {
      return false;
   }

   return true;
}

static bool CheckShaderError(GLuint mShader, GLuint flag, bool isProgram, std::string& errorMessage)
{
   GLint success = 0;
   GLchar error[1024];

   if (isProgram) 
   {
      glGetProgramiv(mShader, flag, &success);
   }
   else
   {
      glGetShaderiv(mShader, flag, &success);
   }

   if (success == GL_FALSE)
   {
      if (isProgram) 
      {
         glGetProgramInfoLog(mShader, sizeof(error), NULL, error);
      }
      else
      {
         glGetShaderInfoLog(mShader, sizeof(error), NULL, error);
      }

      errorMessage = error;
      return false;
   }

   return true;
}

bool Shader::LoadShader(const std::string& shaderPath, int type, std::string& errorMessage)
{
   std::string text;

   if (LoadShaderi(shaderPath, text)) 
   {
      mShader = glCreateShader((GLuint)type);

      if (mShader == 0)
      {
         errorMessage = "Failed to create shader with type: " + (int)type;
         return false;
      }

      const GLchar* p[1];
      p[0] = text.c_str();
      GLint lengths[1];
      lengths[0] = (GLint)text.length();

      glShaderSource(mShader, 1, p, lengths);
      glCompileShader(mShader);

      std::string error;
      if (!CheckShaderError(mShader, GL_COMPILE_STATUS, false, error))
      {
         errorMessage = "Error compiling shader: " + shaderPath + ":\n" + error;
         return false;
      }
   }
   else 
   {
      errorMessage = "Could not open file: " + shaderPath;
      return false;
   }

   return true;
}

static bool LinkShaderProgram(GLuint mShaderProgram, std::string& error)
{
   glLinkProgram(mShaderProgram);

   if (!CheckShaderError(mShaderProgram, GL_LINK_STATUS, true, error))
   {
      return false;
   }

   glValidateProgram(mShaderProgram);

   if (!CheckShaderError(mShaderProgram, GL_LINK_STATUS, true, error)) 
   {
      return false;
   }

   return true;
}

ShaderBase::ShaderBase(const std::vector<std::string>& mAttributes)
   :mVertexShader(), mFragmentShader(), mShaderProgram((GLuint)0),
   mAttributes()
{
   for (int i = 0; i < mAttributes.size(); ++i) 
   {
      this->mAttributes[mAttributes[i]] = i;
   }
}

ShaderBase::~ShaderBase()
{
   if (mShaderProgram != 0)
   {
      glDetachShader(mShaderProgram, mVertexShader.GetShader());
      glDetachShader(mShaderProgram, mFragmentShader.GetShader());
   }

   if (mVertexShader.GetShader() != 0)
   {
      glDeleteShader(mVertexShader.GetShader());
   }

   if (mFragmentShader.GetShader() != 0)
   {
      glDeleteShader(mFragmentShader.GetShader());
   }

   if (mShaderProgram != 0)
   {
      glDeleteProgram(mShaderProgram);
   }
}

bool ShaderBase::LoadShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) 
{
   constexpr int maxErrorLen = 2048 * 2;
   GLchar errorLog[maxErrorLen];
   bool loadError = false;
   std::string currentError;

   //create the shader program
   this->mShaderProgram = glCreateProgram();

   //load each shader from a file
   if (!mVertexShader.LoadShader(vertexShaderPath, GL_VERTEX_SHADER, currentError)) 
   {
      loadError = true;
   }

   //load fragment shader from file
   currentError = "";
   if (!mFragmentShader.LoadShader(fragmentShaderPath, GL_FRAGMENT_SHADER, currentError)) 
   {
      loadError = true;
   }

   //link the shaders in a shader program
   if (!loadError) 
   {
      if (!AttachShaders(mShaderProgram, mVertexShader.GetShader(), mFragmentShader.GetShader())) 
      {
         currentError = "Failed to attach shaders\n";
         loadError = true;
      }

      //bind each attribute location to the correct name
      for (std::map<std::string, int>::iterator i = mAttributes.begin(); i != mAttributes.end(); ++i) 
      {
         glBindAttribLocation(this->mShaderProgram, (GLuint)i->second, i->first.c_str());
      }

      //create the shader program
      currentError = "";
      if (!LinkShaderProgram(this->mShaderProgram, currentError))
      {
         loadError = true;
      }

      //load uniforms
      SetUniformLocations();
   }

   if (loadError)
   {
      std::cout << "Error loading shader program: \n" << currentError << "\n";
   }

   return !loadError;
}
