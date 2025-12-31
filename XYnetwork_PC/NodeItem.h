#pragma once
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QObject>

class DraggableTextItem : public QGraphicsTextItem
{
public:
    DraggableTextItem(QGraphicsItem* parent = nullptr) : QGraphicsTextItem(parent) {
        setFlag(ItemIsMovable); // 允许移动
        setFlag(ItemIsSelectable); // 允许选中
    }

};

class NodeItem : public QGraphicsEllipseItem
{
public:
    enum State {
        NotJoined,   // 未入网
        Joined,      // 已入网
        Base         // 基座
    };

    NodeItem(const QString& name,
             const QPointF& pos,
             double r,
             QGraphicsItem* parent = nullptr);

    void setState(State s);
    State state() const { return m_state; }

    // 设置身份对应的设备ID
    void setDeviceId(const QString& id);

private:
    QString m_name; //身份（MX、GZ1）
    QString m_deviceId;   // 承担该身份的设备ID（如000-0000）
    State m_state;
    QGraphicsTextItem* m_nameText;  // 身份文本
    // QGraphicsTextItem* m_idText;    // ID文本（位于身份下方）
    DraggableTextItem* m_idText; // 可拖拽的ID文本
};
