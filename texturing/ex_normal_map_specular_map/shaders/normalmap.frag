#version 330

in vec3 lightDir;
in vec3 viewDir;
in vec2 texCoord;

uniform sampler2D texture0; // Diffuse
uniform sampler2D texture1; // Specular
uniform sampler2D texture2; // Normal

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

void adsModel( const in vec3 norm, const in vec3 diffuseReflect, out vec3 ambientAndDiff, out vec3 spec )
{
    // Reflection of light direction about normal
    vec3 r = reflect( -lightDir, norm );

    // Calculate the ambient contribution
    vec3 ambient = light.intensity * material.Ka;

    // Calculate the diffuse contribution
    float sDotN = max( dot( lightDir, norm ), 0.0 );
    vec3 diffuse = light.intensity * diffuseReflect * sDotN;

    // Sum the ambient and diffuse contributions
    ambientAndDiff = ambient + diffuse;

    // Calculate the specular highlight contribution
    spec = vec3( 0.0 );
    if ( sDotN > 0.0 )
        spec = light.intensity * pow( max( dot( r, viewDir ), 0.0 ), material.shininess );
}

void main()
{
    // Sample the textures at the interpolated texCoords
    vec4 diffuseTextureColor = texture( texture0, texCoord );
    vec4 specularTextureColor = texture( texture1, texCoord );
    vec4 normal = 2.0 * texture( texture2, texCoord ) - vec4( 1.0 );

    // Calculate the lighting model, keeping the specular component separate
    vec3 ambientAndDiff, spec;
    adsModel( normalize( normal.xyz ), diffuseTextureColor.xyz, ambientAndDiff, spec );

    // Multiply specular factor by specular texture sample
    vec3 specularColor = spec * specularTextureColor.rgb;

    // Combine spec with ambient+diffuse for final fragment color
    fragColor = vec4( ambientAndDiff + specularColor, 1.0 );
}
