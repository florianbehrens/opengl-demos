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
    // implement me
    return vec3(0.5);
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

    adsColor += adsModel( 0, position, n );

    // add other lights
    
    // add spot lights
    
    // add fogging

    fragColor = vec4( adsColor, 1.0 );
}
