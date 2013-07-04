#include "ambientocclusionscene.h"

#include "mesh.h"
#include "objloader.h"
#include "sampler.h"
#include "texture.h"

#include <QImage>
#include <QObject>

#include <math.h>

AmbientOcclusionScene::AmbientOcclusionScene()
    : CameraScene(),
      m_modelMatrix(),
      m_mesh( 0 ),
      m_theta( 0.0f )
{
    m_modelMatrix.setToIdentity();

    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 4.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

MaterialPtr AmbientOcclusionScene::createAmbientOcclusionMaterial( const QString& diffuse,
                                                                   const QString& aoFactors ) const
{
    MaterialPtr material( new Material );

    // Setup the shaders
    material->setShaders( ":/shaders/ambientocclusion.vert",
                          ":/shaders/ambientocclusion.frag" );

    // Create a diffuse texture
    TexturePtr diffuseTexture( new Texture( Texture::Texture2D ) );
    diffuseTexture->create();
    diffuseTexture->bind();
    diffuseTexture->setImage( QImage( diffuse ) );

    // Create the ambient occlusion factors texture
    TexturePtr aoTexture( new Texture( Texture::Texture2D ) );
    aoTexture->create();
    aoTexture->bind();
    aoTexture->setImage( QImage( aoFactors ) );

#if !defined(Q_OS_MAC)
    // Create a sampler. This can be shared by both textures
    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );
    sampler->setMinificationFilter( GL_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );

    // We associate the diffuse texture with texture unit 0 and
    // the ao factors texture with unit 1
    material->setTextureUnitConfiguration( 0, diffuseTexture, sampler, "diffuseTexture" );
    material->setTextureUnitConfiguration( 1, aoTexture, sampler, "ambientOcclusionTexture" );
#else
    diffuseTexture->bind();
    diffuseTexture->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    diffuseTexture->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    diffuseTexture->setMinificationFilter( GL_LINEAR );
    diffuseTexture->setMagnificationFilter( GL_LINEAR );

    aoTexture->bind();
    aoTexture->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    aoTexture->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    aoTexture->setMinificationFilter( GL_LINEAR );
    aoTexture->setMagnificationFilter( GL_LINEAR );

    // We associate the diffuse texture with texture unit 0 and
    // the ao factors texture with unit 1
    material->setTextureUnitConfiguration( 0, diffuseTexture, "diffuseTexture" );
    material->setTextureUnitConfiguration( 1, aoTexture, "ambientOcclusionTexture" );
#endif
    return material;
}

void AmbientOcclusionScene::initialise()
{
    // Load the model mesh
    ObjLoader loader;
    loader.setTangentGenerationEnabled( false );
#if !defined(Q_OS_MAC)
    loader.load( "../../../opengl/assets/meshes/ogrehead/ogrehead.obj" );
#else
    loader.load( "../../../../../../opengl/assets/meshes/ogrehead/ogrehead.obj" );
#endif

    // Create an ambient occlusion material that uses a diffuse texture
    // and an ambient occlusion (accessibility factors) texture
#if !defined(Q_OS_MAC)
    MaterialPtr material =
        createAmbientOcclusionMaterial(
                "../../../opengl/assets/meshes/ogrehead/ogrehead_diffuse.png",
                "../../../opengl/assets/meshes/ogrehead/ogrehead_ambientocclusion.png" );
#else
    MaterialPtr material =
        createAmbientOcclusionMaterial(
                "../../../../../../opengl/assets/meshes/ogrehead/ogrehead_diffuse.png",
                "../../../../../../opengl/assets/meshes/ogrehead/ogrehead_ambientocclusion.png" );
#endif

    // Create a renderable object
    m_mesh = new Mesh;
    m_mesh->setMaterial( material );
    m_mesh->setMeshData( loader );

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    glClearColor( 0.9f, 0.9f, 0.9f, 1.0f );
}

void AmbientOcclusionScene::update( float t )
{
    Q_UNUSED( t );
    m_theta += 0.2;
    if ( m_theta > 360.0f )
        m_theta -= 360.0f;
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
}

void AmbientOcclusionScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_mesh->material()->bind();
    QOpenGLShaderProgramPtr shader = m_mesh->material()->shader();
    m_camera->setStandardUniforms(shader, m_modelMatrix);

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 0.7f, 0.7f, 0.7f ) );

    // Set the material properties
    // Ambient + Diffuse
    shader->setUniformValue( "material.Ka", QVector3D( 0.3f, 0.3f, 0.3f ) );
    shader->setUniformValue( "material.Kd", QVector3D( 0.9f, 0.9f, 0.9f ) );

    // Uncomment for Ambient only
    //shader->setUniformValue( "material.Ka", QVector3D( 0.8f, 0.8f, 0.8f ) );
    //shader->setUniformValue( "material.Kd", QVector3D( 0.0f, 0.0f, 0.0f ) );

    // Let the AoMesh setup it the remainder of its state and draw itself
    m_mesh->render();
}
