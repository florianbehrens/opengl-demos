#ifndef AXIS_ALIGNED_BOUNDING_BOX
#define AXIS_ALIGNED_BOUNDING_BOX

#include <QVector>
#include <QVector3D>

class AxisAlignedBoundingBox
{
public:
    AxisAlignedBoundingBox();

    AxisAlignedBoundingBox(const QVector<QVector3D>& points);

    void update( const QVector<QVector3D>& points );

    QVector3D center() const { return m_center; }
    QVector3D radii() const { return m_radii; }

    QVector3D minPoint() const { return m_center - m_radii; }
    QVector3D maxPoint() const { return m_center + m_radii; }

    float xExtent() const { return 2.0f * m_radii.x(); }
    float yExtent() const { return 2.0f * m_radii.y(); }
    float zExtent() const { return 2.0f * m_radii.z(); }

    float maxExtent() const { return qMax( xExtent(), qMax( yExtent(), zExtent() ) ); }
    float minExtent() const { return qMin( xExtent(), qMin( yExtent(), zExtent() ) ); }

private:
    QVector3D m_center;
    QVector3D m_radii;
};

#endif // AXIS_ALIGNED_BOUNDING_BOX
