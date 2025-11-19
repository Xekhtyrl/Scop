#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in int aTriID;
// layout (location = 3) in vec3 aTangent;
// layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
flat out int TriID;
// out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // World position of the vertex
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;

    // Normal in world space
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Texture coordinates
    TexCoords = aTexCoord;

	TriID = aTriID;

    // Compute TBN matrix (only used if you have tangents & bitangents)
    // vec3 T = normalize(mat3(model) * aTangent);
    // vec3 B = normalize(mat3(model) * aBitangent);
    // vec3 N = normalize(Normal);
    // TBN = mat3(T, B, N);

    // Final clip-space position
    gl_Position = projection * view * worldPos;
	// gl_Position = vec4(aPos, 1.0);
}