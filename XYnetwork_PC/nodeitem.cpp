#include "nodeitem.h"
#include <QFont>

NodeItem::NodeItem(const QString& name,
                   const QPointF& pos,
                   double r,
                   QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent), m_name(name), m_deviceId("")
{
    setRect(pos.x()-r, pos.y()-r, r*2, r*2);
    setPen(QPen(Qt::black, 2));
    setBrush(Qt::white);

    // 身份文本
    m_nameText = new QGraphicsTextItem(name, this);
    QFont f; f.setPointSize(14);
    m_nameText->setFont(f);
    m_nameText->setPos(pos.x() - r/2 - 20, pos.y() - r/2 + 20);
    // ID文本（位于身份下方）
    m_idText = new DraggableTextItem(this);
    QFont idFont; idFont.setPointSize(12);
    m_idText->setFont(idFont);
    m_idText->setPos(pos.x() - r/2 - 20, pos.y() - r/2 + 40); // 位于身份文本下方
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

//设置设备ID（为空则隐藏）
void NodeItem::setDeviceId(const QString& id)
{
    m_deviceId = id;
    m_idText->setPlainText(id);
    m_idText->setVisible(!id.isEmpty()); // 空ID则隐藏
}
