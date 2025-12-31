#ifndef TOPOLOGYVIEW_H
#define TOPOLOGYVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QResizeEvent>
#include "nodeitem.h"

class TopologyView : public QWidget
{
    Q_OBJECT
public:
    explicit TopologyView(QWidget *parent = nullptr);

    QGraphicsScene* scene() const;
    void drawTopology();
    //节点对象化
    void updateNodeState(const QString& nodeName, int slot);
    void updateNodeDeviceId(const QString& identityName, const QString& deviceId);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QGraphicsScene *m_scene;
    QGraphicsView  *m_view;
    QHBoxLayout *m_legendLayout = nullptr;   // 图例布局
    //节点对象化
    QMap<QString, NodeItem*> m_nodes;

    void setupLegend();

    // helper
    static QPointF ellipsePoint(double cx, double cy, double rx, double ry, double t);
    // static void addNodeCircle(QGraphicsScene* s, double x, double y, const QString& id,
    //                           double r = 10, double dx = 12, double dy = -8);
    void addNode(const QString& name, double x, double y);
    static void addLabeledCircle(QGraphicsScene* s, double x, double y, double r = 10);
};

#endif // TOPOLOGYVIEW_H
