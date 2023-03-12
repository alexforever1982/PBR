#version 330 core
out vec4 FragColor;
in vec3 FragPos;

uniform samplerCube environment_map;

void main()
{
	vec3 color = textureLod(environment_map, FragPos, 0.0).rgb;
	
	// HDR tonemap and gamma correct
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2)); 
	
	FragColor = vec4(color, 1.0);
}
