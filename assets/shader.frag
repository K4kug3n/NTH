#version 450

layout(set = 2, binding = 0) uniform LightGpuObject {
  vec4 ambientColor;
  float ambientStrength;
} light;

layout(set = 3, binding = 0) uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_Texcoord;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec3 v_FragPos;

layout(location = 0) out vec4 o_Color;

void main() {
  vec3 lightPos = vec3(0.5, 0.5, 0.5);
  vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

  vec3 norm = normalize(v_Normal);
  vec3 lightDir = normalize(lightPos - v_FragPos);  

  float diff = max(dot(norm, lightDir), 0.0);
  vec4 diffuse = diff * lightColor;

  vec4 ambient = light.ambientStrength * light.ambientColor;

  o_Color = (ambient + diffuse) * texture( u_Texture, v_Texcoord );
}