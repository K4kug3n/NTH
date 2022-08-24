#version 450

layout(set=2, binding=0) uniform LightGpuObject {
  vec4 color;
  float ambientStrength;
} light;

layout(set=3, binding=0) uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_Texcoord;
layout(location = 1) in vec3 v_Color;

layout(location = 0) out vec4 o_Color;

void main() {
  vec4 ambient = light.ambientStrength * light.color;

  o_Color = ambient * texture( u_Texture, v_Texcoord );
}