#pragma once

#include "flx_api.h"

#include <glad/glad.h>

#include <string>

namespace FlexEngine
{
  namespace Asset
  {

    /// <summary>
    /// Helper class for building and compiling shaders using glad
    /// </summary>
    class __FLX_API Shader
    {
    public:
      Shader() = default;
      ~Shader();

      /// <summary>
      /// Destroy the shader program and its components
      /// </summary>
      void Destroy();

      /// <summary>
      /// Set the base path for shader files
      /// </summary>
      /// <param name="base_path">The base path for shader files "base/file/path"</param>
      Shader* SetBasePath(const std::string& base_path);

      /// <summary>
      /// Read and compile a vertex shader for linking
      /// </summary>
      /// <param name="path_to_vertex_shader">The path to the vertex shader file "path/to/vertex_shader"</param>
      Shader* CreateVertexShader(const std::string& path_to_vertex_shader);

      /// <summary>
      /// Read and compile a fragment shader for linking
      /// </summary>
      /// <param name="path_to_fragment_shader">The path to the fragment shader file "path/to/fragment_shader"</param>
      Shader* CreateFragmentShader(const std::string& path_to_fragment_shader);

      /// <summary>
      /// Link the vertex and fragment shaders into a shader program
      /// </summary>
      void Link();

      /// <summary>
      /// Use the shader program with glUseProgram
      /// </summary>
      void Use() const { glUseProgram(m_shader_program); }

      /// <returns>The shader program</returns>
      unsigned int Get() const { return m_shader_program; }

    private:
      std::string m_base_path{};
      unsigned int m_shader_program = 0;
      unsigned int m_vertex_shader = 0;
      unsigned int m_fragment_shader = 0;
    };

  }
}