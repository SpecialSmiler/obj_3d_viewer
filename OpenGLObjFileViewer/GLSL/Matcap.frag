#version 330 core

out vec4 FragColor;
in vec3 Normal;
uniform sampler2D  matcap;

void main()
{
    vec3 norm = normalize(Normal);
    vec2 texcoord = norm.xy * 0.5 + 0.5;
    vec3 color = texture(matcap, texcoord).rgb;
    FragColor = vec4(color, 1.0f);

}