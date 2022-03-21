#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;
out vec3 ViewPos;
out vec2 TexCoords;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	Normal = mat3(transpose(inverse(model))) * aNormal;
	FragPos = vec3(model * vec4(aPos, 1.0));
	ViewPos = vec3(model * vec4(viewPos, 1.0));
	//LightPos = vec3(view * vec4(1.2f, 1.0f, 2.0f, 1.0));//lightPos is already in world space
	TexCoords = aTexCoords;

}
