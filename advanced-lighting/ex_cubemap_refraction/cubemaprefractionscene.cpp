#include "cubemaprefractionscene.h"

#include <mesh.h>
#include <objloader.h>
#include <skybox.h>

CubeMapRefractionScene::CubeMapRefractionScene( QObject* parent )
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

void CubeMapRefractionScene::initialise()
{
    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // Create a material to be shared by the skybox and reflective object
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/skyboxrefraction.vert",
                          ":/shaders/skyboxrefraction.frag" );

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
    loader.load( "../../../opengl/assets/meshes/toyplane.obj" );
#else
    loader.load( "../../../../../../opengl/assets/meshes/toyplane.obj" );
#endif

    m_mesh = new Mesh;
    m_mesh->setMaterial( m_skyBox->material() );
    m_mesh->setMeshData( loader );

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );
}

void CubeMapRefractionScene::update( float t )
{
    // Nothing to do here
    Q_UNUSED( t );
    m_theta += m_yawSpeed;
    m_phi += m_pitchSpeed;
}

void CubeMapRefractionScene::render()
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

    // Draw the skybox
    shader->setUniformValue( "drawSkyBox", true );
    glDepthFunc(GL_LEQUAL);
    m_skyBox->render( mvp );
    glDepthFunc(GL_LESS);

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
    m_modelMatrix.scale( 0.05f ); // Scale the toyplane mesh down to a reasonable size

    modelViewMatrix = m_viewMatrix * m_modelMatrix;
    mvp = m_projectionMatrix * modelViewMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();

    shader->setUniformValue( "worldCameraPosition", worldCamera );
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "modelMatrix", m_modelMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_projectionMatrix );
    shader->setUniformValue( "mvp", mvp );

    // Draw the reflective mesh
    shader->setUniformValue( "drawSkyBox", false );
    shader->setUniformValue( "material.eta", 0.96f );
    shader->setUniformValue( "material.reflectionFactor", 0.0f );

    m_mesh->render();
}

void CubeMapRefractionScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective( 25.0f, aspect, 0.3f, 100.0f );
}

void CubeMapRefractionScene::setYawSpeed( float yawSpeed )
{
    m_yawSpeed = yawSpeed;
}

void CubeMapRefractionScene::setPitchSpeed( float pitchSpeed )
{
    m_pitchSpeed = pitchSpeed;
}
