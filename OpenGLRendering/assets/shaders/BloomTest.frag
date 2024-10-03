#version 460 core

out vec4 fragment_color;

in vec2 tex_coord;

// texture
uniform sampler2D u_texture;
uniform vec3 u_color;
uniform vec3 u_color_add;
uniform vec3 u_color_multiply;
uniform bool u_use_texture;

void main()
{
  vec4 diffuse = vec4(1.0, 0.0, 1.0, 1.0);
  if (u_use_texture)
  {
    diffuse = texture(u_texture, tex_coord);
  }
  else
  {
    diffuse = vec4(u_color, 1.0);
  }
  fragment_color = diffuse;
  //diffuse = diffuse * u_color_multiply + u_color_add;
  //fragment_color = vec4(diffuse, 1.0);
}
