#version 330

uniform struct SpotLightInfo {
    vec4 position;
    vec3 intensity;
    vec3 direction;
    float exponent;
    float cutoffAngle;
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


vec3 adsSpotLightModel( const in vec3 pos, const in vec3 norm )
{
    // If the triangle is wound the wrong way, invert the normal so
    // that we get the desired lighting effect
    vec3 n = norm;
    if ( !gl_FrontFacing )
        n = -n;

    // Ambient contribution
    vec3 ambient = light.intensity * material.ka;

    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( light.position ) - position );

    // Calculate angle of fragment from spot light axis
    float theta = acos( dot( -s, light.direction ) );

    // Calculate cutoff angle in radians
    float thetaCutoff = radians( clamp( light.cutoffAngle, 0.0, 90.0 ) );

    if ( theta < thetaCutoff )
    {
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
        float spotLightFactor = pow( dot( -s, light.direction ), light.exponent );

        // Combine the ambient, diffuse and specular contributions
        return ambient +
            light.intensity * spotLightFactor * ( material.kd * diffuse + material.ks * specular );
    }
    else
    {
        // Fragment is outside cone of spotlight - ambient only
        return ambient;
    }
}

void main()
{
    fragColor = vec4( adsSpotLightModel( position, normalize( normal ) ), 1.0 );
}
