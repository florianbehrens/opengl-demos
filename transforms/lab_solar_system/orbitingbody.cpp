#include "orbitingbody.h"

#include <math.h>

OrbitingBody::OrbitingBody( QObject* parent )
    : QObject( parent ),
      m_localTransform(),
      m_worldTransform(),
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
    // TODO Reset the local transformation
    // TODO Apply transformations for planet size, orbital position and orbital inclination
    // Tip: Use modf, time and orbital period to calculate angular position around orbit
}

QMatrix4x4 OrbitingBody::worldTransform() const
{
    // TODO Initially, return the transform for this body only
    // TODO Later, once planets are working, extend to include parent body's transforms
    return m_worldTransform;
}
