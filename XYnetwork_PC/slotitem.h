#ifndef SLOTITEM_H
#define SLOTITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneHoverEvent>

enum SlotType {
    SLOT_FREE = 0,
    SLOT_TX,
    SLOT_RANGE
};

class SlotItem : public QGraphicsEllipseItem
{
public:
    explicit SlotItem(int index, QGraphicsItem *parent = nullptr);

    void setSlotType(SlotType type);
    void setInfoText(const QString &info);

    int index() const { return m_index; }
    int slotType() const { return m_type; }

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    int m_index;
    SlotType m_type;
    QString m_info;
    void updateAppearance();
};

#endif // SLOTITEM_H
