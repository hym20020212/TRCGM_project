#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include "mxmodedialog.h"
#include "replacechaindialog.h"
#include <QTimer>
#include <QDateTime>
#include "slotitem.h"
#include "topologyview.h"
#include "senddata.h"
#include <QButtonGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);
    connect(ui->actionConnect, &QAction::triggered,
            this, &MainWindow::onActionConnectTriggered);

    connect(ui->actionMxMode, &QAction::triggered,
            this, &MainWindow::onActionMxModeTriggered);
    connect(ui->actionReplace, &QAction::triggered,
            this, &MainWindow::onActionReplaceTriggered);
    connect(tcpSocket, &QTcpSocket::readyRead,
            this, &MainWindow::dealTcpData);
    connect(ui->actionSendData, &QAction::triggered,
            this, &MainWindow::onActionSendDataTriggered);
    connect(tcpSocket, &QTcpSocket::disconnected, this, [this](){
        isTcpConnected = false;
        ui->actionConnect->setText("连接节点");
        ui->tip_textEdit->append("TCP 连接异常断开");
    });

    initUiConfig();


    // 延迟 0ms 调用 initSlotsScene，保证 UI 完成布局后 fitInView 生效
    QTimer::singleShot(0, this, [this](){
        initSlotsSceneGeneric(slotScene0, slotItems0, ui->restslot_mainlabel, 20, 80);
        initSlotsSceneGeneric(slotScene1, slotItems1, ui->restslot_1label, 20, 80);
        initSlotsSceneGeneric(slotScene2, slotItems2, ui->restslot_2label, 20, 80);


        // //时隙状态测试：
        // updateSlot(0, 25, 1, "GZ1", "");
        // updateSlot(0, 21, 2, "BY5", "GZ2");
        // updateSlot(1, 5, 1, "DJ1", "");
        // updateSlot(1, 6, 1, "DJ2", "");
        // updateSlot(2, 10, 1, "DJ6", "");
        // updateSlot(2, 11, 1, "DJ7", "");
        //节点状态测试：
        topologyView->updateNodeState("DJ3", 1); // 已入网
        topologyView->updateNodeState("GZ1", 2); // 基座
        topologyView->updateNodeState("BY2", 0); // 未入网


        // 把 scene 缩放到各自 view 中（自适应）
        ui->slotcasemain_View->fitInView(slotScene0->sceneRect(), Qt::KeepAspectRatio);
        ui->slotcase1_View->fitInView(slotScene1->sceneRect(), Qt::KeepAspectRatio);
        ui->slotcase2_View->fitInView(slotScene2->sceneRect(), Qt::KeepAspectRatio);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUiConfig()
{
    //设置只读
    ui->recvlogTextEdit->setReadOnly(true);
    ui->tip_textEdit->setReadOnly(true);
    //拓扑图初始化
    QVBoxLayout *lay = new QVBoxLayout(ui->netTopologyView);
    lay->setContentsMargins(0,0,0,0);
    ui->netTopologyView->setLayout(lay);
    topologyView = new TopologyView(this);
    lay->addWidget(topologyView);
    topologyView->drawTopology();
    //时隙图初始化
    slotScene0 = new QGraphicsScene(this);
    slotScene1 = new QGraphicsScene(this);
    slotScene2 = new QGraphicsScene(this);

    ui->slotcasemain_View->setScene(slotScene0);
    ui->slotcase1_View->setScene(slotScene1);
    ui->slotcase2_View->setScene(slotScene2);

    ui->slotcasemain_View->setRenderHint(QPainter::Antialiasing);
    ui->slotcase1_View->setRenderHint(QPainter::Antialiasing);
    ui->slotcase2_View->setRenderHint(QPainter::Antialiasing);
    ui->slotcasemain_View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->slotcasemain_View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->slotcase1_View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->slotcase1_View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->slotcase2_View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->slotcase2_View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //设置为可多选
    QButtonGroup *groupbtn = new QButtonGroup;
    groupbtn->addButton(ui->checkBoxHexR,0);
    groupbtn->addButton(ui->RecTimeZone,1);
    groupbtn->addButton(ui->checkBoxHexS,2);
    groupbtn->addButton(ui->SendTimeZone,3);
    groupbtn->setExclusive(false);
    ui->checkBoxHexR->setChecked(1);
    ui->RecTimeZone->setChecked(1);
    ui->checkBoxHexS->setChecked(1);
    ui->SendTimeZone->setChecked(1);

    tcpRecvBuffer = "";
}

void MainWindow::onActionConnectTriggered()
{
    if (isTcpConnected)
    {
        if (tcpSocket->isOpen())
        {
            tcpSocket->close(); // 关闭TCP连接
            isTcpConnected = false; // 更新状态标记
            // 修改Action文本为“连接”
            ui->actionConnect->setText("连接节点");
            ui->tip_textEdit->append("TCP 连接已断开");
        }
        return;
    }

    ConnectDialog dialog(this);
    // 阻塞式显示对话框
    if (dialog.exec() == QDialog::Accepted)
    {
        QString ip = dialog.getIP();
        int port = dialog.getPort();

        bool ok = connectToNode(ip, port);

        if (ok)
        {
            isTcpConnected = true; // 更新状态标记为已连接
            ui->actionConnect->setText("断开连接"); // 修改Action文本
            ui->tip_textEdit->append("连接成功!");
        }
        else
            ui->tip_textEdit->append("连接失败!");
    }
}

bool MainWindow::connectToNode(const QString &ip, int port)
{
    if (tcpSocket->isOpen())
        tcpSocket->close();

    tcpSocket->connectToHost(ip, port);

    // 等待 3 秒建立连接
    if (tcpSocket->waitForConnected(3000))
    {
        ui->tip_textEdit->append("TCP 连接成功: " + ip + ":" + QString::number(port));
        isTcpConnected = true;
        return true;
    }
    else
    {
        ui->tip_textEdit->append("TCP 连接失败: " + tcpSocket->errorString());
        isTcpConnected = false;
        return false;
    }
}

//更新数据输入
void MainWindow::refreshTCPDisplay()
{

    ui->recvlogTextEdit->setPlainText(tcpRecvBuffer);
    QTextCursor cursor = ui->recvlogTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End); // 光标移到末尾
    ui->recvlogTextEdit->setTextCursor(cursor); // 应用光标位置
    ui->recvlogTextEdit->ensureCursorVisible(); // 确保末尾内容可见

}

//TCP接收数据
void MainWindow::dealTcpData()
{
    QByteArray buffer = tcpSocket->readAll();
    if (buffer.isEmpty()) return;

    QString str_rev;  // 最终显示到文本框的内容

    //字符串解析：Hex 模式orASCII 模式
    if (!ui->checkBoxHexR->isChecked())
    {
        // 直接按字符串显示（ASCII）
        str_rev = QString::fromUtf8(buffer);
    }
    else
    {
        // Hex 显示
        QString hexStr = buffer.toHex().toUpper();
        // 预分配内存避免崩溃
        QString spaced;
        spaced.reserve(buffer.size() * 3);
        for (int i = 0; i < hexStr.length(); i += 2)
        {
            spaced += hexStr.mid(i, 2);
            spaced += " ";
        }
        str_rev = spaced;
    }

    //添加时间戳
    if (ui->RecTimeZone->isChecked())
    {
        QDateTime now = QDateTime::currentDateTime();
        QString timePrefix = "[" + now.toString("yyyy-MM-dd hh:mm:ss") + "]\n";
        str_rev = timePrefix + str_rev;
    }

    // 换行
    str_rev += "\r\n";

    //追加到缓冲区（为了不让UI崩溃，需要限制长度）
    tcpRecvBuffer += str_rev;

    // 限制最大显示长度（避免UI卡死）
    if (tcpRecvBuffer.length() > 100000)  // 100KB
    {
        tcpRecvBuffer = tcpRecvBuffer.right(100000);
    }

    //调用协议解析
    parseTcpProtocol(buffer);
    refreshTCPDisplay();

}

//数据解析
void MainWindow::parseTcpProtocol(const QByteArray &buffer)
{
    // 基础校验：帧长度至少满足“固定部分+最小数据体（0字节）+校验（1字节）”
    if (buffer.size() < (FRAME_FIXED_LEN + 1)) { // 7+1=8字节（最小帧长度）
        qDebug() << "帧长度不足：实际" << buffer.size() << "字节，最小需8字节";
        return;
    }
    //1. 解析并校验帧头
    quint16 frameHead = (static_cast<quint8>(buffer.at(0)) << 8) | static_cast<quint8>(buffer.at(1));
    if (frameHead != FRAME_HEAD) {
        qDebug() << "帧头错误：收到0x" << QString::number(frameHead, 16).rightJustified(4, '0')
            << "，期望0xAA55";
        return;
    }
    //2. 解析数据长度（后续数据体的字节数）
    quint16 dataLen = (static_cast<quint8>(buffer.at(2)) << 8) | static_cast<quint8>(buffer.at(3));
    // 校验：实际帧长度应 = 固定部分（7） + 数据体长度（dataLen） + 校验位长度（假设1字节，若用CRC16则2字节）
    const int CHECK_LEN = 1; // 若下位机用CRC16，这里改为2
    int expectedFrameLen = FRAME_FIXED_LEN + dataLen + CHECK_LEN;
    if (buffer.size() != expectedFrameLen) {
        qDebug() << "帧长度不匹配：数据体声明" << dataLen << "字节，实际帧长度" << buffer.size()
                 << "，期望" << expectedFrameLen;
        return;
    }
    //3. 解析指令码（第4字节，即buffer[4]）
    quint8 cmdCode = static_cast<quint8>(buffer.at(4));
    //4. 解析数据体（从第5字节开始，长度为dataLen）
    QByteArray dataBody = buffer.mid(5, dataLen);
    //5. 校验位（可选，这里简化为“跳过”，实际需实现CRC/校验和）
    //6. 校验帧尾
    quint16 frameTail = (static_cast<quint8>(buffer.at(5 + dataLen + CHECK_LEN)) << 8)
                        | static_cast<quint8>(buffer.at(5 + dataLen + CHECK_LEN + 1));
    if (frameTail != FRAME_TAIL) {
        qDebug() << "帧尾错误：收到0x" << QString::number(frameTail, 16).rightJustified(4, '0')
            << "，期望0x55AA";
        return;
    }

    switch (cmdCode) {
        case CMD_BIG_NET_SLOT: { // 大网时隙0/1消息
        qDebug() << "收到大网时隙消息：数据体长度" << dataBody.size() << "字节";
        parseBigNetSlotData(dataBody); // 子函数（你已有，入参是数据体）
        break;
    }
    default: {
        qDebug() << "未知指令码：0x" << QString::number(cmdCode, 16).rightJustified(2, '0');
        break;
    }
    }
}


void MainWindow::onActionMxModeTriggered()
{
    MXmodeDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString mode = dialog.selectedMode();
        ui->tip_textEdit->append("进入" + mode);
    }
}

void MainWindow::onActionReplaceTriggered()
{
    ReplaceChainDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        int chainLevel = dialog.selectedChain();
        ui->tip_textEdit->append("当前为 " + QString::number(chainLevel) + " 级替换链");
    }
}

void MainWindow::initSlotsSceneGeneric(QGraphicsScene *scene,
                                       QVector<SlotItem*> &items,
                                       QLabel *countLabel,
                                       int perRow,
                                       int totalSlots)
{
    if (!scene) return;

    // 清理
    scene->clear();
    for (SlotItem *it : items) { delete it; }
    items.clear();

    const int itemW = 20;
    const int itemH = 20;
    const int spacingX = 8;
    const int spacingY = 14;

    int cols = perRow;
    int rows = (totalSlots + perRow - 1) / perRow;
    int sceneW = cols * (itemW + spacingX);
    int sceneH = rows * (itemH + spacingY);

    scene->setSceneRect(0, 0, sceneW, sceneH);

    for (int i = 0; i < totalSlots; ++i) {
        SlotItem *it = new SlotItem(i);
        int row = i / perRow;
        int col = i % perRow;
        int offsetY = 30;
        qreal x = col * (itemW + spacingX);
        qreal y = row * (itemH + spacingY) + offsetY;

        it->setPos(x, y);
        it->setZValue(1.0);

        it->setInfoText(QString("时隙 %1\n状态：未分配").arg(i));
        it->setSlotType(SLOT_FREE);

        // 编号文本
        QGraphicsTextItem *num = new QGraphicsTextItem(QString::number(i));
        num->setDefaultTextColor(Qt::black);
        num->setFont(QFont("Microsoft Yahei", 8));
        scene->addItem(num);
        num->setPos(x + (itemW - num->boundingRect().width())/2.0,
                    y + itemH - 4);
        num->setZValue(2.0);

        items.append(it);
        scene->addItem(it);
    }

    // 图例（放到场景底部）
    int legendY = sceneH + 50;
    int legendR = 7;
    auto drawLegend = [&](int x, QColor color, const QString &text){
        scene->addEllipse(x, legendY, legendR*2, legendR*2,
                          QPen(Qt::black), QBrush(color));
        QGraphicsTextItem *t = scene->addText(text);
        t->setDefaultTextColor(Qt::black);
        t->setFont(QFont("Microsoft Yahei", 12));
        t->setPos(x + legendR*2 + 5, legendY - 4);
    };

    drawLegend(10,  Qt::green,     "传输");
    drawLegend(110, Qt::blue,      "测距");
    drawLegend(210, Qt::lightGray, "未分配");

    scene->setSceneRect(0, 0, sceneW, legendY + 40);

    // 显示初始剩余计数
    if (countLabel) countLabel->setText(QString::number(totalSlots));
}

int MainWindow::countFreeSlotsGeneric(const QVector<SlotItem*> &items)
{
    int cnt = 0;
    for (SlotItem *it : items) {
        if (!it) continue;
        if (it->slotType() == SLOT_FREE) cnt++;
    }
    return cnt;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (slotScene0 && ui->slotcasemain_View->scene()) {
        ui->slotcasemain_View->fitInView(slotScene0->sceneRect(), Qt::KeepAspectRatio);
    }
    if (slotScene1 && ui->slotcase1_View->scene()) {
        ui->slotcase1_View->fitInView(slotScene1->sceneRect(), Qt::KeepAspectRatio);
    }
    if (slotScene2 && ui->slotcase2_View->scene()) {
        ui->slotcase2_View->fitInView(slotScene2->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::updateSlotGeneric(QVector<SlotItem*> &items,
                                   QLabel *countLabel,
                                   int idx, int typeCode,
                                   const QString &fromName,
                                   const QString &toName)
{

    if (idx < 0 || idx >= items.size()) return;
    SlotItem *it = items[idx];
    if (!it) return;

    if (typeCode == 0) { // 未分配
        it->setSlotType(SLOT_FREE);
        it->setInfoText(QString("时隙 %1\n状态：未分配").arg(idx));
    } else if (typeCode == 1) { // 传输
        it->setSlotType(SLOT_TX);
        it->setInfoText(QString("时隙 %1\n状态：传输\n发起：%2").arg(idx).arg(fromName));
    } else if (typeCode == 2) { // 测距
        it->setSlotType(SLOT_RANGE);
        it->setInfoText(QString("时隙 %1\n状态：测距\n发起：%2\n目标：%3").arg(idx).arg(fromName).arg(toName));
    }

    // 更新计数
    if (countLabel) {
        int freeCnt = countFreeSlotsGeneric(items);
        countLabel->setText(QString::number(freeCnt));
    }
}

// idx/type/from/to 都来自协议解析，接口 netId 表示“哪个小网”，可以按接收到的消息来决定 netId 是 1 还是 2。
void MainWindow::updateSlot(int netId, int idx, int typeCode, const QString &fromName, const QString &toName)
{
    if (netId == 1) {
        updateSlotGeneric(slotItems1, ui->restslot_1label, idx, typeCode, fromName, toName);
    } else if (netId == 2) {
        updateSlotGeneric(slotItems2, ui->restslot_2label, idx, typeCode, fromName, toName);
    }else if (netId == 0) {
        updateSlotGeneric(slotItems0, ui->restslot_mainlabel, idx, typeCode, fromName, toName);
    }
}

void MainWindow::onActionSendDataTriggered()
{
    SendData dialog(this);
    // 将 SendData 的信号连接到 MainWindow 的槽
    connect(&dialog, &SendData::sendDataRequest,
            this, &MainWindow::sendDataToNode);
    dialog.exec();
}

void MainWindow::sendDataToNode(const QString &targetNode, const QString &payload)
{
    if (!tcpSocket || !tcpSocket->isOpen()) {
        ui->tip_textEdit->append("TCP 未连接，无法发送数据");
        return;
    }

    /* ----------- 构建发送数据格式（示例）----------- */
    // 这里你可以按你的协议设计格式
    QByteArray packet;
    packet.append(targetNode.toUtf8());
    packet.append(" | ");
    packet.append(payload.toUtf8());
    packet.append("\n");

    /* ----------- TCP 发送 ----------- */
    tcpSocket->write(packet);
    tcpSocket->flush();

    /* ----------- 更新主界面发送日志 ----------- */
    QString displayPayload;
    if (ui->checkBoxHexS->isChecked()) {
        QByteArray payloadBytes = payload.toUtf8();
        // 转为纯大写Hex字符串（无空格）
        QString hexStr = payloadBytes.toHex().toUpper();
        // 预分配内存：每个字节占2位 + 1个空格，最后一个字节无空格，故总长度为 字节数*3 -1
        displayPayload.reserve(payloadBytes.size() * 3 - 1);
        // 逐2位拼接并添加空格（和接收端逻辑一致）
        for (int i = 0; i < hexStr.length(); i += 2) {
            displayPayload += hexStr.mid(i, 2);
            // 最后一组不添加空格，避免末尾多余空格
            if (i + 2 < hexStr.length()) {
                displayPayload += " ";
            }
        }
    } else {
        displayPayload = payload;
    }
    QString timeStr;
    if (ui->SendTimeZone->isChecked()) {
        timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    QString log;
    if (!timeStr.isEmpty()) {
        log = QString("[%1] 发送到 %2 :\n%3")
                  .arg(timeStr)
                  .arg(targetNode)
                  .arg(displayPayload);
    } else {
        log = QString("发送到 %1 :\n%2")
                  .arg(targetNode)
                  .arg(displayPayload);
    }
    ui->sendlogTextEdit->appendPlainText(log);
}

void MainWindow::on_senddataclear_pushButton_clicked()
{
    ui->sendlogTextEdit->clear();
}

void MainWindow::parseBigNetSlotData(const QByteArray &dataBody)
{
    //1. 基础校验：数据体至少包含前4个固定字段
    // 前4字段：发送节点(1Byte) + 接收节点(1Byte) + 消息类型(1Byte) + 消息长度(1Byte)
    const int FIXED_HEAD_LEN = 4; // 固定头部长度（前4字节）
    if (dataBody.size() < FIXED_HEAD_LEN) {
        qDebug() << "[时隙0/1解析] 数据体过短，无法解析核心字段，当前长度：" << dataBody.size() << "字节（需≥4字节）";
        return;
    }

    //2. 解析核心字段
    quint8 msgType = static_cast<quint8>(dataBody.at(2));  // 消息类型（第3字节，索引2）
    quint8 msgLenField = static_cast<quint8>(dataBody.at(3)); // 消息长度字段（第4字节，索引3）：后续数据体字节数（理论94）

    //3. 区分时隙0/1消息类型
    QString slotType;
    bool isSlot0Or1 = true;
    if (msgType == MSG_TYPE_SLOT0) {
        slotType = "时隙0";
    } else if (msgType == MSG_TYPE_SLOT1) {
        slotType = "时隙1";
    } else {
        slotType = "未知类型";
        isSlot0Or1 = false;
    }
    qDebug() << "[时隙0/1解析] 识别到" << slotType << "消息，消息长度字段值：" << static_cast<int>(msgLenField);

    //4. 非时隙0/1消息直接返回
    if (!isSlot0Or1) {
        qDebug() << "[时隙0/1解析] 非时隙0/1消息，终止解析";
        return;
    }

    //5. 长度校验：总长度 = 消息长度字段值 + 4（固定头部）
    int expectedTotalLen = static_cast<int>(msgLenField) + FIXED_HEAD_LEN;
    if (dataBody.size() != expectedTotalLen) {
        qDebug() << "[时隙0/1解析] 长度校验失败！"
                 << "预期总长度：" << expectedTotalLen << "（消息长度" << static_cast<int>(msgLenField) << "+固定头部4）"
                 << "实际总长度：" << dataBody.size();
        return;
    }

    //6. 长度校验通过（后续调用你的数据处理函数）
    qDebug() << "[时隙0/1解析] " << slotType << "消息长度校验通过！";
    QByteArray realDataBody = dataBody.mid(FIXED_HEAD_LEN); // 提取后续真实数据体（跳过前4字节固定头部）

    // 调用你已有的数据处理函数（按需替换为你的函数名）
    if (msgType == MSG_TYPE_SLOT0) {
        processSlot0Data(realDataBody); // 时隙0数据处理
    } else {
        processSlot1Data(realDataBody); // 时隙1数据处理
    }
}

void MainWindow::processSlot0Data(const QByteArray &realDataBody)
{
    // 时隙0消息的realDataBody是时隙0~46的配置（共47个时隙，每个2字节，总94字节）
    const int SLOT_COUNT = 47;
    if (realDataBody.size() != SLOT_COUNT * 2) {
        qDebug() << "[时隙0处理] 数据长度错误，预期94字节，实际" << realDataBody.size() << "字节";
        return;
    }

    // 遍历每个时隙（0~46）
    for (int i = 0; i < SLOT_COUNT; ++i) {
        int slotIdx = i; // 时隙0~46
        // 提取当前时隙的2字节数据
        quint8 byte1 = static_cast<quint8>(realDataBody.at(i * 2));
        quint8 byte2 = static_cast<quint8>(realDataBody.at(i * 2 + 1));

        // 解析byte1：高7位=发起节点ID，低1位=功能（0=传输，1=测距）
        quint8 fromId = (byte1 >> 1) & 0x7F; // 高7位（0~127）
        int funcFlag = byte1 & 0x01;         // 低1位：0=传输，1=测距
        // 修正typeCode映射：0→传输(1)，1→测距(2)
        int typeCode = funcFlag == 0 ? 1 : 2;
        // 解析byte2：高7位=目标节点ID，低1位=分配状态（0=已分配，1=未分配）
        quint8 toId = (byte2 >> 1) & 0x7F;   // 高7位
        bool isAllocated = (byte2 & 0x01) == 0; // 低1位：0=已分配，1=未分配

        // 获取节点名称（无匹配则显示ID）
        QString fromName = nodeIdToName().value(fromId, QString("ID%1").arg(fromId));
        QString toName = nodeIdToName().value(toId, QString("ID%1").arg(toId));

        // 处理分配状态：未分配则typeCode强制为0
        if (!isAllocated) {
            typeCode = 0; // 未分配
            fromName = "";
            toName = "";
        }

        // 调用updateSlot更新大网时隙（netId=0对应slotItems0）
        updateSlot(0, slotIdx, typeCode, fromName, toName);
        qDebug() << i
                 << "byte1 =" << QString::number(byte1, 16)
                 << "byte2 =" << QString::number(byte2, 16)
                 << "type =" << typeCode
                 << "fromId=" << fromId
                 << "toId=" << toId
                 << "isAllocated=" << isAllocated;
        // qDebug() << "已执行";
    }
    qDebug() << "[时隙0处理] 完成0~46时隙配置更新";
}

void MainWindow::processSlot1Data(const QByteArray &realDataBody)
{
    // 时隙1消息的realDataBody包含：时隙47~79配置（33个时隙，66字节）+ 其他字段
    // 这里只处理时隙配置部分（前66字节）
    const int SLOT_START = 47;
    const int SLOT_COUNT = 33;
    if (realDataBody.size() < SLOT_COUNT * 2) {
        qDebug() << "[时隙1处理] 数据长度不足，预期至少66字节，实际" << realDataBody.size() << "字节";
        return;
    }

    // 遍历每个时隙（47~79）
    for (int i = 0; i < SLOT_COUNT; ++i) {
        int slotIdx = SLOT_START + i; // 时隙47~79
        // 提取当前时隙的2字节数据（仅取realDataBody前66字节）
        quint8 byte1 = static_cast<quint8>(realDataBody.at(i * 2));
        quint8 byte2 = static_cast<quint8>(realDataBody.at(i * 2 + 1));

        // 解析逻辑与时隙0一致
        quint8 fromId = (byte1 >> 1) & 0x7F;
        int typeCode = byte1 & 0x01;
        quint8 toId = (byte2 >> 1) & 0x7F;
        bool isAllocated = (byte2 & 0x01) == 0;

        QString fromName = nodeIdToName().value(fromId, QString("ID%1").arg(fromId));
        QString toName = nodeIdToName().value(toId, QString("ID%1").arg(toId));

        if (!isAllocated) {
            typeCode = 0;
            fromName = "";
            toName = "";
        }

        // 更新大网时隙（netId=0）
        updateSlot(0, slotIdx, typeCode, fromName, toName);
    }
    qDebug() << "[时隙1处理] 完成47~79时隙配置更新";
}
