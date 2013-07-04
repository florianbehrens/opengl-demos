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

uniform struct FogInfo {
    vec3 color;
    float minDistance;
    float maxDistance;
} fog;

in vec3 position;
in vec3 normal;

layout (location = 0) out vec4 fragColor;


vec3 adsModel( const in vec3 pos, const in vec3 norm )
{
    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( light.position ) - pos );

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    vec3 v = normalize( -pos );

    // Reflect the light beam using the normal at this fragment
    vec3 r = reflect( -s, norm );

    // Calculate the diffuse component
    float diffuse = max( dot( s, norm ), 0.0 );

    // Calculate the specular component
    float specular = 0.0;
    if ( dot( s, norm ) > 0.0 )
        specular = pow( max( dot( r, v ), 0.0 ), material.shininess );

    // Combine the ambient, diffuse and specular contributions
    return light.intensity * ( material.ka + material.kd * diffuse + material.ks * specular );
}

void main()
{
    vec3 adsColor = adsModel( position, normalize( normal ) );

    float d = abs( position.z );
    float fogFactor = ( fog.maxDistance - d ) / ( fog.maxDistance - fog.minDistance );
    fogFactor = clamp( fogFactor, 0.0, 1.0 );

    vec3 color = mix( fog.color, adsColor, fogFactor );
    fragColor = vec4( color, 1.0 );
}
