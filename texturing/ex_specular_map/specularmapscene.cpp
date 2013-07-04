#include "specularmapscene.h"

#include <camera.h>
#include <cube.h>

#include <QGLWidget>
#include <QImage>

SpecularMapScene::SpecularMapScene( QObject* parent )
    : CameraScene( parent ),
      m_cube( 0 ),
      m_theta( 0.0f ),
      m_rotateCube( false )
{
    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 4.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void SpecularMapScene::initialise()
{
    // Create a material that performs multi-texturing
#if !defined(Q_OS_MAC)
    MaterialPtr material = createTextureMaterial(
                               "../../../opengl/assets/textures/pattern_10/diffuse.png",
                               "../../../opengl/assets/textures/pattern_10/specular.png");
#else
    MaterialPtr material = createTextureMaterial(
                               "../../../../../../opengl/assets/textures/pattern_10/diffuse.png",
                               "../../../../../../opengl/assets/textures/pattern_10/specular.png");
#endif

    // Create a cube and set the material on it
    m_cube = new Cube( this );
    m_cube->setMaterial( material );
    m_cube->create();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    // Set the clear color to (almost) white
    glClearColor( 0.9f, 0.9f, 0.9f, 1.0f );
}

void SpecularMapScene::update( float t )
{
    // Nothing to do here
    Q_UNUSED( t );

    // Rotate the cube
    if ( m_rotateCube )
    {
        m_theta += 0.5f;
        if ( m_theta > 360.0f )
            m_theta -= 360.0f;
    }
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
    m_modelMatrix.rotate( 90.0f, 1.0f, 0.0f, 0.0f );
}

void SpecularMapScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_cube->material()->bind();
    QOpenGLShaderProgramPtr shader = m_cube->material()->shader();
    m_camera->setStandardUniforms( shader, m_modelMatrix );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.Ka", QVector3D( 0.7f, 0.7f, 0.7f ) );
    shader->setUniformValue( "material.Kd", QVector3D( 0.9f, 0.9f, 0.9f ) );
    shader->setUniformValue( "material.Ks", QVector3D( 0.4f, 0.4f, 0.4f ) );
    shader->setUniformValue( "material.shininess", 20.0f );

    // Let the mesh setup the remainder of its state and draw itself
    m_cube->render();
}

MaterialPtr SpecularMapScene::createTextureMaterial( const QString& diffuseFileName,
                                                     const QString& specularFileName )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/specularmap.vert",
                          ":/shaders/specularmap.frag" );

    // Create a diffuse texture
    TexturePtr texture0( new Texture( Texture::Texture2D ) );
    texture0->create();
    texture0->bind();
    texture0->setImage( QImage( diffuseFileName ) );
    texture0->generateMipMaps();

    // Create a specular texture map
    TexturePtr texture1( new Texture( Texture::Texture2D ) );
    texture1->create();
    texture1->bind();
    texture1->setImage( QImage( specularFileName ) );
    texture1->generateMipMaps();

#if !defined(Q_OS_MAC)
    // Create a sampler. This can be shared by many textures
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

void SpecularMapScene::toggleMultiSampling()
{
    m_multisample = !m_multisample;
    if (m_multisample) {
        glEnable(GL_MULTISAMPLE);
    } else {
        glDisable(GL_MULTISAMPLE);
    }
}

