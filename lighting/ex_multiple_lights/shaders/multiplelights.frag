#version 330

uniform struct LightInfo {
    vec4 position;
    vec3 intensity;
} lights[5];

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


vec3 adsModel( const in int lightIndex, const in vec3 pos, const in vec3 norm )
{
    // If the triangle is wound the wrong way, invert the normal so
    // that we get the desired lighting effect
    vec3 n = norm;
    if ( !gl_FrontFacing )
        n = -n;

    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( lights[lightIndex].position ) - position );

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

    // Combine the ambient, diffuse and specular contributions
    return lights[lightIndex].intensity * ( material.ka + material.kd * diffuse + material.ks * specular );
}

void main()
{
    vec3 color = vec3( 0.0 );
    vec3 n = normalize( normal );
    for ( int i = 0; i < 5; i++ )
    {
        color += adsModel( i, position, n ) / 5.0;
    }

    fragColor = vec4( color, 1.0 );
}
