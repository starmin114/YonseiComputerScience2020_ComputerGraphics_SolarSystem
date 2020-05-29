#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLights[2];
uniform PointLight pointLight;
uniform float shininess;
uniform sampler2D textureDiffuseMap;
uniform int type;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // directional lighting
    vec3 result = vec3(0.0f);
	for(int i =0; i<2; i++)
		result += CalcDirLight(dirLights[i], norm, viewDir);
    // point lights
    result += CalcPointLight(pointLight, norm, FragPos, viewDir);
    
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(textureDiffuseMap, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(textureDiffuseMap, TexCoords));
    return (ambient + diffuse);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{	if(type != 0){
		vec3 lightDir = normalize(light.position - fragPos);
			// diffuse shading
			float diff = max(dot(normal, lightDir), 0.0);
			// specular shading
			vec3 reflectDir = reflect(-lightDir, normal);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
			// attenuation
			float distance = length(light.position - fragPos);
			float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
			// combine results
			vec3 ambient = light.ambient * vec3(texture(textureDiffuseMap, TexCoords));
			vec3 diffuse = light.diffuse * diff * vec3(texture(textureDiffuseMap, TexCoords));
			vec3 specular = light.specular * spec;
			ambient *= attenuation;
			diffuse *= attenuation;
			specular *= attenuation;
			return (ambient + diffuse + specular);
	}
    return vec3(texture(textureDiffuseMap, TexCoords)) * 0.2;
}

