#ifndef TOPOLOGYVIEW_H
#define TOPOLOGYVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QResizeEvent>

class TopologyView : public QWidget
{
    Q_OBJECT
public:
    explicit TopologyView(QWidget *parent = nullptr);

    QGraphicsScene* scene() const;
    void drawTopology();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QGraphicsScene *m_scene;
    QGraphicsView  *m_view;

    // helper
    static QPointF ellipsePoint(double cx, double cy, double rx, double ry, double t);
    static void addNodeCircle(QGraphicsScene* s, double x, double y, const QString& id,
                              double r = 10, double dx = 12, double dy = -8);
    static void addLabeledCircle(QGraphicsScene* s, double x, double y, double r = 10);
};

#endif // TOPOLOGYVIEW_H
