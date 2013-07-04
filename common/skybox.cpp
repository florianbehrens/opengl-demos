#include "skybox.h"

#include <QImage>
#include <QGLWidget>
#include <QOpenGLContext>

// Vertices
//
// 3 floats per vertex
// 4 vertices per face
// 6 faces
static const int vertexDataCount = 6 * 4 * 3;

static const float vertexData[vertexDataCount] = {
    // Left face
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,

    // Top face
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f,  0.5f,
     0.5f, 0.5f,  0.5f,
     0.5f, 0.5f, -0.5f,

    // Right face
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f, -0.5f,

    // Bottom face
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    // Front face
     0.5f, -0.5f, 0.5f,
     0.5f,  0.5f, 0.5f,
    -0.5f,  0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,

    // Back face
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f
};

// Indices
//
// 3 indices per triangle
// 2 triangles per face
// 6 faces
static const int indexDataCount = 6 * 3 * 2;

static const unsigned int indexData[indexDataCount] = {
    0,  1,  2,  0,  2,  3,  // Left face
    4,  5,  6,  4,  6,  7,  // Top face
    8,  9,  10, 8,  10, 11, // Right face
    12, 14, 15, 12, 13, 14, // Bottom face
    16, 17, 18, 16, 18, 19, // Front face
    20, 22, 23, 20, 21, 22  // Back face
};

SkyBox::SkyBox( QObject* parent )
    : QObject( parent ),
      m_seamlessCubeMap( true ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer )
{
}

void SkyBox::setMaterial( const MaterialPtr& material )
{
    m_material = material;
}

void SkyBox::create()
{
    // Allocate some storage to hold per-vertex data
    float* v = new float[vertexDataCount];               // Vertices
    unsigned int* el = new unsigned int[indexDataCount]; // Elements
    m_indexCount = indexDataCount;

    // Generate the vertex data
    memcpy( v, vertexData, vertexDataCount * sizeof( float ) );
    memcpy( el, indexData, indexDataCount * sizeof( unsigned int ) );

    // Create and populate the buffer objects
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( v, vertexDataCount * sizeof( float ) );

    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( el, indexDataCount * sizeof( unsigned int ) );

    // Delete our copy of the data as we no longer need it
    delete [] v;
    delete [] el;

    /** \todo Move into a "finalise()" like function called when geometry
     *  and material are set.
     */
    // Create a vertex array object
    m_vao.create();
    m_vao.bind();

    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();

    m_positionBuffer.bind();
    shader->enableAttributeArray( "vertexPosition" );
    shader->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    m_indexBuffer.bind();

    // End VAO setup
    m_vao.release();
}

void SkyBox::render( const QMatrix4x4& mvp )
{
    m_material->bind();
    m_material->shader()->setUniformValue( "mvp", mvp );
    m_material->shader()->setUniformValue( "cubeMapTex", 0 ); // We used 0 during setup
    m_vao.bind();
    glDrawElements( GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0 );
}

void SkyBox::setCubeMapBaseName( const QString& cubeMapBaseName )
{
    m_cubeMapBaseName = cubeMapBaseName;
    if ( isCreated() )
        updateMaterialCubeMap();
}

void SkyBox::setSeamlessCubeMapEnabled( bool seamlessEnabled )
{
    m_seamlessCubeMap = seamlessEnabled;
}

void SkyBox::updateMaterialCubeMap()
{
    if ( !m_material )
        return;

    // Setup the texture image data - 6 faces for cubemap
    const QStringList suffixes = ( QStringList() << "posx" << "negx" << "posy" << "negy" << "posz" << "negz" );
    GLuint targets[] =
    {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    TexturePtr cubeMapTexture( new Texture( Texture::TextureCubeMap ) );
    cubeMapTexture->create();
    cubeMapTexture->bind();
    for ( int i = 0; i < suffixes.size(); ++i )
    {
        QString fileName = QString( m_cubeMapBaseName ) + "_" + suffixes[i] + ".png";
        QImage image( fileName );
        cubeMapTexture->setCubeMapImage( targets[ i ], image.mirrored() );
    }

#if !defined(Q_OS_MAC)
    // Setup the sampler state
    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionR, GL_CLAMP_TO_EDGE );
    sampler->setMinificationFilter( GL_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );

    // Setup texture unit 0 with the texture data and sampler
    m_material->setTextureUnitConfiguration( 0, cubeMapTexture, sampler );
#else
    cubeMapTexture->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    cubeMapTexture->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    cubeMapTexture->setWrapMode( Texture::DirectionR, GL_CLAMP_TO_EDGE );
    cubeMapTexture->setMinificationFilter( GL_LINEAR );
    cubeMapTexture->setMagnificationFilter( GL_LINEAR );

    // Setup texture unit 0 with the texture data and sampler
    m_material->setTextureUnitConfiguration( 0, cubeMapTexture );
#endif

    // Use the seamless cubemap extensions
    if ( m_seamlessCubeMap && QOpenGLContext::currentContext()->hasExtension( "GL_ARB_seamless_cube_map" ) )
        glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
    else
        glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}
