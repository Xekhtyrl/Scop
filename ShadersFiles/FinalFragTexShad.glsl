#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
//in mat3 TBN; // Tangent-Bitangent-Normal matrix for normal mapping

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

// Map usage toggles
uniform bool useDiffuseMap;
uniform bool useSpecularMap;
uniform bool useNormalMap;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normalMap;
    vec3 ambient;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
    float opacity;
};

uniform Material material;

out vec4 FragColor;

void main()
{
    // Base color (diffuse)
	vec3 albedo;
	if (useDiffuseMap)
		albedo = texture(material.diffuse, TexCoords).rgb;
	else
		albedo = material.diffuseColor;

    // Specular color
    vec3 specularColor = material.specularColor;
    if (useSpecularMap)
        specularColor *= texture(material.specular, TexCoords).rgb;

    // Normal map (if present)
    vec3 normal = normalize(Normal);
    if (useNormalMap) {
        vec3 tangentNormal = texture(material.normalMap, TexCoords).rgb * 2.0 - 1.0;
        normal = normalize(tangentNormal);
    }

    // Lighting vectors
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Ambient
    vec3 ambient = material.ambient * albedo;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedo;

    // Specular (Blinn–Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = specularColor * spec;

    // Combine
    vec3 color = (ambient + diffuse + specular) * lightColor;

    FragColor = vec4(color, material.opacity);
	// FragColor = texture(material.diffuse, TexCoords);
}
