#include "manylightsscene.h"

#include "axisalignedboundingbox.h"
#include "mesh.h"
#include "objloader.h"
#include "sphere.h"
#include "cube.h"
#include "plane.h"
#include "camera.h"

#include <QDateTime>
#include <QColor>

#include <math.h>


void Light::bindUniforms(QOpenGLShaderProgramPtr shader, QString prefix)
{
    
    QString positionName = prefix + ".positionOrNormal";
    shader->setUniformValue( positionName.toLatin1().data(), positionOrDirection );
    
    QString intensityName = prefix + ".intensity";
    shader->setUniformValue( intensityName.toLatin1().data(), intensity );

    QString cutoffName = prefix + ".cutoffDistance";
    shader->setUniformValue( cutoffName.toLatin1().data(), static_cast<GLfloat>(cutoffDistance) );
}

void SpotLight::bindUniforms(QOpenGLShaderProgramPtr shader, QString prefix,
    const QMatrix4x4& modelView)
{
    QVector4D eyePosition = modelView * position;
   
    QMatrix4x4 normal4( modelView.normalMatrix() );
    QVector3D worldLightDir( -position );
    QVector3D eyeDirection = normal4 * worldLightDir;
    eyeDirection.normalize();
    
    QString positionName = prefix + ".position";
    shader->setUniformValue( positionName.toLatin1().data(), eyePosition );
    
    QString directionName = prefix + ".direction";
    shader->setUniformValue( directionName.toLatin1().data(), eyeDirection );
    
    QString intensityName = prefix + ".intensity";
    shader->setUniformValue( intensityName.toLatin1().data(), intensity );

    QString cutoffAngleName = prefix + ".cutoffAngle";
    shader->setUniformValue( cutoffAngleName.toLatin1().data(), static_cast<GLfloat>(cutoffAngle) );
    
    QString cutoffExpName = prefix + ".exponent";
    shader->setUniformValue( cutoffExpName.toLatin1().data(), static_cast<GLfloat>(cutoffExponent) );
}

void FogInfo::bindUniforms(QOpenGLShaderProgramPtr shader)
{
    QString colorName = "fogColor";
    shader->setUniformValue( colorName.toLatin1().data(), color );
    
    QString maxDistName = "fogMaxDistance";
    shader->setUniformValue( maxDistName.toLatin1().data(), static_cast<GLfloat>(maxDistance) );

    QString minDistName = "fogMinDistance";
    shader->setUniformValue( minDistName.toLatin1().data(), static_cast<GLfloat>(minDistance) );
}

ManyLightsScene::ManyLightsScene( const QString& fileName )
    : m_fileName( fileName ),
      m_modelMatrix(),
      m_mesh( 0 ),
      m_theta( 0.0f )
{
    m_modelMatrix.setToIdentity();
}

void ManyLightsScene::initialise()
{
    // Load the model mesh
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled( false );
    loader.setTangentGenerationEnabled( false );
    loader.load( m_fileName );

    // Create a material
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/multiplelights.vert",
                          ":/shaders/multiplelights.frag" );

    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 5.0f, 0.0f, 5.0f ) );
    m_camera->setViewCenter( QVector3D( 3.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
    
    // Create a renderable object
    m_mesh = new Mesh;
    m_mesh->setMaterial( material );
    m_mesh->setMeshData( loader );


    m_cube = new Cube;
    m_cube->setMaterial( material );
    
    m_sphere = new Sphere;
    m_sphere->setRadius(0.2);
    m_sphere->setMaterial( material );
    m_sphere->create();
    
    m_ground = new Plane(20, 20, 10, 10);
    m_ground->setMaterial( material );
    m_ground->create();
    
    // Create some lighting parameters
    prepareLights(4, 2);

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void ManyLightsScene::update( float t )
{
    Q_UNUSED( t );

    m_theta += 0.4;
    if ( m_theta > 360.0f )
        m_theta -= 360.0f;

    // Get the maximum extent of the object so that we can scale it down to unit size
    float maxExtent = m_mesh->boundingVolume()->maxExtent();
    
    m_modelMatrix.setToIdentity();
    m_modelMatrix.scale( 1.0f / ( 1.2f * maxExtent ) );
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
}

// since spotlight uniforms depends on the modelview matrix, we need
// to recompute them for each mesh we draw. Wrap this up in
// a helper method
void ManyLightsScene::bindSpotLights(QOpenGLShaderProgramPtr shader, const QMatrix4x4& modelViewMatrix)
{
  shader->setUniformValue( "numSpotlights", m_spotLights.size() );
  for ( int i = 0; i < m_spotLights.size(); ++i )
  {
      m_spotLights[i].bindUniforms(shader, QString("spotlights[%1]").arg(i),
        modelViewMatrix);
  }
}

void ManyLightsScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QOpenGLShaderProgramPtr shader = m_mesh->material()->shader();
    shader->bind();

    // Set up the view matrix
    QVector3D eye( 0.0f, 0.3f, -4.0f );
    m_camera->setPosition(eye);

    QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * modelViewMatrix;
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_camera->viewProjectionMatrix() );
    shader->setUniformValue( "mvp", mvp );

    // Set the lighting parameters
    shader->setUniformValue( "numLights", m_lights.size() );
    for ( int i = 0; i < m_lights.size(); ++i )
    {
        m_lights[i].bindUniforms(shader, QString("lights[%1]").arg(i));
    }
    
    bindSpotLights(shader, modelViewMatrix);

    // Set the material properties
    shader->setUniformValue( "material.kd", QVector3D( 0.9f, 0.9f, 0.9f ) );
    shader->setUniformValue( "material.ks", QVector3D( 0.95f, 0.95f, 0.95f ) );
    shader->setUniformValue( "material.ka", QVector3D( 0.2f, 0.2f, 0.2f ) );
    shader->setUniformValue( "material.shininess", 20.0f );


    shader->setUniformValue( "fog.maxDistance", 10.0f );
    shader->setUniformValue( "fog.minDistance", 5.0f );
    shader->setUniformValue( "fog.color", QVector3D( 0.9f, 0.9f, 0.9f ) );
    
    m_fog.bindUniforms(shader);

    // Let the mesh setup the remainder of its state and draw itself
    m_mesh->render();     
    renderSphereGrid();
                
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(0, -2, 0);
    modelViewMatrix = m_camera->viewMatrix() * modelMatrix;
    normalMatrix = modelViewMatrix.normalMatrix();
    mvp = m_camera->viewProjectionMatrix() * modelViewMatrix;
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_camera->viewProjectionMatrix() );
    shader->setUniformValue( "mvp", mvp );

    bindSpotLights(shader, modelViewMatrix);
    
    m_ground->render();
}

void ManyLightsScene::renderSphereGrid()
{
    QOpenGLShaderProgramPtr shader = m_sphere->material()->shader();
    shader->setUniformValue( "material.ka", QVector3D( 0.2f, 0.2f, 0.2f ) );
    
    for (int i=-3; i<3; i++) {
        float kd = (i+3) / 6.0;
        shader->setUniformValue( "material.kd", QVector3D( kd, kd, kd ) );
        
        for (int j=-3; j<3; ++j) {
            float ks = (j+3) / 6.0;
            shader->setUniformValue( "material.ks", QVector3D( ks, ks, ks ) );
            
            //////////////////
            
            QMatrix4x4 modelMatrix;
            modelMatrix.setToIdentity();
            modelMatrix.translate(i * 1.2, -1, j * 1.2);
            
            QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * modelMatrix;
            QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
            QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * modelViewMatrix;
            shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
            shader->setUniformValue( "normalMatrix", normalMatrix );
            shader->setUniformValue( "projectionMatrix", m_camera->viewProjectionMatrix() );
            shader->setUniformValue( "mvp", mvp );
            
            //////////
            bindSpotLights(shader, modelViewMatrix);
            
            m_sphere->render();
        }
    }
}

double qdrand()
{
    return qrand() / static_cast<double>(RAND_MAX);
}

void ManyLightsScene::prepareLights(int numLights, int numSpots)
{
    const int r = 3.0f;
    for ( int i =0 ; i < numLights; ++i )
    {
        const float theta = qdrand() * 360.0f;
        const float phi = qdrand() * 90.0f;
        const float costheta = cosf( theta );
        const float sintheta = sinf( theta );
        const float cosphi = cosf( phi );

        float x = r * costheta * cosphi;
        float y = 1.7f;
        float z = 1.0f + r * sintheta * cosphi;

        // coin-toss; 50% chance of positional (w=1.0) or
        // directional (w=0.0)
        float w = (qdrand() > 0.5) ? 1.0 : 0.0f;

        Light light;
        light.positionOrDirection = QVector4D( x, y, z, w );
        
        QColor color;
        color.setHsvF(qdrand(), 0.5, 0.8  );
        color = color.toRgb();
        light.intensity = QVector3D(color.redF(), color.greenF(), color.blueF());
            
        m_lights.append( light );
    }
    
    for ( int i =0 ; i < numSpots; ++i )
    {
        const float theta = qdrand() * 360.0f;
        const float phi = qdrand() * 90.0f;
        const float costheta = cosf( theta );
        const float sintheta = sinf( theta );
        const float cosphi = cosf( phi );

        float x = r * costheta * cosphi;
        float y = 1.7f;
        float z = 1.0f + r * sintheta * cosphi;

        SpotLight light;
        light.position = QVector4D( x, y, z, 1.0 );
        light.cutoffExponent = 4.0;
        light.cutoffAngle = 30.0;
        
        QColor color;
        light.intensity = QVector3D(0.5, 0.5, 0.5);
            
        m_spotLights.append( light );
    }
}
