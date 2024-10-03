#version 460 core

//Second part of bloom -> Gaussian Blur (Horizontal)

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D image;

void main() {
    vec2 tex_offset = 1.0 / textureSize(image, 0);  // Texture size
    vec3 result = texture(image, TexCoords).rgb * 0.227027;

    for (int i = 1; i < 5; ++i) {
        result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * 0.1945946;
        result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * 0.1945946;
    }

    FragColor = vec4(result, 1.0);
}