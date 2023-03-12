#version 330 core
out vec4 FragColor;
in vec3 FragPos;

uniform sampler2D rectangular_map;

const vec2 inv_atan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v);

void main()
{
	vec2 uv = SampleSphericalMap(normalize(FragPos));
	vec3 color = texture(rectangular_map, uv).rgb;
	
	FragColor = vec4(color, 1.0);
}

vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= inv_atan;
	uv += 0.5;
	return uv;
}
