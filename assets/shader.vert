#version 450

layout(set=0, binding=1) uniform u_UniformBuffer {
  mat4 u_ProjectionMatrix;
};

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec3 i_Color;
layout(location = 2) in vec2 i_Texcoord;

out gl_PerVertex
{
  vec4 gl_Position;
};

//layout(location = 0) out vec2 v_Texcoord;
layout(location = 0) out vec3 v_Color;

void main() {
  gl_Position = u_ProjectionMatrix * i_Position;
  v_Color = i_Color;
}
