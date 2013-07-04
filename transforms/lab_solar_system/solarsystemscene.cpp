#include "solarsystemscene.h"

#include "orbitingbody.h"

#include <camera.h>
#include <sphere.h>

#include <QGLWidget>
#include <QImage>

SolarSystemScene::SolarSystemScene( QObject* parent )
    : AbstractScene( parent ),
      m_camera( new Camera( this ) ),
      m_vx( 0.0f ),
      m_vy( 0.0f ),
      m_vz( 0.0f ),
      m_viewCenterFixed( false ),
      m_panAngle( 0.0f ),
      m_tiltAngle( 0.0f ),
      m_sphere( 0 ),
      m_animate( true ),
      m_time( 0.0f )
{
    // Initialize the camera position and orientation
    // TODO Set camera position to (0, 0, 60)
    // TODO Set camera to look at the origin with positive y-axis as "up" vector
}

void SolarSystemScene::initialise()
{
    // Create a material for the Sun
    m_sunMaterial = createSunMaterial();

    // Create a material for the planets
    MaterialPtr material = createPhongMaterial();

    // Create a sphere and set the material on it
    m_sphere = new Sphere( this );
    m_sphere->setMaterial( material );
    m_sphere->create();

    // Create some objects for our solar system
    createOrbitingBodies();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    // Set the clear color to (almost) white
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
}

void SolarSystemScene::createOrbitingBodies()
{
    // Create some "planets"
    OrbitingBody* redPlanet = new OrbitingBody( this );
    redPlanet->setBodyRadius( 0.5f );
    redPlanet->setColor( QColor::fromRgbF( 0.8f, 0.0f, 0.0f ) );
    redPlanet->setOrbitalRadius( 3.0f );
    redPlanet->setOrbitalPhase( 0.0f );
    redPlanet->setOrbitalPeriod( 1.5f );
    m_bodies.append( redPlanet );

    OrbitingBody* greenPlanet = new OrbitingBody( this );
    greenPlanet->setBodyRadius( 0.8f );
    greenPlanet->setColor( QColor::fromRgbF( 0.0f, 0.8f, 0.0f ) );
    greenPlanet->setOrbitalRadius( 8.0f );
    greenPlanet->setOrbitalPhase( 0.33f );
    greenPlanet->setOrbitalPeriod( 7.0f );
    m_bodies.append( greenPlanet );

    OrbitingBody* bluePlanet = new OrbitingBody( this );
    bluePlanet->setBodyRadius( 0.6f );
    bluePlanet->setColor( QColor::fromRgbF( 0.0f, 0.0f, 0.8f ) );
    bluePlanet->setOrbitalRadius( 15.0f );
    bluePlanet->setOrbitalPhase( 0.66f );
    bluePlanet->setOrbitalPeriod( 25.0f );
    m_bodies.append( bluePlanet );

    OrbitingBody* purplePlanet = new OrbitingBody( this );
    purplePlanet->setBodyRadius( 0.6f );
    purplePlanet->setColor( QColor::fromRgbF( 0.8f, 0.0f, 0.8f ) );
    purplePlanet->setOrbitalRadius( 12.0f );
    purplePlanet->setOrbitalPhase( 0.5f );
    purplePlanet->setOrbitalPeriod( 10.0f );
    purplePlanet->setOrbitalInclination( 10.0f );
    m_bodies.append( purplePlanet );

    // Create a moon as child of the red planet
    OrbitingBody* greyMoon = new OrbitingBody( greenPlanet );
    greyMoon->setBodyRadius( 0.3f );
    greyMoon->setColor( QColor::fromRgbF( 0.8f, 0.8f, 0.8f ) );
    greyMoon->setOrbitalRadius( 1.8f );
    greyMoon->setOrbitalPhase( 0.0f );
    greyMoon->setOrbitalPeriod( -2.0f );
    greyMoon->setOrbitalInclination( 0.0f );
    m_bodies.append(  greyMoon );

    // TODO Add some more planets/moons of your own
}

void SolarSystemScene::update( float t )
{
    // Store the current time so that we can feed it to the Sun shader
    m_time = t;

    // Advance the animation
    if ( m_animate )
    {
        // TODO Update the transformation of each planet/moon
    }

    m_modelMatrix.setToIdentity();
    m_modelMatrix.scale( 2.0f );

    // TODO Update the camera position and orientation
}

void SolarSystemScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //
    // Render the Sun
    //
    m_sunMaterial->bind();
    QOpenGLShaderProgramPtr shader = m_sunMaterial->shader();

    // Set the usual view/projection matrices
    QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * m_modelMatrix;
    shader->setUniformValue( "mvp", mvp );
    shader->setUniformValue( "time", m_time );

    // Let the mesh setup the remainder of its state and draw itself
    m_sphere->render();


    //
    // Render the planets and moons
    //

    // Setup the rendering pipeline - bind Phong lighting shader
    m_sphere->material()->bind();
    shader = m_sphere->material()->shader();

    // Set the lighting parameters
    // Light is at the origin in world coordinates to simulate the Sun
    // We need to convert this to eye coordinates which the shader expects
    QVector4D worldLightPos( 0.0f, 0.0f, 0.0f, 1.0f );
    QVector4D eyeLightPos = m_camera->viewMatrix() * worldLightPos;
    shader->setUniformValue( "light.position", eyeLightPos );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Iterate through the oribiting bodies, set uniforms and draw each one
    foreach ( OrbitingBody* body, m_bodies )
    {
        // TODO Get the world/model matrix for the planet/moon
        // TODO Construct modelView, normal, projection and mvp matrices
        // TODO Set matrices as uniform variables on the shader program

        setBodyColor( shader, body );

        m_sphere->render();
    }
}

void SolarSystemScene::setBodyColor( const QOpenGLShaderProgramPtr& shader,
                                     OrbitingBody* body ) const
{
    QColor c = body->color();
    QVector3D color( c.redF(), c.greenF(), c.blueF() );
    shader->setUniformValue( "material.kd", color );

    c = c.darker( 200 );
    QVector3D ambientColor( c.redF(), c.greenF(), c.blueF() );
    shader->setUniformValue( "material.ka", ambientColor );

    shader->setUniformValue( "material.ks", QVector3D( 0.4f, 0.4f, 0.4f ) );
    shader->setUniformValue( "material.shininess", 20.0f );
}

void SolarSystemScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 20.0f, aspect, 0.3, 1000.0f );
}

MaterialPtr SolarSystemScene::createPhongMaterial()
{
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/phong.vert",
                          ":/shaders/phong.frag" );
    return material;
}

MaterialPtr SolarSystemScene::createSunMaterial()
{
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/noise.vert",
                          ":/shaders/noise.frag" );
    return material;
}
