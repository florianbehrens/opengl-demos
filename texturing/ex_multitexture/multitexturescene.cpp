#include "multitexturescene.h"

#include <cube.h>
#include <material.h>

#include <QGLWidget>
#include <QImage>

MultiTextureScene::MultiTextureScene( QObject* parent )
    : CameraScene( parent ),
      m_cube( 0 ),
      m_theta( 0.0f ),
      m_factor( 1.0f )
{
}

void MultiTextureScene::initialise()
{
    // Create a material that performs multi-texturing
#if !defined(Q_OS_MAC)
    MaterialPtr material = createMultiTextureMaterial(
                               "../../../opengl/assets/textures/brick1.jpg",
                               "../../../opengl/assets/textures/moss.png" );
#else
    MaterialPtr material = createMultiTextureMaterial(
                               "../../../../../../opengl/assets/textures/brick1.jpg",
                               "../../../../../../opengl/assets/textures/moss.png" );
#endif

    // Create a cube and set the material on it
    m_cube = new Cube( this );
    m_cube->setMaterial( material );
    m_cube->create();

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // Enable depth testing and back-face culling
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    glClearColor( 0.9f, 0.9f, 0.9f, 1.0f );
}

void MultiTextureScene::update( float t )
{
    // Nothing to do here
    Q_UNUSED( t );

    m_modelMatrix.setToIdentity();
    m_theta += 0.4f;
    if ( m_theta > 360.0f )
        m_theta -= 360.0f;
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
    m_modelMatrix.rotate( 90.0f, 1.0, 0.0f, 0.0f );

    static float df = -0.01f;
    m_factor += df;
    if ( m_factor <= 0.0f || m_factor >= 1.0f )
        df = -df;
}

void MultiTextureScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_cube->material()->bind();
    QOpenGLShaderProgramPtr shader = m_cube->material()->shader();
    shader->bind();

    m_camera->setStandardUniforms( shader, m_modelMatrix );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.Ka", QVector3D( 0.4f, 0.4f, 0.4f ) );
    shader->setUniformValue( "material.Kd", QVector3D( 0.9f, 0.6f, 0.6f ) );
    shader->setUniformValue( "material.Ks", QVector3D( 0.05f, 0.05f, 0.05f ) );
    shader->setUniformValue( "material.shininess", 2.0f );

    // Update the amount of the overlay texture we should blend in
    shader->setUniformValue( "factor", m_factor );

    // Let the mesh setup the remainder of its state and draw itself
    m_cube->render();
}

MaterialPtr MultiTextureScene::createMultiTextureMaterial( const QString& texture0FileName,
                                                           const QString& texture1FileName )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/multitexture.vert",
                          ":/shaders/multitexture.frag" );

    // Create a base diffuse texture
    TexturePtr texture0( new Texture( Texture::Texture2D ) );
    texture0->create();
    texture0->bind();
    texture0->setImage( QImage( texture0FileName ) );
    texture0->generateMipMaps();

    // Create the overlay texture
    TexturePtr texture1( new Texture( Texture::Texture2D ) );
    texture1->create();
    texture1->bind();
    texture1->setImage( QImage( texture1FileName ) );
    texture1->generateMipMaps();

#if !defined(Q_OS_MAC)
    // Create a sampler. This can be shared by both textures
    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );
    sampler->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first 2 texture units
    material->setTextureUnitConfiguration( 0, texture0, sampler, QByteArrayLiteral( "texture0" ) );
    material->setTextureUnitConfiguration( 1, texture1, sampler, QByteArrayLiteral( "texture1" ) );
#else
    texture0->bind();
    texture0->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture0->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture0->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    texture0->setMagnificationFilter( GL_LINEAR );

    texture1->bind();
    texture1->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture1->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture1->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    texture1->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first 2 texture units
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "texture0" ) );
    material->setTextureUnitConfiguration( 1, texture1, QByteArrayLiteral( "texture1" ) );
#endif
    return material;
}
