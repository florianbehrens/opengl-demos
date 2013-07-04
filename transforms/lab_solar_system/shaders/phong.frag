#version 330

uniform struct LightInfo {
    vec4 position;
    vec3 intensity;
} light;

struct MaterialInfo
{
    vec3 ka;            // Ambient reflectivity
    vec3 kd;            // Diffuse reflectivity
    vec3 ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
};
uniform MaterialInfo material;

in vec3 position;
in vec3 normal;

layout (location = 0) out vec4 fragColor;


vec3 adsModel( const in vec3 pos, const in vec3 norm )
{
    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( light.position ) - pos );

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    vec3 v = normalize( -position.xyz );

    // Reflect the light beam using the normal at this fragment
    vec3 r = reflect( -s, norm );

    // Calculate the diffuse component
    vec3 diffuse = vec3( max( dot( s, norm ), 0.0 ) );

    // Calculate the specular component
    vec3 specular = vec3( 0.0 );
    if ( dot( s, norm ) > 0.0 )
        specular = vec3( pow( max( dot( r, v ), 0.0 ), material.shininess ) );

    // Combine the ambient, diffuse and specular contributions
    return light.intensity * ( material.ka + material.kd * diffuse + material.ks * specular );
}

void main()
{
    fragColor = vec4( adsModel( position, normalize( normal ) ), 1.0 );
}
