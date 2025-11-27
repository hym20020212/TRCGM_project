#include "slotcaseview.h"
#include "slotitem.h"
#include <QVBoxLayout>
#include <QtMath>

SlotCaseView::SlotCaseView(QWidget *parent)
    : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

// void SlotCaseView::initSlots(int count)
// {
//     m_scene->clear();
//     m_items.clear();
//     m_data.clear();
//     m_data.resize(count);

//     for (int i = 0; i < count; ++i) {
//         SlotInfo s;
//         s.slotIndex = i;
//         s.type = SlotInfo::Free;
//         m_data[i] = s;

//         SlotItem *it = new SlotItem(s);
//         m_items.append(it);
//         m_scene->addItem(it);
//     }

//     layoutSlots();

//     // 发一次剩余数量
//     int freeCount = count;
//     emit freeSlotCountChanged(freeCount);
// }

void SlotCaseView::layoutSlots()
{
    if (m_items.isEmpty()) return;

    const int perRow = 20;   // 每行 20 个
    const int spacing = 28;  // 间距，像素
    for (int i = 0; i < m_items.size(); ++i) {
        int row = i / perRow;
        int col = i % perRow;
        m_items[i]->setPos(col * spacing, row * spacing);
    }

    int rows = (m_items.size() + perRow - 1) / perRow;
    QRectF rect(0, 0, perRow * spacing, rows * spacing);
    m_scene->setSceneRect(rect);
    fitInView(rect, Qt::KeepAspectRatio);
}

// void SlotCaseView::setSlotInfo(int index, const SlotInfo &info)
// {
//     if (index < 0 || index >= m_items.size()) return;
//     m_data[index] = info;

//     SlotItem *it = dynamic_cast<SlotItem*>(m_items[index]);
//     if (it) it->setInfo(info);

//     int freeCount = 0;
//     for (const SlotInfo &s : m_data) if (s.type == SlotInfo::Free) ++freeCount;
//     emit freeSlotCountChanged(freeCount);
// }

// void SlotCaseView::updateAllSlots(const QVector<SlotInfo> &list)
// {
//     int n = qMin(list.size(), m_items.size());
//     for (int i = 0; i < n; ++i) setSlotInfo(i, list[i]);
// }
