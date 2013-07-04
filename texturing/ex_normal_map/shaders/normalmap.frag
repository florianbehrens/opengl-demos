#version 330

in vec3 lightDir;
in vec3 viewDir;
in vec2 texCoord;

uniform sampler2D texture0; // Diffuse
uniform sampler2D texture1; // Normal

struct LightInfo
{
    vec4 position;  // Light position in eye coords.
    vec3 intensity; // A,D,S intensity
};
uniform LightInfo light;

struct MaterialInfo
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
};
uniform MaterialInfo material;

layout( location = 0 ) out vec4 fragColor;

vec3 ambientDiffuseModel( const in vec3 norm, const in vec3 diffuseReflect )
{
    // Reflection of light direction about normal
    vec3 r = reflect( -lightDir, norm );

    // Calculate the ambient contribution
    vec3 ambient = light.intensity * material.Ka;

    // Calculate the diffuse contribution
    float sDotN = max( dot( lightDir, norm ), 0.0 );
    vec3 diffuse = light.intensity * diffuseReflect * sDotN;

    // Sum the ambient and diffuse contributions
    return ambient + diffuse;
}

void main()
{
    // Sample the textures at the interpolated texture coords
    vec4 diffuseTextureColor = texture( texture0, texCoord );

    // We have to scale and bias the normal data
    vec4 normal = 2.0 * texture( texture1, texCoord ) - vec4( 1.0 );

    // Calculate the ambient and diffuse lighting model
    vec3 ambientAndDiffuse = ambientDiffuseModel( normal.xyz, diffuseTextureColor.xyz );

    // Output final fragment color
    fragColor = vec4( ambientAndDiffuse, 1.0 );
}
