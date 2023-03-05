#version 450

layout(set = 2, binding = 0) uniform Light {
	vec3 viewPos;

	vec4 color;
	vec3 position;
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
	vec3 lightDir = normalize(light.position - v_FragPos);

	vec3 viewDir = normalize(light.viewPos - v_FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec4 specular = light.specularStrength * spec * light.color;  

	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = diff * light.color;

	vec4 ambient = light.ambientStrength * light.color;

	o_Color = (ambient + diffuse + specular) * texture( u_Texture, v_Texcoord );
}