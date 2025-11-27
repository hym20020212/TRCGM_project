#ifndef SLOTCASEVIEW_H
#define SLOTCASEVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVector>
#include "SlotInfo.h"

class SlotCaseView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit SlotCaseView(QWidget *parent = nullptr);

    // 初始化 N 个时隙（默认 80）
    void initSlots(int count = 80);

    // 单个时隙更新（外部调用）
    void setSlotInfo(int index, const SlotInfo &info);

    // 批量更新（一次性替换）
    void updateAllSlots(const QVector<SlotInfo> &list);

signals:
    void freeSlotCountChanged(int freeCount);

private:
    QGraphicsScene *m_scene;
    QVector<QGraphicsRectItem*> m_items; // 实际为 SlotItem*
    QVector<SlotInfo> m_data;

    void layoutSlots();
};

#endif // SLOTCASEVIEW_H
