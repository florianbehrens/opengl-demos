#include "cubemapreflectionscene.h"

#include <mesh.h>
#include <objloader.h>
#include <skybox.h>

CubeMapReflectionScene::CubeMapReflectionScene( QObject* parent )
    : AbstractScene( parent ),
      m_theta( 0.0f ),
      m_phi( 0.0f ),
      m_modelMatrix(),
      m_viewMatrix(),
      m_projectionMatrix(),
      m_skyBox( new SkyBox( this ) ),
      m_yawSpeed( 0.0f ),
      m_pitchSpeed( 0.0f )
{
}

void CubeMapReflectionScene::initialise()
{
    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // Create a material to be shared by the skybox and reflective object
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/skybox.vert",
                          ":/shaders/skybox.frag" );

    // Create the skybox
    m_skyBox->setMaterial( material );
    m_skyBox->create();
#if !defined(Q_OS_MAC)
    m_skyBox->setCubeMapBaseName( "../../../opengl/assets/textures/cubemap_miramar/miramar" );
#else
    m_skyBox->setCubeMapBaseName( "../../../../../../opengl/assets/textures/cubemap_miramar/miramar" );
#endif

    // Create a renderable object
    // Load the model mesh
    ObjLoader loader;
    loader.setTangentGenerationEnabled( false );
#if !defined(Q_OS_MAC)
    loader.load( "../../../opengl/assets/meshes/ladybird.obj" );
#else
    loader.load( "../../../../../../opengl/assets/meshes/ladybird.obj" );
#endif

    // Create a material to be shared by the skybox and reflective object
    MaterialPtr material2( new Material );
    material2->setShaders( ":/shaders/skyboxreflection.vert",
                          ":/shaders/skyboxreflection.frag" );

    m_mesh = new Mesh;
    m_mesh->setMaterial( material2 );
    m_mesh->setMeshData( loader );

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );
}

void CubeMapReflectionScene::update( float t )
{
    // Nothing to do here
    Q_UNUSED( t );
    m_theta += m_yawSpeed;
    m_phi += m_pitchSpeed;
}

void CubeMapReflectionScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set up the view matrix
    QVector3D eye( 0.0f, 0.0f, 0.0f );
    QVector3D center( 0.0f, 0.0f, 1.0f );
    QVector3D up( 0.0f, 1.0f, 0.0f );
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt( eye, center, up );
    m_viewMatrix.rotate( m_phi, 1.0f, 0.0f, 0.0f );
    m_viewMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );

    m_modelMatrix.setToIdentity();
    QMatrix4x4 modelViewMatrix = m_viewMatrix * m_modelMatrix;
    QMatrix4x4 mvp = m_projectionMatrix * modelViewMatrix;

    // We need a pointer to the material's shader to set uniform variables
    QOpenGLShaderProgramPtr shader = m_skyBox->material()->shader();
    shader->bind();

    // Draw the skybox
    glDepthFunc( GL_LEQUAL );
    m_skyBox->render( mvp );
    glDepthFunc( GL_LESS );

    eye = QVector3D( 0.0f, 0.0f, -4.0f );
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt( eye, center, up );
    m_viewMatrix.rotate( m_phi, 1.0f, 0.0f, 0.0f );
    m_viewMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );

    // Calculate the position of the camera in world coordinates
    QMatrix4x4 rot;
    rot.rotate( -m_theta, 0.0f, 1.0f, 0.0f );
    rot.rotate( -m_phi, 1.0f, 0.0f, 0.0f );
    QVector4D worldEye = rot * eye;
    QVector3D worldCamera = worldEye.toVector3D();

    // Rotate (and scale the model)
    m_modelMatrix.setToIdentity();
    static float theta = 0.0f;
    theta += 0.3f;
    m_modelMatrix.rotate( theta, 0.0f, 1.0f, 0.0f );
    m_modelMatrix.scale( 0.002f ); // Scale the ladybird mesh down to a reasonable size

    modelViewMatrix = m_viewMatrix * m_modelMatrix;
    mvp = m_projectionMatrix * modelViewMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();

    shader = m_mesh->material()->shader();
    shader->bind();

    shader->setUniformValue( "worldCameraPosition", worldCamera );
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "modelMatrix", m_modelMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_projectionMatrix );
    shader->setUniformValue( "mvp", mvp );

    // Draw the reflective mesh
    shader->setUniformValue( "materialColor", QVector4D( 0.9f, 0.2f, 0.2f, 1.0f) );
    shader->setUniformValue( "reflectFactor", 0.6f );

    m_mesh->render();
}

void CubeMapReflectionScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective( 25.0f, aspect, 0.3f, 100.0f );
}

void CubeMapReflectionScene::setYawSpeed( float yawSpeed )
{
    m_yawSpeed = yawSpeed;
}

void CubeMapReflectionScene::setPitchSpeed( float pitchSpeed )
{
    m_pitchSpeed = pitchSpeed;
}
