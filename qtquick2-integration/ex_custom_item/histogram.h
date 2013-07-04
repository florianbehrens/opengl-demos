#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QQuickItem>

class AbstractScene;

class Histogram : public QQuickItem
{
    Q_OBJECT

public:
    explicit Histogram( QQuickItem* parent = 0 );
    ~Histogram();

protected:
    QSGNode *updatePaintNode( QSGNode*, UpdatePaintNodeData* );

private:
    AbstractScene* m_scene;
};

#endif // HISTOGRAM_H
