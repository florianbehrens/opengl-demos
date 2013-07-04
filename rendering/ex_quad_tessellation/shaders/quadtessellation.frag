#version 400

uniform struct LineInfo {
  float width;
  vec4 color;
} line;

noperspective in vec3 g_edgeDistance;

layout (location = 0) out vec4 fragColor;

void main()
{
    // Calculate the color from the phong model
    vec4 color = vec4( 1.0 );

    // Find the smallest distance between the fragment and a triangle edge
    float d = min( g_edgeDistance.x, g_edgeDistance.y );
    d = min( d, g_edgeDistance.z );

    // Blend between line color and phong color
    float mixVal;
    if ( d < line.width - 1.0 )
    {
        mixVal = 1.0;
    }
    else if ( d > line.width + 1.0 )
    {
        mixVal = 0.0;
    }
    else
    {
        float x = d - ( line.width - 1.0 );
        mixVal = exp2( -2.0 * ( x * x ) );
    }


    fragColor = mix( color, line.color, mixVal );
    //fragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}
