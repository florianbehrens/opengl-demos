#include "objloader.h"

#include "mesh.h"
#include "axisalignedboundingbox.h"

#include <QDebug>
#include <QFile>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QTextStream>
#include <QVector>

ObjLoader::ObjLoader()
    : m_loadTextureCoords( true ),
      m_generateTangents( true ),
      m_centerMesh( false )
{
}

bool ObjLoader::load( const QString& fileName )
{
    qDebug() << Q_FUNC_INFO;

    int nFaces = 0;

    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        qDebug() << "Could not open file" << fileName << "for reading";
        return false;
    }

    QTextStream stream( &file );

    while ( !stream.atEnd() )
    {
        QString line = stream.readLine();
        line = line.simplified();

        if ( line.length() > 0 && line.at( 0 ) != '#' )
        {
            QTextStream lineStream( &line, QIODevice::ReadOnly );
            QString token;
            lineStream >> token;

            if (token == "v" )
            {
                float x, y, z;
                lineStream >> x >> y >> z;
                m_points.append( QVector3D( x, y, z ) );
            }
            else if ( token == "vt" && m_loadTextureCoords )
            {
                // Process texture coordinate
                float s,t;
                lineStream >> s >> t;
                m_texCoords.append( QVector2D( s, t ) );
            }
            else if ( token == "vn" )
            {
                float x, y, z;
                lineStream >> x >> y >> z;
                m_normals.append( QVector3D( x, y, z ) );
            }
            else if (token == "f" )
            {
                nFaces++;

                // Process face
                QVector<unsigned int> face;
                int slash1, slash2;
                while( !lineStream.atEnd() )
                {
                    QString vertString;
                    lineStream >> vertString;
                    int pIndex = -1, nIndex = -1, tcIndex = -1;

                    slash1 = vertString.indexOf( '/' );
                    if ( slash1 == -1 )
                    {
                        pIndex = vertString.toInt() - 1;
                    }
                    else
                    {
                        slash2 = vertString.indexOf( '/', slash1 + 1 );
                        pIndex = vertString.mid( 0, slash1 ).toInt() - 1;
                        if ( slash2 > slash1 + 1 )
                        {
                            tcIndex = vertString.mid( slash1 + 1, slash2 - slash1 ).toInt() - 1;
                        }
                        nIndex = vertString.mid( slash2 + 1, vertString.length() - slash2 ).toInt() - 1;
                    }

                    if ( pIndex == -1 )
                    {
                        qWarning( "Missing point index!!! " );
                    }
                    else
                    {
                        face.append( pIndex );
                    }

                    if ( m_loadTextureCoords && tcIndex != -1 && pIndex != tcIndex )
                    {
                        qWarning( "Texture and point indices are not consistent.\n");
                    }

                    if ( nIndex != -1 && nIndex != pIndex )
                    {
                        qWarning( "Normal and point indices are not consistent.\n" );
                    }
                }

                // If number of edges in face is greater than 3,
                // decompose into triangles as a triangle fan.
                if ( face.size() > 3 )
                {
                    unsigned int v0 = face[0];
                    unsigned int v1 = face[1];
                    unsigned int v2 = face[2];

                    // First face
                    m_faces.append( v0 );
                    m_faces.append( v1 );
                    m_faces.append( v2 );
                    for ( int i = 3; i < face.size(); ++i )
                    {
                        v1 = v2;
                        v2 = face[i];
                        m_faces.append( v0 );
                        m_faces.append( v1 );
                        m_faces.append( v2 );
                    }
                }
                else
                {
                    m_faces.append( face[0] );
                    m_faces.append( face[1] );
                    m_faces.append( face[2] );
                }
            } // end of face
        } // end of input line
    } // while( !stream.atEnd() )

    if ( m_normals.isEmpty() )
        generateAveragedNormals( m_points, m_normals, m_faces );

    if ( m_generateTangents && !m_texCoords.isEmpty() )
        generateTangents( m_points, m_normals, m_faces, m_texCoords, m_tangents );

    if ( m_centerMesh )
        center( m_points );

    qDebug() << "Loaded mesh from: " << fileName;
    qDebug() << " " << m_points.size() << "points";
    qDebug() << " " << nFaces << "faces";
    qDebug() << " " << m_faces.size() / 3 << "triangles.";
    qDebug() << " " << m_normals.size() << "normals";
    qDebug() << " " << m_tangents.size() << "tangents ";
    qDebug() << " " << m_texCoords.size() << "texture coordinates.";

    return true;
}

void ObjLoader::generateAveragedNormals( const QVector<QVector3D>& points,
                                         QVector<QVector3D>& normals,
                                         const QVector<unsigned int>& faces ) const
{
    for ( int i = 0; i < points.size(); ++i )
        normals.append( QVector3D() );

    for ( int i = 0; i < faces.size(); i += 3 )
    {
        const QVector3D& p1 = points[ faces[i]   ];
        const QVector3D& p2 = points[ faces[i+1] ];
        const QVector3D& p3 = points[ faces[i+2] ];

        QVector3D a = p2 - p1;
        QVector3D b = p3 - p1;
        QVector3D n = QVector3D::crossProduct( a, b ).normalized();

        normals[ faces[i]   ] += n;
        normals[ faces[i+1] ] += n;
        normals[ faces[i+2] ] += n;
    }

    for ( int i = 0; i < normals.size(); ++i )
        normals[i].normalize();
}

void ObjLoader::generateTangents( const QVector<QVector3D>& points,
                                  const QVector<QVector3D>& normals,
                                  const QVector<unsigned  int>& faces,
                                  const QVector<QVector2D>& texCoords,
                                  QVector<QVector4D>& tangents ) const
{
    tangents.clear();
    QVector<QVector3D> tan1Accum;
    QVector<QVector3D> tan2Accum;

    for ( int i = 0; i < points.size(); i++ )
    {
        tan1Accum.append( QVector3D() );
        tan2Accum.append( QVector3D() );
        tangents.append( QVector4D() );
    }

    // Compute the tangent vector
    for ( int i = 0; i < faces.size(); i += 3 )
    {
        const QVector3D& p1 = points[ faces[i] ];
        const QVector3D& p2 = points[ faces[i+1] ];
        const QVector3D& p3 = points[ faces[i+2] ];

        const QVector2D& tc1 = texCoords[ faces[i] ];
        const QVector2D& tc2 = texCoords[ faces[i+1] ];
        const QVector2D& tc3 = texCoords[ faces[i+2] ];

        QVector3D q1 = p2 - p1;
        QVector3D q2 = p3 - p1;
        float s1 = tc2.x() - tc1.x(), s2 = tc3.x() - tc1.x();
        float t1 = tc2.y() - tc1.y(), t2 = tc3.y() - tc1.y();
        float r = 1.0f / ( s1 * t2 - s2 * t1 );
        QVector3D tan1( ( t2 * q1.x() - t1 * q2.x() ) * r,
                        ( t2 * q1.y() - t1 * q2.y() ) * r,
                        ( t2 * q1.z() - t1 * q2.z() ) * r );
        QVector3D tan2( ( s1 * q2.x() - s2 * q1.x() ) * r,
                        ( s1 * q2.y() - s2 * q1.y() ) * r,
                        ( s1 * q2.z() - s2 * q1.z() ) * r );
        tan1Accum[ faces[i]   ] += tan1;
        tan1Accum[ faces[i+1] ] += tan1;
        tan1Accum[ faces[i+2] ] += tan1;
        tan2Accum[ faces[i]   ] += tan2;
        tan2Accum[ faces[i+1] ] += tan2;
        tan2Accum[ faces[i+2] ] += tan2;
    }

    for ( int i = 0; i < points.size(); ++i )
    {
        const QVector3D& n = normals[i];
        QVector3D& t1 = tan1Accum[i];
        QVector3D& t2 = tan2Accum[i];

        // Gram-Schmidt orthogonalize
        tangents[i] = QVector4D( QVector3D( t1 - QVector3D::dotProduct( n, t1 ) * n ).normalized(), 0.0f );

        // Store handedness in w
        tangents[i].setW( ( QVector3D::dotProduct( QVector3D::crossProduct( n, t1 ), t2 ) < 0.0f ) ? -1.0f : 1.0f );
    }
}

void ObjLoader::center( QVector<QVector3D>& points )
{
    if ( points.isEmpty() )
        return;

    AxisAlignedBoundingBox bb(points);
    QVector3D center = bb.center();

    // Translate center of the AABB to the origin
    for ( int i = 0; i < points.size(); ++i )
    {
        QVector3D& point = points[i];
        point = point - center;
    }
}
