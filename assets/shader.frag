#version 450

layout(set=2, binding=0) uniform sampler2D u_Texture;

layout(set=3, binding=0) uniform LightGpuObject {
  vec4 color;
} light;

layout(location = 0) in vec2 v_Texcoord;
layout(location = 1) in vec3 v_Color;

layout(location = 0) out vec4 o_Color;

void main() {
  float ambientStrength = 1.0;
  vec4 ambient = ambientStrength * light.color;


  o_Color = ambient * texture( u_Texture, v_Texcoord );
}