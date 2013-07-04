#version 330

#define M_PI 3.1415926535897932384626433832795

in vec3 lightDir;
in vec3 viewDir;
in vec2 texCoord;
in vec3 modelPosition;

struct LightInfo
{
    vec4 position;  // Light position in eye coords.
    vec3 intensity; // A,D,S intensity
};
uniform LightInfo light;

struct MaterialInfo
{
    vec3 ka;            // Ambient reflectivity
};
uniform MaterialInfo material;

layout( location = 0 ) out vec4 fragColor;

vec3 ambientDiffuseModel( const in vec3 norm, const in vec3 diffuseReflect )
{
    // Reflection of light direction about normal
    vec3 r = reflect( -lightDir, norm );

    // Calculate the ambient contribution
    vec3 ambient = light.intensity * material.ka;

    // Calculate the diffuse contribution
    float sDotN = max( dot( lightDir, norm ), 0.0 );
    vec3 diffuse = light.intensity * diffuseReflect * sDotN;

    // Sum the ambient and diffuse contributions
    return ambient + diffuse;
}

vec3 proceduralColor(vec2 texCoord, float steps)
{
    // implement me!
    return vec3(0.0, 1.0, 1.0);
}

vec3 proceduralNormal(vec2 texCoord, float steps)
{
    // Generate dimples. Similar to the grid of cirlces procedural texture
    // We use twice as many bumps in s direction as this covers twice the angular
    // size of the t direction.
    vec2 p = fract( vec2( steps * texCoord.s, steps * texCoord.t ) ) - vec2( 0.5 );
    float d = p.x * p.x + p.y * p.y;
    float f = 1.0 / sqrt( d + 1.0 );

    const float bumpSize = 0.2;
    if ( d >= bumpSize )
    {
        p = vec2( 0.0 );
        f = 1.0;
    }
    vec3 normal = vec3( p.x, p.y, 1.0 ) * f;
    return normal;
}

void main()
{
    vec3 diffuseColor = proceduralColor(texCoord, 50);

    vec3 normal = vec3( 0.0, 0.0, 1.0);
    //vec3 normal = proceduralNormal( texCoord, 22 );

    vec3 c = ambientDiffuseModel( normal, diffuseColor );

    // useful for debugging your normals
    // vec3 c = ( normal * 0.5 ) + vec3( 0.5 );

    fragColor = vec4( c, 1.0 );
}
