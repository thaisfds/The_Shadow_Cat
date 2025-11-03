// Request GLSL 3.3
#version 330

// This corresponds to the output color to the color buffer
out vec4 outColor;
uniform vec3 uColor;

// This is used for the texture sampling
uniform sampler2D uTexture;

// This is used for texture blending
uniform float uTextureFactor;

// Tex coord input from vertex shader
in vec2 fragTexCoord;

void main()
{
    vec4 texColor = texture(uTexture, fragTexCoord);
    vec4 solidColor = vec4(uColor, 1.0);
    outColor = mix(solidColor, texColor, uTextureFactor);
}
