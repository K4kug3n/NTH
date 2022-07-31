#version 460

layout(set=0, binding=1) uniform UniformBufferObject {
  mat4 view;
  mat4 proj;
} ubo;

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Color;
layout(location = 2) in vec2 i_Texcoord;

out gl_PerVertex
{
  vec4 gl_Position;
};

struct ObjectData{
  mat4 model;
};

//all object matrices
layout(std140,set = 1, binding = 0) readonly buffer ObjectBuffer{
  ObjectData objects[];
} objectBuffer;

layout(location = 0) out vec2 v_Texcoord;
layout(location = 1) out vec3 v_Color;

void main() {
  mat4 modelMatrix = objectBuffer.objects[gl_BaseInstance].model;
  gl_Position = ubo.proj * ubo.view * modelMatrix * vec4(i_Position, 1.0);
  v_Color = i_Color;
  v_Texcoord = i_Texcoord;
}
