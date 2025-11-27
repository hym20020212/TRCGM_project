#ifndef NODEINFO_H
#define NODEINFO_H

#include <QString>
#include <stdint.h>

struct NodeInfo
{
    QString name;
    uint8_t rawId;

    bool isSmallNet;   // bit7
    int  number;       // bit6~bit1
    bool joined;       // bit0
    bool isBase;       // 是否基座

    static NodeInfo fromRaw(const QString& name, uint8_t raw)
    {
        NodeInfo n;
        n.name = name;
        n.rawId = raw;

        n.isSmallNet = (raw & 0x80) != 0;
        n.number = (raw >> 1) & 0x3F;
        n.joined = (raw & 0x01);

        //  MX、GZ1、GZ2 为基座
        n.isBase = (name == "MX" || name == "GZ1" || name == "GZ2");
        return n;
    }
};

#endif // NODEINFO_H
