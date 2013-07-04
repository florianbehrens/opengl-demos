#version 330

in vec3 position;
in vec2 texCoord;

uniform vec2 resolution;
uniform float time;
uniform sampler2D texture0;

layout( location = 0 ) out vec4 fragColor;

void main()
{
    // Scale down the texture coordinates a little
    vec2 q = gl_FragCoord.xy / resolution.xy;
    vec2 uv = 0.5 + ( q - 0.5 ) * ( 0.9 + 0.1 * sin( 0.2 * time ) );

    // Lookup color, each channel from a slightly different location
    vec3 col;
    col.r = texture( texture0, vec2( uv.s + 0.003, uv.t ) ).r;
    col.g = texture( texture0, vec2( uv ) ).g;
    col.b = texture( texture0, vec2( uv.s - 0.003, uv.t ) ).b;

    // Contrast
    col = clamp( col * 0.5 + 0.5 * col * col * 1.2, 0.0, 1.0 );

    // Vignette
    col *= 0.5 + 0.5 * 16.0 * uv.s * uv.t * ( 1.0 - uv.s ) * ( 1.0 - uv.t );

    // Tint
    col *= vec3( 0.8, 1.0, 0.7 );

    // TV lines
    col *= 0.9 + 0.1 * sin( 10.0 * time + uv.t * 1000.0 );

    // TV flicker
    col *= 0.97 + 0.03 * sin( 110.0 * time );

    // Sinusoidal sweep between original and post-processed
    vec3 originalColor = texture( texture0, texCoord ).xyz;
    float comp = smoothstep( 0.2, 0.7, sin( 0.7 * time ) );
    col = mix( col, originalColor, clamp( -2.0 + 2.0 * q.x + 3.0 * comp, 0.0, 1.0 ) );

    fragColor = vec4( col, 1.0 );
}
