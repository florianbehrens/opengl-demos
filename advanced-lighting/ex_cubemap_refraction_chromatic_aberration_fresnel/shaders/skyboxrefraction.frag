#version 330

uniform samplerCube cubeMapTex;
uniform bool drawSkyBox;

struct MaterialInfo {
    vec3 eta;               // Ratio of indices of refraction
    float reflectionFactor; // Fraction of light reflected
};
uniform MaterialInfo material;

in vec3 reflectDir;
in vec3 refractDirRed;
in vec3 refractDirGreen;
in vec3 refractDirBlue;
in float fresnelFactor;

layout( location = 0 ) out vec4 fragColor;

void main()
{
    // Access the cube map texture using the reflection and refraction
    // direction vectors.
    vec4 reflectColor = texture( cubeMapTex, reflectDir );
    vec4 refractColorRed = texture( cubeMapTex, refractDirRed );
    vec4 refractColorGreen = texture( cubeMapTex, refractDirGreen );
    vec4 refractColorBlue = texture( cubeMapTex, refractDirBlue );

    if ( drawSkyBox )
    {
        fragColor = reflectColor;
    }
    else
    {
        vec4 refractColor = vec4( refractColorRed.r, refractColorGreen.g, refractColorBlue.b, 1.0 );
        fragColor = mix( refractColor, reflectColor, fresnelFactor );
    }
}

