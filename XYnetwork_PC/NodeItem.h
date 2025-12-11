#pragma once
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>

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

private:
    QString m_name;
    State m_state;
    QGraphicsTextItem* m_text;
};
