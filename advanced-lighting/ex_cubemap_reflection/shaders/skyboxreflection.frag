#version 330

in vec3 reflectDir;

uniform samplerCube cubeMapTex;
uniform float reflectFactor;
uniform vec4 materialColor;

layout( location = 0 ) out vec4 fragColor;

void main()
{
    // Access the cube map texture
    vec4 cubeMapColor = texture( cubeMapTex, reflectDir );
    fragColor = mix( materialColor, cubeMapColor, reflectFactor );
}

