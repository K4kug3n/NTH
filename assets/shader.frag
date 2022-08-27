#version 450

layout(set = 2, binding = 0) uniform LightGpuObject {
	vec3 viewPos;

	vec4 lightColor;
	vec3 lightPos;
	float ambientStrength;
	float specularStrength;
} light;

layout(set = 3, binding = 0) uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_Texcoord;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec3 v_FragPos;

layout(location = 0) out vec4 o_Color;

void main() {
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(light.lightPos - v_FragPos);

	vec3 viewDir = normalize(light.viewPos - v_FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec4 specular = light.specularStrength * spec * light.lightColor;  

	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = diff * light.lightColor;

	vec4 ambient = light.ambientStrength * light.lightColor;

	o_Color = (ambient + diffuse + specular) * texture( u_Texture, v_Texcoord );
}