#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <QTcpSocket>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QVector>
#include "slotitem.h"
#include "topologyview.h"
#include "applyslot.h"
#include "replacechaindialog.h"

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
    // static int num_freeslot = 0 ;//动态时隙数量

private slots:
    void onActionConnectTriggered();
    void onActionMxModeTriggered();
    void onActionReplaceTriggered();
    void onActionSendDataTriggered();
    void onActionSelfworkTriggered();
    void onActionControlTriggered();
    void onActionLostcontrolTriggered();
    void onActionMeasureTriggered();
    void onActionBaseSetTriggered();
    void onActionJoinstartTriggered();
    void onActionQuitstartTriggered();
    void onActionApplyslotTriggered();
    void onApplyConfirmed(int transNum, int measureNum);
    void dealTcpData();
    void sendDataToNode(const QString &targetNode, const QString &payload);
    void on_senddataclear_pushButton_clicked();
    // void on_recvdataclear_pushButton_clicked();
    void on_recvdataclear_pushButton_clicked();
    void updateMeasureActionText();
    void on_netlunch_pushButton_clicked();
    void onReplaceChainConfirmed(const QString& tipText);

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
    QString tcpSendBuffer;
    QMap<int, bool> m_nodeRangingState; // 缓存节点测距状态：key=节点ID，value=是否测距
    QString m_currentNodeName; // 当前连接的节点名称（如MX/GZ1/GZ2）
    quint8 m_currentNodeId; // 当前连接的节点ID
    QTimer *m_idRequestTimer;// 身份索要定时器
    ReplaceChainDialog *replaceChainDialog = nullptr;

    int num_freeslot;//动态时隙数量
    bool isTcpConnected = false;
    bool isControl = false;
    bool isLoseControl = false;
    bool m_hasReceivedNodeId;  // 是否已收到节点身份信息

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
    void printsenddata(const QByteArray &data);
    void controlPermission(const QByteArray &dataBody);
    void parseBigNetSlotData(const QByteArray &dataBody);
    void recvdataFromNode(const QByteArray &dataBody);
    void processSlot0Data(const QByteArray &realDataBody);
    void processSlot1Data(const QByteArray &realDataBody);
    void processMain_TopologData(const QByteArray &realDataBody);
    void sendNodeIdRequest();

    static quint8 parse7bitNodeIdFromByte(quint8 stateByte);
    static QString getDeviceIdShowText(quint8 nodeId);

    // static const quint16 FRAME_HEAD = 0xAA55;    // 帧头标识
    // static const quint16 FRAME_TAIL = 0x55AA;    // 帧尾标识
    static const int FRAME_FIXED_LEN = 2 + 1; // 数据长度(2)+指令码(1) = 3字节（不含数据体）
    // 指令类型
    static const quint8 CMD_NODE_ID = 0x00;      // 下位机节点身份（节点→PC）
    static const quint8 CMD_NETWORK_START = 0x01;      // 1. 网络启动（PC→MX）
    static const quint8 CMD_NETWORK_AUTO_RUN = 0x02;   // 2. 网络自主运行（PC→MX）
    static const quint8 CMD_MX_RUN_MODE = 0x03;        // 3. MX运行模式（PC→MX）
    static const quint8 CMD_REPLACE_CHAIN = 0x04;      // 4. 替换链（PC→MX）
    static const quint8 CMD_SEND_DATA = 0x05;          // 5. 发送数据（PC→MX）
    static const quint8 CMD_BIG_NET_SLOT = 0x06;       // 6. 大网时隙0/1（MX→PC）
    static const quint8 CMD_SMALL_NET_MSG = 0x07;// 7. 小网遥测（MX→PC）
    static const quint8 CMD_RECVE_DATA = 0x08;       // 8. 接收数据（MX→PC）
    static const quint8 CMD_CONTROL_NET_IND = 0x09;    // 9. 控制网络指示（MX/GZ1/2→PC）
    static const quint8 CMD_CONTROL_NET_CONFIRM = 0x0A;// 10. 控制网络确认（PC→MX/GZ1/2）
    static const quint8 CMD_LOSECONTROL_IND = 0x0B;   // 11. 失去控制指示（MX/GZ1/2→PC）
    static const quint8 CMD_LOSE_CONTROL_CONFIRM = 0x0C;// 12. 失去控制确认（PC→MX/GZ1/2）
    static const quint8 CMD_CANCEL_RANGE = 0x0D;      // 13. 取消测距（PC→非MX节点）
    static const quint8 CMD_RESUME_RANGE = 0x0E;      // 14. 恢复测距（PC→非MX节点）
    static const quint8 CMD_APPLY_SLOT = 0x0F;        // 15. 申请时隙（PC→节点）
    static const quint8 CMD_CANCEL_SLOT = 0x10;       // 16. 取消时隙（PC→节点）
    static const quint8 CMD_NODE_JOIN_START = 0x11;   // 17. 节点入网启动（PC→节点）
    static const quint8 CMD_NODE_LEAVE_START = 0x12;  // 18. 节点退网启动（PC→节点）
    static const quint8 CMD_BASE_SET = 0x13;          // 19. 基座设置（PC→MX）

    static const quint8 MSG_TYPE_SLOT0 = 0x00; // 时隙0消息的消息类型
    static const quint8 MSG_TYPE_SLOT1 = 0x01;

    //节点ID映射表
    static QMap<quint8, QString> nodeIdToName() {
        QMap<quint8, QString> map;
        // 大网节点ID（7位，范围0~127）
        map.insert(0b0000000, "Ori_MX(000-0000)");       // MX (000-0000)
        map.insert(0b0010000, "Ori_GZ1(001-0000)");      // GZ1 (001-0000)
        map.insert(0b0010001, "Ori_GZ2(001-0001)");      // GZ2 (001-0001)
        map.insert(0b0100000, "Ori_BY1(010-0000)");      // BY1 (010-0000)
        map.insert(0b0100001, "Ori_BY2(010-0001)");      // BY2 (010-0001)
        map.insert(0b0100010, "Ori_BY3(010-0010)");      // BY3 (010-0010)
        map.insert(0b0100011, "Ori_BY4(010-0011)");      // BY4 (010-0011)
        map.insert(0b0100100, "Ori_BY5(010-0100)");      // BY5 (010-0100)
        map.insert(0b0100101, "Ori_BY6(010-0101)");      // BY6 (010-0101)
        map.insert(0b0110000, "Ori_ZJ0(011-0000)");      // ZJ0 (011-0000)
        map.insert(0b0110001, "Ori_ZJ1(011-0001)");      // ZJ1 (011-0001) 备用zj
        map.insert(0b0110010, "Ori_ZJ2(011-0010)");      // ZJ2 (011-0010)
        map.insert(0b0110011, "Ori_ZJ3(011-0011)");      // ZJ3 (011-0011) 备用zj
        map.insert(0b0001111, "广播");     // 广播 (000-1111)
        // 小网ZJ节点ID
        map.insert(0b1000000, "Ori_DJ1(100-0000)");
        map.insert(0b1000001, "Ori_DJ2(100-0001)");
        map.insert(0b1000010, "Ori_DJ3(100-0010)");
        map.insert(0b1000011, "Ori_DJ4(100-0011)");
        map.insert(0b1000100, "Ori_DJ5(100-0100)");
        map.insert(0b1000101, "Ori_DJ6(100-0101)");
        map.insert(0b1000110, "Ori_DJ7(100-0110)");
        map.insert(0b1000111, "Ori_DJ8(100-0111)");
        map.insert(0b1001000, "Ori_DJ9(100-1000)");
        map.insert(0b1001001, "Ori_DJ10(100-1001)");
        map.insert(0b1001010, "Ori_DJ11(100-1010)");
        map.insert(0b1001011, "Ori_DJ12(100-1011)");
        map.insert(0b1001100, "Ori_DJ13(100-1100)");
        map.insert(0b1001101, "Ori_DJ14(100-1101)");
        map.insert(0b1001110, "Ori_DJ15(100-1110)");
        map.insert(0b1001111, "Ori_DJ16(100-1111)");
        map.insert(0b1010000, "Ori_DJ17(101-0000)");
        map.insert(0b1010001, "Ori_DJ18(101-0001)");
        map.insert(0b1010010, "Ori_DJ19(101-0010)");
        map.insert(0b1010011, "Ori_DJ20(101-0011)");
        return map;
    }

    static QMap<quint8, QString> nodeIdToFunc() {
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
        map.insert(0b0110001, "ZJ1");      // ZJ1 (011-0001) 备用zj
        map.insert(0b0110010, "ZJ2");      // ZJ2 (011-0010)
        map.insert(0b0110011, "ZJ3");      // ZJ3 (011-0011) 备用zj
        map.insert(0b0001111, "广播");     // 广播 (000-1111)
        return map;
    }


protected:
    void resizeEvent(QResizeEvent *event) override;
};
#endif // MAINWINDOW_H

