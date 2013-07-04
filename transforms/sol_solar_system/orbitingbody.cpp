#include "orbitingbody.h"

#include <math.h>

OrbitingBody::OrbitingBody( QObject* parent )
    : QObject( parent ),
      m_localTransform(),
      m_bodyRadius( 1.0f ),
      m_color( QColor::fromRgbF( 0.7, 0.7, 0.7 ) ),
      m_orbitalRadius( 0.0f ),
      m_orbitalInclination( 0.0f ),
      m_orbitalPeriod( 10.0f ),
      m_orbitalPhase( 0.0f )
{
}

void OrbitingBody::setBodyRadius( float bodyRadius )
{
    m_bodyRadius = bodyRadius;
}

void OrbitingBody::setColor( QColor color )
{
    m_color = color;
}

QColor OrbitingBody::color() const
{
    return m_color;
}

void OrbitingBody::setOrbitalRadius( float orbitalRadius )
{
    m_orbitalRadius = orbitalRadius;
}

void OrbitingBody::setOrbitalInclination( float inclination )
{
    m_orbitalInclination = inclination;
}

void OrbitingBody::setOrbitalPeriod( float period )
{
    m_orbitalPeriod = period;
}

void OrbitingBody::setOrbitalPhase( float phase )
{
    m_orbitalPhase = phase;
}

void OrbitingBody::updateTransform( float time )
{
    // Reset the transformation
    m_localTransform.setToIdentity();

    // Rotate about z-axis to set orbital plane
    m_localTransform.rotate( m_orbitalInclination, 0.0f, 0.0f, 1.0f );

    // Rotate about y-axis to get to correct position
    double orbitNumber = 0;
    const float phase = modf( time / m_orbitalPeriod, &orbitNumber );
    float theta = 360.0f * m_orbitalPhase + 360.0f * phase;
    m_localTransform.rotate( theta, 0.0f, 1.0f, 0.0f );

    // Translate out to orbital radius
    m_localTransform.translate( m_orbitalRadius, 0.0f, 0.0f );

    // Scale the planet/moon to the correct size
    m_localTransform.scale( m_bodyRadius );
}

QMatrix4x4 OrbitingBody::worldTransform() const
{
    OrbitingBody* parentBody = qobject_cast<OrbitingBody*>( parent() );
    if ( parentBody )
        return parentBody->worldTransform() * m_localTransform;
    return m_localTransform;
}
