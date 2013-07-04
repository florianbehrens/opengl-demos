#version 330

uniform float time;

in vec2 texCoord;

layout( location = 0 ) out vec4 fragColor;

void main()
{
    vec2 tc = texCoord;

    // Simple gradient
    fragColor = vec4( tc.s, tc.s, tc.s, 1.0 );

    // Repeating gradient
    //float i = fract( 5.0 * tc.s );
    //fragColor = vec4( i, i, i, 1.0 );

    // Smooth repeating gradient
    //float i = abs( 2.0 * fract( 5.0 * tc.s ) - 1.0 );
    //fragColor = vec4( i, i, i, 1.0 );

    // Chequer board
    //float i = mod( floor( 10.0 * tc.s ) + floor( 10.0 * tc.t ), 2.0 );
    //fragColor = vec4( i, i, i, 1.0 );
}


