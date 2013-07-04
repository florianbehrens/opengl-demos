#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in vec3 point; // The position of the data point. Varies per instance

out vec3 position;
out vec3 normal;
out vec3 diffuseColor;

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform vec2 cubeScale = vec2( 1.0, 1.0 );

// Calculate RGB triplet from HSV
vec3 hsvToRGB( float h, float s, float v )
{
    if ( s <= 0.0 )
        return vec3( v );

    h = h * 6.0;
    float c = v * s;
    float x = ( 1.0 - abs( ( mod( h, 2 ) - 1 ) ) ) * c;
    float m = v - c;
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;

    if ( h < 1.0 )      { r = c;   g = x;   b = 0.0;}
    else if ( h < 2.0 ) { r = x;   g = c;   b = 0.0; }
    else if ( h < 3.0 ) { r = 0.0; g = c;   b = x; }
    else if ( h < 4.0 ) { r = 0.0; g = x;   b = c; }
    else if ( h < 5.0 ) { r = x;   g = 0.0; b = c; }
    else                { r = c;   g = 0.0; b = x; }

    return vec3( r + m, g + m, b + m );
}

void main()
{
    // Move the cuboid according to the x, z values. This puts the cuboid in the
    // correct place in the x-z plane. The cubeScale adjusts the cross-sectional
    // area of the cube so that we can space the cuboids nicely
    position.xz = cubeScale * vertexPosition.xz + point.xz;

    // Move the vertex up by 0.5 so that bottom of cuboid is on the x-z plane
    // and the top is at y = 1.0. This nicely allows us to scale the y-dimension
    position.y = vertexPosition.y + 0.5;

    // Now scale the y component by the y-value of the data point
    position.y *= point.y;

    // Transform the position
    position = vec4( modelViewMatrix * vec4( position, 1.0 ) ).xyz;

    // If the y-value of the point is negative, flip the y-component of the normal
    vec3 n = vertexNormal;
    if ( point.y <= 0.0 )
        n.y = -n.y;

    // Transform the normal as usual
    normal = normalMatrix * n;

    // Choose a color based upon y value
    float hue = mix( 0.5, 0.05, 0.5 + point.y / 10.0 );
    diffuseColor = hsvToRGB( hue, 1.0, 1.0 );

    // Draw at the current position
    gl_Position = projectionMatrix * vec4( position, 1.0 );
}
