#version 330

uniform struct LightInfo {
    vec4 position;
    vec3 intensity;
} light;

uniform vec3 Ka;            // Ambient reflectivity
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ks;            // Specular reflectivity
uniform float shininess;    // Specular shininess factor

in vec3 position;
in vec3 normal;

layout (location = 0) out vec4 fragColor;


vec3 adsModel( const in vec3 pos, const in vec3 norm )
{
    // If the triangle is wound the wrong way, invert the normal so
    // that we get the desired lighting effect
    vec3 n = norm;
    if ( !gl_FrontFacing )
        n = -n;

    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( light.position ) - pos );

    // Calculate the vector from the fragment to the eye position (the
    // origin since this is in "eye" or "camera" space
    vec3 v = normalize( -pos );

    // Refleft the light beam using the normal at this fragment
    vec3 r = reflect( -s, n );

    // Calculate the diffus component
    float diffuse = max( dot( s, n ), 0.0 );

    // Calculate the specular component
    float specular = 0.0;
    if ( dot( s, n ) > 0.0 )
        specular = pow( max( dot( r, v ), 0.0 ), shininess );

    // Combine the ambient, diffuse and specular contributions
    return light.intensity * ( Ka + Kd * diffuse + Ks * specular );
}

void main()
{
    fragColor = vec4( adsModel( position, normalize( normal ) ), 1.0 );
}
