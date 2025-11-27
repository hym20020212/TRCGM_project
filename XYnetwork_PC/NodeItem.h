#ifndef NODEITEM_H
#define NODEITEM_H
#include <QFont>
#include <QGraphicsItemGroup>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>

class NodeItem : public QGraphicsItemGroup
{
public:
    NodeItem(const QString& name, double x, double y, double r)
    {
        m_circle = new QGraphicsEllipseItem(x-r, y-r, r*2, r*2);
        m_circle->setPen(QPen(Qt::black, 2));
        m_circle->setBrush(Qt::white);

        m_text = new QGraphicsTextItem(name);
        QFont f = m_text->font();
        f.setPointSize(14);
        m_text->setFont(f);
        m_text->setPos(x - 14, y - 12);

        addToGroup(m_circle);
        addToGroup(m_text);
    }

    void applyState(bool joined, bool isBase)
    {
        // 填充颜色
        m_circle->setBrush(joined ? Qt::green : Qt::red);

        // 基座蓝色边框
        if (isBase)
            m_circle->setPen(QPen(Qt::blue, 3));
        else
            m_circle->setPen(QPen(Qt::black, 2));
    }

private:
    QGraphicsEllipseItem *m_circle;
    QGraphicsTextItem *m_text;
};

#endif // NODEITEM_H
