#version 120

varying vec3 lightDir;
varying vec3 viewDir;
varying vec2 texCoord;

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
    vec4 diffuseTextureColor = texture2D( texture0, texCoord );

    // We have to scale and bias the normal data
    vec4 normal = 2.0 * texture2D( texture1, texCoord ) - vec4( 1.0 );

    // Calculate the lighting model, keeping the specular component separate
    vec3 ambientAndDiff, spec;
    adsModel( normalize( normal.xyz ), diffuseTextureColor.xyz, ambientAndDiff, spec );

    // Add specular contribution for final fragment color
    gl_FragColor = vec4( ambientAndDiff + spec, 1.0 );
}
