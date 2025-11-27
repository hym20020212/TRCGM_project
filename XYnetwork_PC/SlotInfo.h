#ifndef SLOTINFO_H
#define SLOTINFO_H

#include <QString>

struct SlotInfo {
    int slotIndex = 0;            // 时隙编号 0..79
    enum Type { Free = 0, Tx = 1, Ranging = 2 } type = Free;
    QString fromNode;             // 发起节点（传输/测距）
    QString toNode;               // 目标节点（测距时使用）
};

#endif // SLOTINFO_H
