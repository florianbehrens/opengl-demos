#version 330

uniform struct LightInfo {
    vec4 position;
    vec3 intensity;
} light;

uniform struct LineInfo {
  float width;
  vec4 color;
} line;

uniform vec3 Ka;            // Ambient reflectivity
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ks;            // Specular reflectivity
uniform float shininess;    // Specular shininess factor

in vec3 g_position;
in vec3 g_normal;
noperspective in vec3 g_edgeDistance;

layout (location = 0) out vec4 fragColor;


vec3 adsModel( const in vec3 pos, const in vec3 normal )
{
    // If the triangle is wound the wrong way, invert the normal so
    // that we get the desired lighting effect
    vec3 n = normal;
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
    vec3 diffuse = vec3( max( dot( s, n ), 0.0 ) );

    // Calculate the specular component
    vec3 specular = vec3( pow( max( dot( r, v ), 0.0 ), shininess ) );

    // Combine the ambient, diffuse and specular contributions
    return light.intensity * ( Ka + Kd * diffuse + Ks * specular );
}

void main()
{
    // Calculate the color from the phong model
    vec4 color = vec4( adsModel( g_position, normalize( g_normal ) ), 1.0 );

    // Find the smallest distance between the fragment and a triangle edge
    float d = min( g_edgeDistance.x, g_edgeDistance.y );
    d = min( d, g_edgeDistance.z );

    // Blend between line color and phong color
    float mixVal;
    if ( d < line.width - 1 )
    {
        mixVal = 1.0;
    }
    else if ( d > line.width + 1 )
    {
        mixVal = 0.0;
    }
    else
    {
        float x = d - ( line.width - 1 );
        mixVal = exp2( -2.0 * ( x * x ) );
    }

    fragColor = mix( color, line.color, mixVal );
}
