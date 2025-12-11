#include "nodeitem.h"
#include <QFont>

NodeItem::NodeItem(const QString& name,
                   const QPointF& pos,
                   double r,
                   QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent), m_name(name)
{
    setRect(pos.x()-r, pos.y()-r, r*2, r*2);
    setPen(QPen(Qt::black, 2));
    setBrush(Qt::white);

    // 名称文本
    m_text = new QGraphicsTextItem(name, this);
    QFont f; f.setPointSize(14);
    m_text->setFont(f);
    m_text->setPos(pos.x() - r/2 , pos.y() - r/2 + 20);

    m_state = NotJoined;
}

void NodeItem::setState(State s)
{
    m_state = s;

    switch(s){
    case NotJoined:   setBrush(QBrush(Qt::red)); break;
    case Joined:      setBrush(QBrush(Qt::green)); break;
    case Base:        setBrush(QBrush(Qt::blue)); break;
    }
}
