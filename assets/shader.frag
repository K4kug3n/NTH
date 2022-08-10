#version 450

layout(set=2, binding=0) uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_Texcoord;
layout(location = 1) in vec3 v_Color;

layout(location = 0) out vec4 o_Color;

void main() {
  o_Color = texture( u_Texture, v_Texcoord );
  //o_Color = vec4(v_Color.x, v_Color.y, v_Color.z, 1.0);
  //o_Color = vec4(v_Color * texture(u_Texture, v_Texcoord).rgb, 1.0);
}