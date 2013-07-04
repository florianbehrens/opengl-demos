#version 400

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

//in vec2 te_texCoords[];

//out vec2 g_texCoords;
noperspective out vec3 g_edgeDistance;

uniform mat4 viewportMatrix;

void main()
{
    // Transform each vertex into viewport space
    vec2 p0 = vec2( viewportMatrix * ( gl_in[0].gl_Position / gl_in[0].gl_Position.w ) );
    vec2 p1 = vec2( viewportMatrix * ( gl_in[1].gl_Position / gl_in[1].gl_Position.w ) );
    vec2 p2 = vec2( viewportMatrix * ( gl_in[2].gl_Position / gl_in[2].gl_Position.w ) );

    // Calculate lengths of 3 edges of triangle
    float a = length( p1 - p2 );
    float b = length( p2 - p0 );
    float c = length( p1 - p0 );

    // Calculate internal angles using the cosine rule
    float alpha = acos( ( b * b + c * c - a * a ) / ( 2.0 * b * c ) );
    float beta = acos( ( a * a + c * c - b * b ) / ( 2.0 * a * c ) );

    // Calculate the perpendicular distance of each vertex from the opposing edge
    float ha = abs( c * sin( beta ) );
    float hb = abs( c * sin( alpha ) );
    float hc = abs( b * sin( alpha ) );

    // Now add this perpendicular distance as a per-vertex property in addition to
    // the position calculated in the vertex shader.

    // Vertex 0 (a)
    g_edgeDistance = vec3( ha, 0, 0 );
    //g_texCoords = te_texCoords[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    // Vertex 1 (b)
    g_edgeDistance = vec3( 0, hb, 0 );
    //g_texCoords = te_texCoords[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    // Vertex 2 (c)
    g_edgeDistance = vec3( 0, 0, hc );
    //g_texCoords = te_texCoords[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    // Finish the primitive off
    EndPrimitive();
}
