#include "spotlightscene.h"

#include "axisalignedboundingbox.h"
#include "mesh.h"
#include "objloader.h"
#include "plane.h"

#include <QObject>

#include <math.h>

static const float pi = 4.0 * atanf( 1.0f );
static const float degToRad = pi / 180.0f;

SpotLightScene::SpotLightScene( const QString& fileName )
    : m_fileName( fileName ),
      m_modelMatrix(),
      m_viewMatrix(),
      m_projectionMatrix(),
      m_mesh( 0 ),
      m_plane( 0 ),
      m_thetaLight( 120.0f ),
      m_thetaModel( 0.0f ),
      m_cutoffAngle( 15.0f ),
      m_exponent( 30.0 )
{
    m_modelMatrix.setToIdentity();
}

void SpotLightScene::initialise()
{
    // Load the model mesh
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled( false );
    loader.setTangentGenerationEnabled( false );
    loader.load( m_fileName );

    // Create a material
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/spotlight.vert",
                          ":/shaders/spotlight.frag" );

    // Create a renderable object
    m_mesh = new Mesh;
    m_mesh->setMaterial( material );
    m_mesh->setMeshData( loader );

    m_plane = new Plane;
    m_plane->setMaterial( material );
    m_plane->setWidth( 10.0f );
    m_plane->setHeight( 10.0f );
    m_plane->create();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void SpotLightScene::update( float t )
{
    Q_UNUSED( t );

    m_thetaModel += 0.2;
    if ( m_thetaModel > 360.0f )
        m_thetaModel -= 360.0f;

    // Get the maximum extent of the object so that we can scale it down to unit size
    float maxExtent = m_mesh->boundingVolume()->maxExtent();

    m_modelMatrix.setToIdentity();
    m_modelMatrix.scale( 1.0f / ( 0.2f * maxExtent ) );
    m_modelMatrix.rotate( m_thetaModel, 0.0f, 1.0f, 0.0f );

    m_thetaLight += 0.5;
    if ( m_thetaLight > 360.0f )
        m_thetaLight -= 360.0f;

    const float r = 12.0f;
    const float h = 8.0f;
    m_worldLightPos = QVector4D( r * cos( m_thetaLight * degToRad ), h, r * sin( m_thetaLight * degToRad ), 1.0f );

    if ( !qFuzzyIsNull( m_deltaCutoff ) )
        m_cutoffAngle += m_deltaCutoff;
    if ( !qFuzzyIsNull( m_deltaExponent ) )
        m_exponent += m_deltaExponent;
}

void SpotLightScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QOpenGLShaderProgramPtr shader = m_mesh->material()->shader();
    shader->bind();

    // Set up the view matrix
    QVector3D eye( 0.0f, 5.0f, -5.0f );
    QVector3D center( 0.0f, 0.0f, 0.0f );
    QVector3D up( 0.0f, 1.0f, 0.0f );
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt( eye, center, up );

    QMatrix4x4 modelViewMatrix = m_viewMatrix * m_modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_projectionMatrix * modelViewMatrix;
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_projectionMatrix );
    shader->setUniformValue( "mvp", mvp );

    // Convert light position and direction to eye coords
    QVector4D eyeLightPos = m_viewMatrix * m_worldLightPos;
    QMatrix4x4 normal4( m_viewMatrix.normalMatrix() );
    QVector3D worldLightDir( -m_worldLightPos );
    QVector3D eyeLightDir = normal4 * worldLightDir;
    eyeLightDir.normalize();

    // Set spotlight properties
    shader->setUniformValue( "light.position", eyeLightPos );
    shader->setUniformValue( "light.direction", eyeLightDir );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );
    shader->setUniformValue( "light.exponent", m_exponent );
    shader->setUniformValue( "light.cutoffAngle", m_cutoffAngle );

    // Set the material properties
    shader->setUniformValue( "material.kd", QVector3D( 0.9f, 0.5f, 0.3f ) );
    shader->setUniformValue( "material.ks", QVector3D( 0.95f, 0.95f, 0.95f ) );
    shader->setUniformValue( "material.ka", QVector3D( 0.2f, 0.2f, 0.2f ) );
    shader->setUniformValue( "material.shininess", 100.0f );

    // Let the mesh setup the remainder of its state and draw itself
    m_mesh->render();

    // Reset model matrix and render "floor"
    m_modelMatrix.setToIdentity();
    modelViewMatrix = m_viewMatrix * m_modelMatrix;
    normalMatrix = modelViewMatrix.normalMatrix();
    mvp = m_projectionMatrix * modelViewMatrix;
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_projectionMatrix );
    shader->setUniformValue( "mvp", mvp );

    shader->setUniformValue( "material.kd", QVector3D( 0.9f, 0.9f, 0.9f ) );
    shader->setUniformValue( "material.ks", QVector3D( 0.95f, 0.95f, 0.95f ) );
    shader->setUniformValue( "material.ka", QVector3D( 0.5f, 0.5f, 0.5f ) );
    shader->setUniformValue( "material.shininess", 10.0f );

    m_plane->render();
}

void SpotLightScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective( 60.0f, aspect, 0.3f, 1000.0f );
}
