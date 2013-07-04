#version 330

in vec3 direction;

uniform samplerCube cubeMapTex;

layout( location = 0 ) out vec4 fragColor;

void main()
{
    // Access the cube map texture
    fragColor = texture( cubeMapTex, direction );
}

