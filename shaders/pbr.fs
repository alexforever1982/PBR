#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material
{
	sampler2D albedo;
	sampler2D normal;
	sampler2D metallic;
	sampler2D roughness;
	sampler2D ao;
};

struct Light
{
	vec3 position;
	vec3 color;
};

//IBL
uniform samplerCube irradiance_map;
uniform samplerCube prefilter_map;
uniform sampler2D brdfLUT;

// lights
uniform Light lights[4];

uniform Material material;
//uniform Light light;
uniform vec3 camera;

const float PI = 3.14159265359;

vec3 GetNormalFromMap();
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

void main()
{
	// material properties
	vec3 albedo = pow(texture(material.albedo, TexCoords).rgb, vec3(2.2));
	float metallic = texture(material.metallic, TexCoords).r;
	float roughness = texture(material.roughness, TexCoords).r;
	float ao = texture(material.ao, TexCoords).r;
	
	// light properties
	vec3 N = GetNormalFromMap();
	vec3 V = normalize(camera - FragPos);
	vec3 R = reflect(-V, N);
	
	// reflectance at normal incidence
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);
	
	// reflectance equation
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < 4; i++)
	{
		// light radiance
		vec3 L = normalize(lights[i].position - FragPos);
		vec3 H = normalize(V + L);
		float distance = length(lights[i].position - FragPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lights[i].color * attenuation;
		
		// cook-torrance brdf
		float NDF = DistributionGGX(N, H, roughness);   
		float G   = GeometrySmith(N, V, L, roughness);    
		vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);
		
		vec3 numerator    = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;
	
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;
		
		float NdotL = max(dot(N, L), 0.0); 
		
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}
	
	// ambient light (IBL)
	
	vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
	
	// IBL diffuse part
	vec3 irradiance = texture(irradiance_map, N).rgb;
	vec3 diffuse    = irradiance * albedo;
	
	// IBL specular part
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(prefilter_map, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
	
	vec3 ambient = (kD * diffuse + specular) * ao;
	
	vec3 color = ambient + Lo;
	
	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0/2.2)); 
	
	FragColor = vec4(color , 1.0);
}

vec3 GetNormalFromMap()
{
	vec3 tangentNormal = texture(material.normal, TexCoords).xyz * 2.0 - 1.0;
	
	vec3 Q1  = dFdx(FragPos);
	vec3 Q2  = dFdy(FragPos);
	vec2 st1 = dFdx(TexCoords);
	vec2 st2 = dFdy(TexCoords);
	
	vec3 N   =  normalize(Normal);
	vec3 T   =  normalize(Q1*st2.t - Q2*st1.t);
	vec3 B   = -normalize(cross(N, T));
	mat3 TBN =  mat3(T, B, N);
	
	return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	
	float nom   = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	
	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
