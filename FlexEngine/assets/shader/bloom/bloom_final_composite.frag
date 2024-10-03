#version 460 core

//Third part of bloom -> Combining HDR Color with Bloom Color (Add)

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

void main() {
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    FragColor = vec4(hdrColor + bloomColor, 1.0);  // Additive blending
}
