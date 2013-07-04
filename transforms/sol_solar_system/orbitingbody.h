#ifndef ORBITINGBODY_H
#define ORBITINGBODY_H

#include <QObject>

#include <QColor>
#include <QMatrix4x4>

class OrbitingBody : public QObject
{
    Q_OBJECT

public:
    explicit OrbitingBody( QObject* parent = 0 );

    // Planet/moon properties
    void setBodyRadius( float bodyRadius );
    void setColor( QColor color );
    QColor color() const;

    // Orbital properties
    void setOrbitalRadius( float orbitRadius );
    void setOrbitalInclination( float inclination );
    void setOrbitalPeriod( float period );
    void setOrbitalPhase( float phase );

    // Transform functions
    void updateTransform( float time );
    QMatrix4x4 worldTransform() const;
    
private:
    QMatrix4x4 m_localTransform;

    float m_bodyRadius;
    QColor m_color;

    float m_orbitalRadius;
    float m_orbitalInclination;
    float m_orbitalPeriod;
    float m_orbitalPhase;
};

#endif // ORBITINGBODY_H
