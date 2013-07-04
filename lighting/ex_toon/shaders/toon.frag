#version 330

uniform struct LightInfo {
    vec4 position;
    vec3 intensity;
} light;

uniform struct MaterialInfo {
    vec3 ka; // Ambient reflectivity
    vec3 kd; // Diffuse reflectivity
} material;

in vec3 position;
in vec3 normal;

layout (location = 0) out vec4 fragColor;

const int levels = 4;
const float scaleFactor = 1.0 / levels;

vec3 toonShade( const in vec3 pos, const in vec3 norm )
{
    // If the triangle is wound the wrong way, invert the normal so
    // that we get the desired lighting effect
    vec3 n = norm;
    if ( !gl_FrontFacing )
        n = -n;

    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( light.position ) - pos );

    // Calculate the diffuse component
    float cosine = dot( s, n );
    float diffuse = floor( cosine * levels ) * scaleFactor;

    // Combine the ambient and diffuse contributions. No specular!
    return light.intensity * ( material.ka + material.kd * diffuse );
}

void main()
{
    fragColor = vec4( toonShade( position, normalize( normal ) ), 1.0 );
}
