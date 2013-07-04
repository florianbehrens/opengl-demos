#version 330

uniform float time;

in vec2 texCoord;

layout( location = 0 ) out vec4 fragColor;

float aaStep( const in float threshold, const in float value )
{
    // Calculate the width of the threshold that we need to apply AA over.
    // This is done using dFdx() and dFdy() built-in functions that calculate
    // an approximation of the partial derivative of the function that produces
    // "value". It does this by comparing "value" at surrounding fragments.
    // These are the only way to indirectly access other fragment's data.
    //
    // The factor 0.7 is used rather than 0.5 as the smoothstep function is
    // steeper in it's central region than a simple linear interpolation.
    // This expands the width over which we apply AA slightly to compensate.
    float aaWidth = 0.7 * length( vec2( dFdx( value ), dFdy( value ) ) );
    return smoothstep( threshold - aaWidth, threshold + aaWidth, value );
}

void main()
{
    vec2 tc = texCoord;

    // Sinusoid
    float val = 0.5 - tc.t + 0.1 * sin( 30.0 * tc.s );
    float i = smoothstep( -0.01, 0.01, val );
    fragColor = vec4( i, i, i, 1.0 );

    // Circles
    //float val = length( fract( 5.0 * tc ) - vec2( 0.5, 0.5 ) );
    //float i = smoothstep( 0.3, 0.32, val );
    //fragColor = vec4( i, i, i, 1.0 );

    // Better AA Cirlces
    //float val = length( fract( 5.0 * tc ) - vec2( 0.5, 0.5 ) );
    //float i = aaStep( 0.3, val );
    //fragColor = vec4( i, i, i, 1.0 );
}


