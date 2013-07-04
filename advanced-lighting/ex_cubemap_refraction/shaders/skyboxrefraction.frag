#version 330

uniform samplerCube cubeMapTex;
uniform bool drawSkyBox;

struct MaterialInfo {
    float eta;              // Ratio of indices of refraction
    float reflectionFactor; // Fraction of light reflected
};
uniform MaterialInfo material;

in vec3 reflectDir;
in vec3 refractDir;

layout( location = 0 ) out vec4 fragColor;

void main()
{
    // Access the cube map texture using the reflection and refraction
    // direction vectors.
    vec4 reflectColor = texture( cubeMapTex, reflectDir );
    vec4 refractColor = texture( cubeMapTex, refractDir );

    if ( drawSkyBox )
        fragColor = reflectColor;
    else
        fragColor = mix( refractColor, reflectColor, material.reflectionFactor );
}

