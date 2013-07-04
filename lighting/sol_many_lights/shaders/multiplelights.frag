#version 330

uniform struct LightInfo {
    vec4 positionOrNormal; // positional light has w=1, directional has w=0
    vec3 intensity;
    float cutoffDistance;
} lights[5];

uniform struct SpotLightInfo {
    vec4 position;
    vec3 intensity;
    vec3 direction;
    float exponent;
    float cutoffAngle;
} spotlights[5];

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


uniform int numLights;
uniform int numSpotlights;

in vec3 position;
in vec3 normal;

layout (location = 0) out vec4 fragColor;

vec3 adsSpotLightModel( const in int spotIndex, const in vec3 pos, const in vec3 norm )
{
    // If the triangle is wound the wrong way, invert the normal so
    // that we get the desired lighting effect
    vec3 n = norm;
    if ( !gl_FrontFacing )
        n = -n;

    // Ambient contribution
    vec3 ambient = spotlights[spotIndex].intensity * material.ka;

    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( spotlights[spotIndex].position ) - pos );

    // Calculate angle of fragment from spot light axis
    float theta = acos( dot( -s, spotlights[spotIndex].direction ) );

    // Calculate cutoff angle in radians
    float thetaCutoff = radians( clamp( spotlights[spotIndex].cutoffAngle, 0.0, 90.0 ) );

    if ( theta > thetaCutoff )
        return vec3( 0.0, 0.0, 0.0);

    // Fragment is within cone of spotlight - full ADS with angular attenuation

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    vec3 v = normalize( -pos );

    // Reflect the light beam using the normal at this fragment
    vec3 r = reflect( -s, n );

    // Calculate the diffuse component
    float diffuse = max( dot( s, n ), 0.0 );

    // Calculate the specular component
    float specular = 0.0;
    if ( dot( s, n ) > 0.0 )
        specular = pow( max( dot( r, v ), 0.0 ), material.shininess );

    // Calculate angular attentuation factor
    float spotLightFactor = pow( dot( -s, spotlights[spotIndex].direction ), spotlights[spotIndex].exponent );

    // Combine the ambient, diffuse and specular contributions
    return  spotlights[spotIndex].intensity * spotLightFactor * ( material.kd * diffuse + material.ks * specular );
}

vec3 adsModel( const in int lightIndex, const in vec3 pos, const in vec3 norm )
{
    // If the triangle is wound the wrong way, invert the normal so
    // that we get the desired lighting effect
    vec3 n = norm;
    if ( !gl_FrontFacing )
        n = -n;

    // Calculate the vector from the light to the fragment
    vec4 posOrDir = lights[lightIndex].positionOrNormal ;

    // directional lights have w == 0, positional lights have w = 1
    vec3 s = normalize( ( posOrDir.w == 0.0) ? posOrDir.xyz :  vec3(posOrDir) - position );

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    vec3 v = normalize( -position.xyz );

    // Reflect the light beam using the normal at this fragment
    vec3 r = reflect( -s, n );

    // Calculate the diffuse component
    float diffuse = max( dot( s, n ), 0.0 );

    // Calculate the specular component
    float specular = 0.0;
    if ( dot( s, n ) > 0.0 )
        specular = pow( max( dot( r, v ), 0.0 ), material.shininess );

    // Combine the ambient, diffuse and specular contributions
    return lights[lightIndex].intensity * ( material.ka + material.kd * diffuse + material.ks * specular );
}

void main()
{
    vec3 adsColor = vec3( 0.0 );
    vec3 n = normalize( normal );
    int totalLights = numLights + numSpotlights;

    // contributions from each light
    for ( int i = 0; i < numLights; i++ )
    {
        adsColor += adsModel( i, position, n ) / totalLights;
    }

    // contributions from each spotlight
    for ( int i = 0; i < numSpotlights; i++ )
    {
        adsColor += adsSpotLightModel( i, position, n ) / totalLights;
    }

    // fogging
    float d = abs( position.z );
    float fogFactor = ( fog.maxDistance - d ) / ( fog.maxDistance - fog.minDistance );
    fogFactor = clamp( fogFactor, 0.0, 1.0 );

    vec3 color = mix( fog.color, adsColor, fogFactor );
    fragColor = vec4( color, 1.0 );
}
