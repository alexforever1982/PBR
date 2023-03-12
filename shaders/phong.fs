#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shininess;
};

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 camera;

vec3 getNormalFromMap();

void main()
{
    // ambient
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	
	// diffuse
	vec3 norm = normalize(Normal);
	norm = getNormalFromMap();
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	
	// specular
	vec3 viewDir = normalize(camera - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
	
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(material.normal, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
