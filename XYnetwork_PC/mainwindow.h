#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <QTcpSocket>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QVector>
#include "slotitem.h"
#include "topologyview.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initUiConfig();
    void refreshTCPDisplay();//刷新 UI 的槽函数


private slots:
    void onActionConnectTriggered();
    void onActionMxModeTriggered();
    void onActionReplaceTriggered();
    void onActionSendDataTriggered();
    void dealTcpData();
    void sendDataToNode(const QString &targetNode, const QString &payload);
    void on_senddataclear_pushButton_clicked();
    // void on_recvdataclear_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    TopologyView *topologyView = nullptr;   // 拓扑图成员变量
    QGraphicsScene *slotScene0 = nullptr;
    QGraphicsScene *slotScene1 = nullptr;      // 为 slotcase1_View 准备的 scene
    QGraphicsScene *slotScene2 = nullptr;
    QVector<SlotItem*> slotItems0;
    QVector<SlotItem*> slotItems1;              // 存放 80 个 SlotItem 指针
    QVector<SlotItem*> slotItems2;
    QString tcpRecvBuffer;   // TCP 接收显示缓存
    bool isTcpConnected = false;

    bool connectToNode(const QString &ip, int port);
    void initSlotsSceneGeneric(QGraphicsScene *scene, QVector<SlotItem*> &items, QLabel *countLabel,
                               int perRow = 20, int totalSlots = 80);
    void updateSlotGeneric(QVector<SlotItem*> &items, QLabel *countLabel,
                           int idx, int typeCode, const QString &fromName = QString(), const QString &toName = QString());
    int countFreeSlotsGeneric(const QVector<SlotItem*> &items);
    void updateSlot(int netId, int idx, int typeCode,
                                const QString &fromName,
                                const QString &toName);
    void parseTcpProtocol(const QByteArray &buf);
    void parseBigNetSlotData(const QByteArray &dataBody);
    void processSlot0Data(const QByteArray &realDataBody);
    void processSlot1Data(const QByteArray &realDataBody);

    static const quint16 FRAME_HEAD = 0xAA55;    // 帧头标识
    static const quint16 FRAME_TAIL = 0x55AA;    // 帧尾标识
    static const int FRAME_FIXED_LEN = 2 + 2 + 1 + 2; // 帧头(2)+数据长度(2)+指令码(1)+帧尾(2) = 7字节（不含数据体、校验）
    static const quint8 CMD_BIG_NET_SLOT = 0x10; // 大网时隙0/1消息的指令码（对应第6个接口）
    static const quint8 MSG_TYPE_SLOT0 = 0x00; // 时隙0消息的消息类型
    static const quint8 MSG_TYPE_SLOT1 = 0x01;

    //节点ID映射表
    static QMap<quint8, QString> nodeIdToName() {
        QMap<quint8, QString> map;
        // 大网节点ID（7位，范围0~127）
        map.insert(0b0000000, "MX");       // MX (000-0000)
        map.insert(0b0010000, "GZ1");      // GZ1 (001-0000)
        map.insert(0b0010001, "GZ2");      // GZ2 (001-0001)
        map.insert(0b0100000, "BY1");      // BY1 (010-0000)
        map.insert(0b0100001, "BY2");      // BY2 (010-0001)
        map.insert(0b0100010, "BY3");      // BY3 (010-0010)
        map.insert(0b0100011, "BY4");      // BY4 (010-0011)
        map.insert(0b0100100, "BY5");      // BY5 (010-0100)
        map.insert(0b0100101, "BY6");      // BY6 (010-0101)
        map.insert(0b0110000, "ZJ0");      // ZJ0 (011-0000)
        map.insert(0b0110001, "ZJ1");      // ZJ1 (011-0001)
        map.insert(0b0110010, "ZJ2");      // ZJ2 (011-0010)
        map.insert(0b0110011, "ZJ3");      // ZJ3 (011-0011)
        map.insert(0b0001111, "广播");     // 广播 (000-1111)
        return map;
    }

protected:
    void resizeEvent(QResizeEvent *event) override;
};
#endif // MAINWINDOW_H

