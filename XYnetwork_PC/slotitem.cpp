#include "slotitem.h"
#include <QToolTip>
#include <QBrush>
#include <QPen>

SlotItem::SlotItem(int index, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent), m_index(index), m_type(SLOT_FREE)
{
    // default circle size 20x20; 位置由外部通过 setPos 设置
    setRect(0, 0, 20, 20);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable);
    updateAppearance();
}

void SlotItem::setSlotType(SlotType type)
{
    m_type = type;
    updateAppearance();
}

void SlotItem::setInfoText(const QString &info)
{
    m_info = info;
}

void SlotItem::updateAppearance()
{
    QBrush brush;
    switch (m_type) {
    case SLOT_FREE:  brush = QBrush(Qt::lightGray); break;
    case SLOT_TX:    brush = QBrush(Qt::green);     break;
    case SLOT_RANGE: brush = QBrush(Qt::blue);      break;
    default:         brush = QBrush(Qt::lightGray); break;
    }
    setBrush(brush);
    setPen(QPen(Qt::black, 1));
}

void SlotItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (!m_info.isEmpty())
        QToolTip::showText(event->screenPos(), m_info);
}

void SlotItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (!m_info.isEmpty())
        QToolTip::showText(event->screenPos(), m_info);
}

void SlotItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    QToolTip::hideText();
}
