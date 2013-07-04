#version 330

in vec3 position;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

struct LightInfo
{
    vec4 position;  // Light position in eye coords.
    vec3 intensity; // A,D,S intensity
};
uniform LightInfo light;

struct MaterialInfo
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
};
uniform MaterialInfo material;

layout( location = 0 ) out vec4 fragColor;

void phongModel( vec3 pos, vec3 norm, out vec3 ambientAndDiff, out vec3 spec )
{
    // Some useful vectors
    vec3 s = normalize( vec3( light.position ) - pos );
    vec3 v = normalize( -pos );
    vec3 r = reflect( -s, norm );

    // Calculate the ambient contribution
    vec3 ambient = light.intensity * material.Ka;

    // Calculate the diffuse contribution
    float sDotN = max( dot( s, norm ), 0.0 );
    vec3 diffuse = light.intensity * material.Kd * sDotN;

    // Sum the ambient and diffuse contributions
    ambientAndDiff = ambient + diffuse;

    // Calculate the specular highlight contribution
    spec = vec3( 0.0 );
    if ( sDotN > 0.0 )
    {
        spec = light.intensity * pow( max( dot( r, v ), 0.0 ), material.shininess );
    }
}

void main()
{
    // Sample the textures at the interpolated texCoords
    vec4 diffuseTextureColor = texture( texture0, texCoord );
    vec4 specularTextureColor = texture( texture1, texCoord );
    float alpha = texture( texture2, texCoord ).a;

    if ( alpha > 0.5 )
        discard;

    // Use 2-sided shading
    if ( gl_FrontFacing )
    {
        // Calculate the lighting model, keeping the specular component separate
        vec3 ambientAndDiff;
        vec3 spec;
        phongModel( position, normalize( normal ), ambientAndDiff, spec );

        // Calculate the final fragment color by multiplying the ambient and diffuse color
        // by the texture color and then adding on the specular highlight contribution
        vec4 diffColor = vec4( ambientAndDiff, 1.0 ) * diffuseTextureColor;

        vec4 specularColor = vec4( spec, 1.0 ) * specularTextureColor;

        fragColor = diffColor + specularColor;
    }
    else
    {
        fragColor = vec4( 0.173, 0.447, 0.78, 1.0 );
    }
}
