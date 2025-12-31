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
#include "baseset.h"
#include "applyslot.h"
#include <QButtonGroup>
#include <QMessageBox>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);
    initUiConfig();
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
    connect(ui->actionSelfwork, &QAction::triggered,
            this, &MainWindow::onActionSelfworkTriggered);
    connect(ui->actionControl, &QAction::triggered,
            this, &MainWindow::onActionControlTriggered);
    connect(ui->actionLostcontrol, &QAction::triggered,
            this, &MainWindow::onActionLostcontrolTriggered);
    connect(ui->actionMeasure, &QAction::triggered,
            this, &MainWindow::onActionMeasureTriggered);
    connect(ui->actionBaseset, &QAction::triggered,
            this, &MainWindow::onActionBaseSetTriggered);
    connect(ui->actionJoinstart, &QAction::triggered,
            this, &MainWindow::onActionJoinstartTriggered);
    connect(ui->actionQuitstart, &QAction::triggered,
            this, &MainWindow::onActionQuitstartTriggered);
    connect(ui->actionApplyslot, &QAction::triggered,
            this, &MainWindow::onActionApplyslotTriggered);
    connect(m_idRequestTimer, &QTimer::timeout, this, &MainWindow::sendNodeIdRequest);
    connect(tcpSocket, &QTcpSocket::disconnected, this, [this](){
        isTcpConnected = false;
        ui->actionConnect->setText("连接节点");
        // ui->tip_textEdit->append("TCP 连接异常断开");
    });
    connect(replaceChainDialog, &ReplaceChainDialog::replaceChainConfirmed,
            this, &MainWindow::onReplaceChainConfirmed);

    // initUiConfig();
    // 延迟 0ms 调用 initSlotsScene，保证 UI 完成布局后 fitInView 生效
    QTimer::singleShot(0, this, [this](){
        initSlotsSceneGeneric(slotScene0, slotItems0, ui->restslot_mainlabel, 20, 80);
        initSlotsSceneGeneric(slotScene1, slotItems1, ui->restslot_1label, 20, 80);
        initSlotsSceneGeneric(slotScene2, slotItems2, ui->restslot_2label, 20, 80);

        // 把 scene 缩放到各自 view 中（自适应）
        ui->slotcasemain_View->fitInView(slotScene0->sceneRect(), Qt::KeepAspectRatio);
        ui->slotcase1_View->fitInView(slotScene1->sceneRect(), Qt::KeepAspectRatio);
        ui->slotcase2_View->fitInView(slotScene2->sceneRect(), Qt::KeepAspectRatio);
    });

    // topologyView->updateNodeDeviceId("MX", "000-0000");
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
    replaceChainDialog = new ReplaceChainDialog(this);
    lay->setContentsMargins(0,0,0,0);
    ui->netTopologyView->setLayout(lay);
    topologyView = new TopologyView(this);
    lay->addWidget(topologyView);
    topologyView->drawTopology();
    m_currentNodeName = "";
    m_currentNodeId = 0;
    // // 初始化身份索要定时器
    m_idRequestTimer = new QTimer(this);
    m_idRequestTimer->setInterval(2000);  // 定时2秒

    ui->colorHintLabel->setText(
        "<span style='color:#0000FF;'>数据体长度</span> | "
        "<span style='color:#FF0000;'>指令码</span> | "
        "<span style='color:#000000;'>数据体</span> | "
        "<span style='color:#808080;'>无效帧</span>"
        );
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
    num_freeslot = 0;
}

void MainWindow::onActionConnectTriggered()
{
    if (isTcpConnected)
    {
        if (tcpSocket->isOpen())
        {
            tcpSocket->close(); // 关闭TCP连接
            isTcpConnected = false; // 更新状态标记
            m_idRequestTimer->stop(); // 断开连接时停止定时器
            // 修改Action文本为“连接”
            ui->actionConnect->setText("连接节点");
            ui->tip_textEdit->append("TCP 连接已断开");
        }
        return;
    }

    ConnectDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString ip = dialog.getIP();
        int port = dialog.getPort();

        bool ok = connectToNode(ip, port);

        if (ok)
        {
            isTcpConnected = true; // 更新状态标记为已连接
            m_hasReceivedNodeId = false;// 重置身份接收标记
            ui->actionConnect->setText("断开连接"); // 修改Action文本
            ui->tip_textEdit->append("连接成功!");
            // 启动定时器，开始每隔2秒发送身份索要消息
            m_idRequestTimer->start();
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

    // ui->recvlogTextEdit->setPlainText(tcpRecvBuffer);
    // QTextCursor cursor = ui->recvlogTextEdit->textCursor();
    // cursor.movePosition(QTextCursor::End); // 光标移到末尾
    // ui->recvlogTextEdit->setTextCursor(cursor); // 应用光标位置
    // ui->recvlogTextEdit->ensureCursorVisible(); // 确保末尾内容可见
    ui->recvlogTextEdit->setHtml(tcpRecvBuffer);
    QTextCursor cursor = ui->recvlogTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End); // 光标移到末尾
    ui->recvlogTextEdit->setTextCursor(cursor); // 应用光标位置
    ui->recvlogTextEdit->ensureCursorVisible(); // 确保末尾内容可见
}

void MainWindow::printsenddata(const QByteArray &data)
{
    if (data.isEmpty()) return; // 空数据直接返回
    QString coloredHex; // 带颜色的Hex字符串
    const int LEN_BYTE_COUNT = 2;  // 前2字节（数据长度）
    const int CMD_BYTE_INDEX = 2;  // 第3字节（指令码，索引2）
    // 1. 按字节遍历，为不同部分设置颜色
    for (int i = 0; i < data.size(); ++i)
    {
        // 转换为两位十六进制字符串
        QString hex = QString("%1").arg(static_cast<quint8>(data[i]), 2, 16, QChar('0')).toUpper();
        QString color;
        // 前2字节：蓝色
        if (i < LEN_BYTE_COUNT) {
            color = "#0000FF"; // 蓝色
        }
        // 第3字节：红色
        else if (i == CMD_BYTE_INDEX) {
            color = "#FF0000"; // 红色
        }
        // 后续字节：黑色
        else {
            color = "#000000"; // 黑色
        }
        // 拼接带颜色的HTML标签
        coloredHex += QString("<span style='color:%1;'>%2 </span>").arg(color).arg(hex);
    }
    // 2. 添加时间戳（可选，按发送时间标注）
    QDateTime now = QDateTime::currentDateTime();
    QString timePrefix = QString("<b>[%1] ：</b><br/>").arg(now.toString("yyyy-MM-dd hh:mm:ss"));
    QString sendContent =timePrefix + coloredHex + "<br/>"; // 换行分隔
    // 3. 追加到发送缓冲区，限制最大长度（避免UI卡顿）
    tcpSendBuffer += sendContent;
    const quint32 MAX_SEND_CHAR = 5000; // 富文本字符数限制
    if (tcpSendBuffer.length() > MAX_SEND_CHAR)
    {
        tcpSendBuffer = tcpSendBuffer.right(MAX_SEND_CHAR);
        // 补全未闭合的HTML标签（防止样式错乱）
        int openSpan = tcpSendBuffer.count("<span");
        int closeSpan = tcpSendBuffer.count("</span>");
        for (int i = 0; i < (openSpan - closeSpan); ++i) {
            tcpSendBuffer += "</span>";
        }
    }

    // 4. 显示到sendlogTextEdit并定位光标到末尾
    ui->sendlogTextEdit->setHtml(tcpSendBuffer);
    QTextCursor cursor = ui->sendlogTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End); // 光标移到末尾
    ui->sendlogTextEdit->setTextCursor(cursor);
    ui->sendlogTextEdit->ensureCursorVisible(); // 确保末尾内容可见
}


void MainWindow::sendNodeIdRequest()
{
    // 检查TCP连接状态，避免未连接时发送
    if (!isTcpConnected || !tcpSocket->isOpen()) {
        m_idRequestTimer->stop();  // 停止定时器
        return;
    }

    // 组装身份索要消息：00 00 00
    QByteArray sendData;
    sendData.append((char)0x00);
    sendData.append((char)0x00);
    sendData.append((char)0x00);
    // 发送数据
    tcpSocket->write(sendData);
    printsenddata(sendData);
    tcpSocket->flush();
    // 打印日志
    QString log = QString("[%1] 发送身份请求消息：00 00 00")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    ui->tip_textEdit->append(log);
    qDebug() << log;
}

//TCP接收数据
void MainWindow::dealTcpData()
{
    QByteArray buffer = tcpSocket->readAll();
    if (buffer.isEmpty()) return;
    QString str_rev;  // 最终显示到文本框的内容

    //解析帧结构，区分长度、指令码、数据体 =====
    QString coloredHex; // 带颜色的Hex字符串
    const int FRAME_FIXED_LEN = 3; // 固定帧长度（2字节长度+1字节指令码）
    bool isFrameValid = false;
    quint16 dataLen = 0;
    if (buffer.size() >= FRAME_FIXED_LEN) {
        // 解析数据长度（与parseTcpProtocol逻辑一致）
        dataLen = (static_cast<quint8>(buffer.at(0)) << 8) | static_cast<quint8>(buffer.at(1));
        int expectedFrameLen = FRAME_FIXED_LEN + dataLen;
        // 有效帧条件：总长度匹配声明的长度
        isFrameValid = (buffer.size() == expectedFrameLen);
    }

    //字符串解析：Hex 模式orASCII 模式
    if (!ui->checkBoxHexR->isChecked())
    {
        // 直接按字符串显示（ASCII）
        str_rev = QString::fromUtf8(buffer);
    }
    else
    {
        // Hex模式：按帧结构分颜色显示
        QByteArray lenBytes = buffer.left(2);   // 数据长度字节（前2字节）
        QByteArray cmdByte = buffer.mid(2, 1);  // 指令码字节（第3字节）
        QByteArray bodyBytes = buffer.mid(3);   // 数据体字节（第4字节及以后）

        // 定义颜色：长度(蓝)、指令码(红)、数据体(黑)
        QString colorLen = "#0000FF";    // 蓝色
        QString colorCmd = "#FF0000";    // 红色
        QString colorBody = "#000000";   // 黑色
        QString colorInvalid = "#808080";// 灰色（帧无效时）

        // 拼接带颜色的Hex段
        if (isFrameValid)
        {
            // 1. 数据长度字节（红色）
            for (int i = 0; i < lenBytes.size(); ++i)
            {
                QString hex = QString("%1").arg(static_cast<quint8>(lenBytes[i]), 2, 16, QChar('0')).toUpper();
                coloredHex += QString("<span style='color:%1;'>%2 </span>").arg(colorLen).arg(hex);
            }
            // 2. 指令码字节（蓝色）
            QString cmdHex = QString("%1").arg(static_cast<quint8>(cmdByte[0]), 2, 16, QChar('0')).toUpper();
            coloredHex += QString("<span style='color:%1;'>%2 </span>").arg(colorCmd).arg(cmdHex);
            // 3. 数据体字节（绿色）
            for (int i = 0; i < bodyBytes.size(); ++i)
            {
                QString hex = QString("%1").arg(static_cast<quint8>(bodyBytes[i]), 2, 16, QChar('0')).toUpper();
                coloredHex += QString("<span style='color:%1;'>%2 </span>").arg(colorBody).arg(hex);
            }
        }
        else
        {
            // 帧无效：全部灰色显示
            QString hexStr = buffer.toHex().toUpper();
            for (int i = 0; i < hexStr.length(); i += 2)
            {
                QString hex = hexStr.mid(i, 2);
                coloredHex += QString("<span style='color:%1;'>%2 </span>").arg(colorInvalid).arg(hex);
            }
        }
        str_rev = coloredHex;
        // // Hex 显示
        // QString hexStr = buffer.toHex().toUpper();
        // // 预分配内存避免崩溃
        // QString spaced;
        // spaced.reserve(buffer.size() * 3);
        // for (int i = 0; i < hexStr.length(); i += 2)
        // {
        //     spaced += hexStr.mid(i, 2);
        //     spaced += " ";
        // }
        // str_rev = spaced;
    }

    //添加时间戳
    if (ui->RecTimeZone->isChecked())
    {
        // QDateTime now = QDateTime::currentDateTime();
        // QString timePrefix = "[" + now.toString("yyyy-MM-dd hh:mm:ss") + "]\n";
        // str_rev = timePrefix + str_rev;
        QDateTime now = QDateTime::currentDateTime();
        QString timePrefix = QString("<b>[%1]</b><br/>").arg(now.toString("yyyy-MM-dd hh:mm:ss")); // 加粗时间戳
        str_rev = timePrefix + str_rev;
    }

    // 换行
    // str_rev += "\r\n";
    str_rev += "<br/>";

    //追加到缓冲区（为了不让UI崩溃，需要限制长度）
    tcpRecvBuffer += str_rev;

    // 限制最大显示长度（避免UI卡死）
    // const quint32 MAX_BYTE = 500;
    // if (tcpRecvBuffer.length() > MAX_BYTE)
    // {
    //     tcpRecvBuffer = tcpRecvBuffer.right(MAX_BYTE);
    // }
    const quint32 MAX_CHAR = 20000; // 富文本字符数限制，比纯文本大
    if (tcpRecvBuffer.length() > MAX_CHAR)
    {
        // 截取末尾，避免HTML标签截断导致样式错乱
        tcpRecvBuffer = tcpRecvBuffer.right(MAX_CHAR);
        // 修复可能的标签截断（简单处理：补全未闭合的<span>）
        if (tcpRecvBuffer.contains("<span") && !tcpRecvBuffer.contains("</span>"))
        {
            tcpRecvBuffer += "</span>";
        }
    }

    //调用协议解析
    parseTcpProtocol(buffer);
    refreshTCPDisplay();
}

//数据解析
void MainWindow::parseTcpProtocol(const QByteArray &buffer)
{
    // 基础校验：帧长度至少满足“固定部分+最小数据体（0字节）”
    if (buffer.size() < (FRAME_FIXED_LEN)) { // 3字节（最小帧长度）
        qDebug() << "帧长度不足：实际" << buffer.size() << "字节，最小需3字节";
        return;
    }
    //解析数据长度（后续数据体的字节数）
    quint16 dataLen = (static_cast<quint8>(buffer.at(0)) << 8) | static_cast<quint8>(buffer.at(1));
    int expectedFrameLen = FRAME_FIXED_LEN + dataLen;
    if (buffer.size() != expectedFrameLen) {
        qDebug() << "帧长度不匹配：数据体声明" << dataLen << "字节，实际数据体长度" << buffer.size() - FRAME_FIXED_LEN;
        ui->tip_textEdit->append("帧长度不匹配：数据体声明"
                                 + QString::number(dataLen)
                                 + "字节，实际数据体长度"
                                 + QString::number(buffer.size() - FRAME_FIXED_LEN));
        return;
    }
    //3. 解析指令码（第2字节，即buffer[2]）
    quint8 cmdCode = static_cast<quint8>(buffer.at(2));
    //4. 解析数据体（从第3字节开始，长度为dataLen）
    QByteArray dataBody = buffer.mid(3, dataLen);

    switch (cmdCode) {
        case CMD_NODE_ID:  //连接节点身份
            qDebug() << "收到节点身份消息";
            ui->tip_textEdit->append("收到节点身份消息");
            m_idRequestTimer->stop();
            m_hasReceivedNodeId = true; // 标记已收到身份信息
            controlPermission(dataBody); // 子函数
        break;
        case CMD_BIG_NET_SLOT:  // 大网时隙0/1消息
            qDebug() << "收到大网时隙消息：数据体长度" << dataBody.size() << "字节";
            ui->tip_textEdit->append("收到大网时隙消息：数据体长度"
                                     + QString::number(dataBody.size())
                                     + "字节");
            parseBigNetSlotData(dataBody); // 子函数
        break;
        case CMD_SMALL_NET_MSG:  // 小网遥测数据
            qDebug() << "收到小网遥测数据：数据体长度" << dataBody.size() << "字节";
            ui->tip_textEdit->append("收到小网遥测数据：数据体长度"
                                     + QString::number(dataBody.size())
                                     + "字节");
             // 子函数
            break;
        case CMD_RECVE_DATA:  // 接收数据
            // qDebug() << "收到数据：数据体长度" << dataBody.size() << "字节";
            // ui->tip_textEdit->append("收到数据：数据体长度"
            //                          + QString::number(dataBody.size())
            //                          + "字节");
            recvdataFromNode(dataBody);
            break;
        case CMD_CONTROL_NET_IND:  // 控制网络指示
            qDebug() << "收到控制网络指示";
            ui->tip_textEdit->append(QString("[%1] 收到控制网络指示")
                                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            isControl = true;
            break;
        case CMD_LOSECONTROL_IND:  // 失去控制指示
            qDebug() << "收到失去控制指示";
            ui->tip_textEdit->append(QString("[%1] 收到失去控制指示")
                                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            isLoseControl = true;
            break;
    default:
        qDebug() << "未知指令码：0x" << QString::number(cmdCode, 16).rightJustified(2, '0');
        break;
    }
}

void MainWindow::onActionMxModeTriggered()
{
    MXmodeDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString mode = dialog.selectedMode();
        quint8 modeByte = dialog.selectedModeByte();
        if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
            QString errorLog = QString("[%1] MX模式指令发送失败：请先建立TCP连接！")
                                   .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            ui->tip_textEdit->append(errorLog);
            qDebug() << errorLog;
            return;
        }
        QByteArray sendData;
        quint16 dataLen = 1;
        sendData.append((quint8)((dataLen >> 8) & 0xFF));
        sendData.append((quint8)(dataLen & 0xFF));
        quint8 cmd = 0x03;
        sendData.append(reinterpret_cast<const char*>(&cmd), 1);
        sendData.append(reinterpret_cast<const char*>(&modeByte), 1);
        qint64 sentBytes = tcpSocket->write(sendData);
        printsenddata(sendData);
        if (sentBytes == sendData.size()) {
            QString successLog = QString("[%1] 发送MX%2指令")
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                     .arg(mode)
                                     .arg(QString::number(modeByte, 16).toUpper());
            ui->tip_textEdit->append(successLog);
            qDebug() << successLog;
        } else {
            QString failLog = QString("[%1] MX模式指令发送失败：仅发送%2字节（预期%3字节），模式：%4")
                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                  .arg(sentBytes)
                                  .arg(sendData.size())
                                  .arg(mode);
            ui->tip_textEdit->append(failLog);
            qDebug() << failLog;
        }
    }
}

void MainWindow::onActionReplaceTriggered()
{
    replaceChainDialog->show();
    replaceChainDialog->raise();
    replaceChainDialog->activateWindow();
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

    // -------------------------- 1. Hex模式输入校验 --------------------------
    QByteArray inputData; // 存储处理后的输入数据（Hex转二进制/ASCII直接转）
    bool isHexMode = ui->checkBoxHexS->isChecked();
    if (isHexMode) {
        // 校验1：是否为空（避免纯空格）
        QString purePayload = payload.trimmed();
        if (purePayload.isEmpty()) {
            QMessageBox::warning(this, "警告", "Hex模式下数据不能为空！");
            return;
        }

        // 校验2：是否为“空格分隔的2字符十六进制”（正则匹配 00 A1 B2 格式）
        QRegularExpression hexRegex("^([0-9A-Fa-f]{2})( [0-9A-Fa-f]{2})*$");
        if (!hexRegex.match(purePayload).hasMatch()) {
            QMessageBox::warning(this, "警告", "Hex模式需输入空格分隔的2字符十六进制（如：A1 B2 C3）");
            return;
        }

        // 处理：去掉空格，转二进制数据
        QString noSpaceHex = purePayload.replace(" ", "");
        inputData = QByteArray::fromHex(noSpaceHex.toUtf8());
    } else {
        // ASCII模式：直接转字节数组
        inputData = payload.toUtf8();
    }

    // -------------------------- 2. 生成“高7位节点ID+低1位0”的头字节 --------------------------
    // 节点名称→7位ID映射（严格遵循文档：大网节点3bit功能编码+4bit序号=7bit ID）
    QMap<QString, quint8> nodeNameTo7bitId = {
        {"MX（000-0000）",    0b0000000}, // MX：功能编码000 + 序号0
        {"GZ1（001-0000）",   0b0010000}, // GZ1：功能编码001 + 序号0
        {"GZ2（001-0001）",   0b0010001}, // GZ2：功能编码001 + 序号1
        {"BY1（010-0000）",   0b0100000}, // BY1：功能编码010 + 序号0
        {"BY2（010-0001）",   0b0100001}, // BY2：功能编码010 + 序号1
        {"BY3（010-0010）",   0b0100010}, // BY3：功能编码010 + 序号2
        {"BY4（010-0011）",   0b0100011}, // BY4：功能编码010 + 序号3
        {"BY5（010-0100）",   0b0100100}, // BY5：功能编码010 + 序号4
        {"BY6（010-0101）",   0b0100101}, // BY6：功能编码010 + 序号5
        {"ZJ0（011-0000）",   0b0110000}, // ZJ0：功能编码011 + 序号0
        {"ZJ1（011-0001）",   0b0110001}, // ZJ1：功能编码011 + 序号1
        {"ZJ2（011-0010）",   0b0110010}, // ZJ2：功能编码011 + 序号2
        {"ZJ3（011-0011）",   0b0110011}, // ZJ3：功能编码011 + 序号3
        {"广播（000-1111）",   0b0001111}  // 广播：文档指定ID 000-1111
    };

    // 校验节点是否存在
    if (!nodeNameTo7bitId.contains(targetNode)) {
        QMessageBox::warning(this, "警告", "未知目标节点：" + targetNode);
        return;
    }

    // 生成头字节：高7位=节点ID，低1位=0
    quint8 headByte = (nodeNameTo7bitId[targetNode] << 1) | 0x00;

    //3. 封装完整消息
    QByteArray dataBody;
    dataBody.append(headByte);       // 头字节（1Byte）
    dataBody.append(inputData);      // 输入数据（N Byte）

    // 封装最终TCP发送包（数据体长度+指令码+数据体）
    quint16 dataBodyLen = static_cast<quint16>(dataBody.size()); // 数据体长度（1+N Byte）
    QByteArray dataLenBytes;
    dataLenBytes.append((quint8)((dataBodyLen >> 8) & 0xFF)); // 高8位
    dataLenBytes.append((quint8)(dataBodyLen & 0xFF));        // 低8位

    // 打包数据（2Byte长度+指令码+数据体）
    QByteArray packet;
    packet.append(dataLenBytes);  // 2Byte数据体长度
    packet.append(0x05);          // 1Byte指令码
    packet.append(dataBody);      // 1+N Byte数据体

    // -------------------------- 4. TCP发送数据 --------------------------
    tcpSocket->write(packet);
    tcpSocket->flush();

    // -------------------------- 5. 更新发送日志--------------------------
    // QString displayPayload;
    // if (isHexMode) {
    //     // Hex模式日志：显示“头字节+输入数据”的Hex格式（带空格）
    //     QByteArray allData = inputData;
    //     displayPayload.reserve(allData.size() * 3 - 1);
    //     QString hexStr = allData.toHex().toUpper();
    //     for (int i = 0; i < hexStr.length(); i += 2) {
    //         displayPayload += hexStr.mid(i, 2);
    //         if (i + 2 < hexStr.length()) displayPayload += " ";
    //     }
    //     displayPayload = "[Hex] " + displayPayload; // 日志标注Hex模式
    // } else {
    //     // ASCII模式日志：显示原始输入
    //     displayPayload = payload;
    // }

    // 拼接日志（带时间戳/无时间戳）
    QString timeStr = ui->SendTimeZone->isChecked()
                          ? QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                          : "";
    // QString log = timeStr.isEmpty()
    //                   ? QString("发送到 %1 :\n%2").arg(targetNode).arg(displayPayload)
    //                   : QString("[%1] 发送到 %2 :\n%3").arg(timeStr).arg(targetNode).arg(displayPayload);
    // ui->sendlogTextEdit->appendPlainText(log);
    QString log = timeStr.isEmpty()
                      ? QString("发送数据到 %1 ").arg(targetNode)
                      : QString("[%1] 发送数据到 %2 ").arg(timeStr).arg(targetNode);
    ui->tip_textEdit->append(log);
    printsenddata(packet);
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
        ui->tip_textEdit->append("[时隙0/1解析] 数据体过短，无法解析核心字段，当前长度："
                                 + QString::number(dataBody.size())
                                 + "字节（需≥4字节）");
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
    ui->tip_textEdit->append("[时隙0/1解析] 识别到'"
                             + slotType
                             + "'消息，消息长度字段值："
                             + QString::number(msgLenField));

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

    //6. 长度校验通过（后续调用数据处理函数）
    qDebug() << "[时隙0/1解析] " << slotType << "消息长度校验通过！";
    ui->tip_textEdit->append("[时隙0/1解析] '"
                             + slotType
                             + "'消息长度校验通过！"
                             );

    QByteArray realDataBody = dataBody.mid(FIXED_HEAD_LEN); // 提取后续真实数据体（跳过前4字节固定头部）

    // 调用你已有的数据处理函数（按需替换为你的函数名）
    if (msgType == MSG_TYPE_SLOT0) {
        processSlot0Data(realDataBody); // 时隙0数据处理
    } else {
        processSlot1Data(realDataBody); // 时隙1数据处理
        processMain_TopologData(realDataBody);//大网拓扑图处理
    }
}

void MainWindow::controlPermission(const QByteArray &dataBody)
{
    // 1. 校验数据长度（仅1字节）
    if (dataBody.size() != 1) {
        QString errorLog = QString("[%1] 权限控制：数据长度错误，预期1字节，实际%2字节")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                               .arg(dataBody.size());
        qDebug() << errorLog;
        ui->tip_textEdit->append(errorLog);
        return;
    }

    // 2. 解析1字节数据：高7位为节点ID，低1位置0
    quint8 dataByte = static_cast<quint8>(dataBody.at(0));
    quint8 nodeId = (dataByte >> 1) & 0x7F; // 右移1位丢弃低1位，取高7位ID
    QString nodeName = nodeIdToFunc().value(nodeId, QString("未知节点(ID:0x%1)").arg(QString::number(nodeId, 16)));

    // 3. 打印节点身份到tip_textEdit
    QString identityLog = QString("[%1] 检测到节点身份：%2 ")
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                              .arg(nodeName);
    ui->tip_textEdit->append(identityLog);
    ui->Node_ID->setText(QString("%1 ").arg(nodeName));
    qDebug() << identityLog;
    updateMeasureActionText();

    // 4. 根据节点身份控制权限
    if (nodeName == "MX") {
        // MX节点：拥有所有操作权限（启用所有控件）
        ui->actionSelfwork->setEnabled(true);
        ui->netlunch_pushButton->setEnabled(true);
        ui->actionMxMode->setEnabled(true);
        ui->actionReplace->setEnabled(true);
        ui->actionControl->setEnabled(true);
        ui->actionLostcontrol->setEnabled(true);
    } else if (nodeName == "GZ1" || nodeName == "GZ2") {
        // GZ1/GZ2节点：
        ui->actionSelfwork->setEnabled(false);
        ui->netlunch_pushButton->setEnabled(false);
        ui->actionMxMode->setEnabled(false);
        ui->actionReplace->setEnabled(false);
        ui->actionControl->setEnabled(true);
        ui->actionLostcontrol->setEnabled(true);
    } else {
        // 其他节点（BY/ZJ等）：
        ui->actionSelfwork->setEnabled(false);
        ui->netlunch_pushButton->setEnabled(false);
        ui->actionMxMode->setEnabled(false);
        ui->actionReplace->setEnabled(false);
        ui->actionControl->setEnabled(false);
        ui->actionLostcontrol->setEnabled(false);
    }
}

void MainWindow::processSlot0Data(const QByteArray &realDataBody)
{
    // 时隙0消息的realDataBody是时隙0~46的配置（共47个时隙，每个2字节，总94字节）
    const int SLOT_COUNT = 47;
    if (realDataBody.size() != SLOT_COUNT * 2) {
        qDebug() << "[时隙0处理] 数据长度错误，预期94字节，实际" << realDataBody.size() << "字节";
        ui->tip_textEdit->append("[时隙0处理] 数据长度错误，预期94字节，实际"
                                 + QString::number(realDataBody.size())
                                 + "字节");
        return;
    }
    bool isCurrentNodeRanging = false;
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
        //检测当前节点是否在测距
        if (m_currentNodeId != 0) { // 已识别当前节点
            // 发起节点是当前节点 且 功能为测距（funcFlag=1）
            if (fromId == m_currentNodeId && funcFlag == 1) {
                isCurrentNodeRanging = true;// 只要有一个时隙在测距，就标记为测距中
            }
            // 目标节点是当前节点 且 功能为测距（可选：若需检测被测距状态）
            if (toId == m_currentNodeId && funcFlag == 1) {
                isCurrentNodeRanging = true;
            }
        }
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
    // 缓存当前节点的测距状态
    m_nodeRangingState[m_currentNodeId] = isCurrentNodeRanging;
    // 更新actionMeasure文本
    updateMeasureActionText();
    qDebug() << "[时隙0处理] 完成0~46时隙配置更新";
    ui->tip_textEdit->append("[时隙0处理] 完成0~46时隙配置更新");
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
        int funcFlag = byte1 & 0x01;         // 低1位：0=传输，1=测距
        // 修正typeCode映射：0→传输(1)，1→测距(2)
        int typeCode = funcFlag == 0 ? 1 : 2;
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
    ui->tip_textEdit->append("[时隙1处理] 完成47~79时隙配置更新");
    quint8 num_freeslotByte = static_cast<quint8>(realDataBody.at(realDataBody.size() - 8));
    // 赋值给全局静态变量
    num_freeslot = static_cast<int>(num_freeslotByte);
    qDebug() << "[时隙1处理] 解析到动态时隙数量：" << num_freeslot;
    ui->tip_textEdit->append(QString("[时隙1处理] 动态时隙数量：%1").arg(num_freeslot));

}

// static QMap<quint8, QString> nodeIdToName() {
//     QMap<quint8, QString> map;
//     // 大网节点ID（7bit）
//     map.insert(0b0000000, "MX");       // 000-0000
//     map.insert(0b0010000, "GZ1");      // 001-0000
//     map.insert(0b0010001, "GZ2");      // 001-0001
//     map.insert(0b0100000, "BY1");      // 010-0000
//     map.insert(0b0100001, "BY2");      // 010-0001
//     map.insert(0b0100010, "BY3");      // 010-0010
//     map.insert(0b0100011, "BY4");      // 010-0011
//     map.insert(0b0100100, "BY5");      // 010-0100
//     map.insert(0b0100101, "BY6");      // 010-0101
//     map.insert(0b0110000, "ZJ0");      // 011-0000
//     map.insert(0b0110001, "ZJ1");      // 011-0001
//     map.insert(0b0110010, "ZJ2");      // 011-0010
//     map.insert(0b0110011, "ZJ3");      // 011-0011
//     // 小网ZJ节点ID
//     map.insert(0b1000000, "DJ1");
//     map.insert(0b1000001, "DJ2");
//     map.insert(0b1000010, "DJ3");
//     map.insert(0b1000011, "DJ4");
//     map.insert(0b1000100, "DJ5");
//     map.insert(0b1000101, "DJ6");
//     map.insert(0b1000110, "DJ7");
//     map.insert(0b1000111, "DJ8");
//     map.insert(0b1001000, "DJ9");
//     map.insert(0b1001001, "DJ10");
//     map.insert(0b1001010, "DJ11");
//     map.insert(0b1001011, "DJ12");
//     map.insert(0b1001100, "DJ13");
//     map.insert(0b1001101, "DJ14");
//     map.insert(0b1001110, "DJ15");
//     map.insert(0b1001111, "DJ16");
//     map.insert(0b1010000, "DJ17");
//     map.insert(0b1010001, "DJ18");
//     map.insert(0b1010010, "DJ19");
//     map.insert(0b1010011, "DJ20");
//     return map;
// }

QString nodeIdToBinaryStr(quint8 nodeId) {
    return QString("%1-%2")
    .arg((nodeId >> 4) & 0x07, 3, 2, QChar('0'))
        .arg(nodeId & 0x0F, 4, 2, QChar('0'));
}

quint8 MainWindow::parse7bitNodeIdFromByte(quint8 stateByte)
{
    quint8 nodeId = (stateByte >> 1) & 0x7F;
    return nodeId;
}

QString MainWindow::getDeviceIdShowText(quint8 nodeId)
{
    QMap<quint8, QString> idMap = MainWindow::nodeIdToName();
    if (((nodeId >> 4) & 0x07) == 0b000) {
        return "Ori_MX(000-0000)";
    }
    return idMap.value(nodeId, QString("未知设备ID(0x%1)").arg(QString::number(nodeId, 16).toUpper()));
}

void MainWindow::processMain_TopologData(const QByteArray &realDataBody)
{
    // 校验数据长度：拓扑信息从第67字节（索引66）开始，需至少包含6字节基座+14字节节点状态
    const int TOPO_START_INDEX = 66;    // 第67字节对应索引66
    const int MIN_TOPO_LEN = 6 + 14;    // 拓扑信息最小长度
    if (realDataBody.size() < TOPO_START_INDEX + MIN_TOPO_LEN) {
        qDebug() << "[拓扑处理] 数据长度不足，预期至少" << TOPO_START_INDEX + MIN_TOPO_LEN
                 << "字节，实际" << realDataBody.size() << "字节";
        return;
    }

    // 解析基座分配信息（6字节：3个基座，每个2字节）
    QMap<QString, bool> baseNodes; // 标记基座节点，避免重复更新
    for (int i = 0; i < 3; ++i) {
        int baseOffset = TOPO_START_INDEX + i * 2;
        // 解析基座序号（第1字节）
        quint8 baseSeq = static_cast<quint8>(realDataBody.at(baseOffset));
        // 解析节点ID（第2字节：前7bit为ID，第8bit置0）
        quint8 nodeId = (static_cast<quint8>(realDataBody.at(baseOffset + 1)) >> 1) & 0x7F;

        // 映射节点ID到名称
        QString nodeName = nodeIdToFunc().value(nodeId, QString("未知节点(0x%1)").arg(QString::number(nodeId, 16)));

        // 更新基座节点状态（slot=2表示基座）
        topologyView->updateNodeState(nodeName, 2);
        baseNodes[nodeName] = true; // 标记为基座节点
        qDebug() << "[基座分配] 基座" << static_cast<int>(baseSeq) << "→ 节点：" << nodeName << "(ID:0x" << QString::number(nodeId, 16) << ")";
    }

    // 解析节点状态信息（14字节：每个字节对应一个节点）
    const int NODE_STATE_START = TOPO_START_INDEX + 6; // 基座之后开始节点状态
    QStringList identityList = {
        "MX","GZ1","GZ2","BY1","BY2","BY3","BY4","BY5","BY6","ZJ0","ZJ1","ZJ2","ZJ3","ZJ4"
    };

    // 遍历14个状态字节，按顺序匹配身份
    for (int i = 0; i < 14; ++i) {
        int stateOffset = NODE_STATE_START + i;
        quint8 stateByte = static_cast<quint8>(realDataBody.at(stateOffset));
        QString curIdentity = identityList.at(i); // 获取当前字节对应的身份名称
        // 基座节点优先：已标记为基座的身份，跳过入网状态更新
        if (baseNodes.contains(curIdentity)) {
            qDebug() << "[拓扑处理] " << curIdentity << "→ 基座节点，跳过状态更新（保留ID更新）";
        } else {
            // 非基座节点：正常更新入网状态
            bool isJoined = (stateByte != 0x00);
            int slot = isJoined ? 1 : 0;
            topologyView->updateNodeState(curIdentity, slot);
        }
        //解析承担者ID
        QString deviceIdShow = ""; // 默认空（无承担者）
        if (stateByte != 0x00) { // 非零字节才解析承担者ID
            quint8 nodeId7bit = parse7bitNodeIdFromByte(stateByte); // 提取高7位ID
            deviceIdShow = getDeviceIdShowText(nodeId7bit); // 含MX特殊判定的最终显示文本
        }
        topologyView->updateNodeDeviceId(curIdentity, deviceIdShow);
        replaceChainDialog->updateNodeDeviceId(curIdentity, deviceIdShow);
        qDebug() << "[拓扑处理] " << curIdentity
                 << " | 状态字节:0x" << QString::number(stateByte,16).toUpper()
                 << " | 节点类型:" << (baseNodes.contains(curIdentity) ? "基座" : (stateByte != 0x00 ? "已入网" : "未入网"))
                 << " | 承担者ID:" << (deviceIdShow.isEmpty() ? "无" : deviceIdShow);
    }
    qDebug() << "[拓扑处理] 完成拓扑图节点状态+承担者ID更新";
    // replaceChainDialog->updateNodeDeviceId("DJ1", "100");
}

void MainWindow::on_recvdataclear_pushButton_clicked()
{
    // 1. 清空UI显示
    ui->recvlogTextEdit->clear();
    // 2. 清空核心缓冲区
    tcpRecvBuffer.clear();
    // 3. 重置光标到开头
    QTextCursor cursor = ui->recvlogTextEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    ui->recvlogTextEdit->setTextCursor(cursor);
}


void MainWindow::on_netlunch_pushButton_clicked()
{
    if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        QString log = QString("[%1] 发送失败：请先建立TCP连接")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(log);
        qDebug() << log;
        return;
    }
    QByteArray sendData;
    quint16 dataLen = 0;
    sendData.append(reinterpret_cast<const char*>(&dataLen), 2); // 2字节数据长度
    quint8 cmd = 0x01;
    sendData.append(reinterpret_cast<const char*>(&cmd), 1); // 1字节指令码
    qint64 bytesSent = tcpSocket->write(sendData);
    if (bytesSent == sendData.size()) {
        // 发送成功，打印日志到tip_textEdit
        QString log = QString("[%1] 发送网络启动指令")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(log);
        qDebug() << log;
    } else {
        // 发送失败
        QString log = QString("[%1] 发送网络启动指令失败：仅发送%2字节（预期%3字节）")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                          .arg(bytesSent)
                          .arg(sendData.size());
        ui->tip_textEdit->append(log);
        qDebug() << log;
    }
}

void MainWindow::onActionSelfworkTriggered()
{
    if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        QString errorLog = QString("[%1] 发送失败：请先建立TCP连接！")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(errorLog);
        qDebug() << errorLog;
        return;
    }
    QByteArray sendData;
    quint16 dataLen = 0;
    sendData.append(reinterpret_cast<const char*>(&dataLen), 2);
    quint8 cmd = 0x02;
    sendData.append(reinterpret_cast<const char*>(&cmd), 1);
    qint64 sentBytes = tcpSocket->write(sendData);
    printsenddata(sendData);
    if (sentBytes == sendData.size()) {
        QString successLog = QString("[%1] 发送自主工作指令")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(successLog);
        qDebug() << successLog;
    } else {
        QString failLog = QString("[%1] 自主工作指令发送失败：仅发送%2字节（预期%3字节）")
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                              .arg(sentBytes)
                              .arg(sendData.size());
        ui->tip_textEdit->append(failLog);
        qDebug() << failLog;
    }
}

void MainWindow::recvdataFromNode(const QByteArray &dataBody)
{
    if (dataBody.isEmpty()) {
        QString errorLog = QString("[%1] 接收数据失败：数据体为空")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(errorLog);
        qDebug() << errorLog;
        return;
    }
    quint8 nodeIdByte = static_cast<quint8>(dataBody.at(0));
    quint8 nodeId = (nodeIdByte >> 1) & 0x7F; // 右移1位丢弃低1位，取高7位ID
    QString nodeName = nodeIdToName().value(nodeId, QString("未知节点(ID:0x%1)").arg(QString::number(nodeId, 16).toUpper()));

    // 3. 组装日志并打印到tip_textEdit
    QString recvLog = QString("[%1] 收到来自%2的数据，数据长度为：%3字节")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")) // 时间
                          .arg(nodeName) // 节点名称
                          .arg(dataBody.size()); // 数据体长度
    ui->tip_textEdit->append(recvLog);
    qDebug() << recvLog << "| 节点ID(高7位)：0x" << QString::number(nodeId, 16).toUpper();
}

void MainWindow::onActionControlTriggered()
{
    if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        QString errorLog = QString("[%1] 发送失败：请先建立TCP连接！")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(errorLog);
        qDebug() << errorLog;
        return;
    }
    if (isControl == false){
        QMessageBox::warning(this, "警告", "未收到控制网络指示，无法发送控制网络确认");
        return;
    }else{
        QByteArray sendData;
        quint16 dataLen = 0;
        sendData.append(reinterpret_cast<const char*>(&dataLen), 2);
        quint8 cmd = 0x0A;
        sendData.append(reinterpret_cast<const char*>(&cmd), 1);
        qint64 sentBytes = tcpSocket->write(sendData);
        printsenddata(sendData);
        if (sentBytes == sendData.size()) {
            QString successLog = QString("[%1] 控制网络确认")
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            qDebug() << successLog;
            ui->tip_textEdit->append(successLog);
            ui->tip_textEdit->append("当前节点升级为MX");
            ui->actionSelfwork->setEnabled(true);
            ui->netlunch_pushButton->setEnabled(true);
            ui->actionMxMode->setEnabled(true);
            ui->actionReplace->setEnabled(true);
            ui->actionControl->setEnabled(true);
            ui->actionLostcontrol->setEnabled(true);
        } else {
            QString failLog = QString("[%1] 控制网络确认失败：仅发送%2字节（预期%3字节）")
                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                  .arg(sentBytes)
                                  .arg(sendData.size());
            ui->tip_textEdit->append(failLog);
            qDebug() << failLog;
        }
        isControl = false;
    }
}

void MainWindow::onActionLostcontrolTriggered()
{
    if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        QString errorLog = QString("[%1] 发送失败：请先建立TCP连接！")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(errorLog);
        qDebug() << errorLog;
        return;
    }
    if (isLoseControl == false){
        QMessageBox::warning(this, "警告", "未收到失去控制指示，无法发送失去控制确认");
        return;
    }else{
        QByteArray sendData;
        quint16 dataLen = 0;
        sendData.append(reinterpret_cast<const char*>(&dataLen), 2);
        quint8 cmd = 0x0C;
        sendData.append(reinterpret_cast<const char*>(&cmd), 1);
        qint64 sentBytes = tcpSocket->write(sendData);
        printsenddata(sendData);
        if (sentBytes == sendData.size()) {
            QString successLog = QString("[%1] 失去控制确认")
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            qDebug() << successLog;
            ui->tip_textEdit->append(successLog);
            ui->tip_textEdit->append("当前节点失去MX资格");
            ui->actionSelfwork->setEnabled(false);
            ui->netlunch_pushButton->setEnabled(false);
            ui->actionMxMode->setEnabled(false);
            ui->actionReplace->setEnabled(false);
            ui->actionControl->setEnabled(true);
            ui->actionLostcontrol->setEnabled(true);
        } else {
            QString failLog = QString("[%1] 失去控制确认失败：仅发送%2字节（预期%3字节）")
                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                  .arg(sentBytes)
                                  .arg(sendData.size());
            ui->tip_textEdit->append(failLog);
            qDebug() << failLog;
        }
        isLoseControl = false;
    }
}

void MainWindow::updateMeasureActionText()
{
    // 1. 从Node_ID标签获取当前节点名称/ID
    QString nodeText = ui->Node_ID->text();
    if (nodeText.contains("未知") || nodeText.contains("未检测")) {
        ui->actionMeasure->setText("恢复测距"); // 默认文本
        ui->actionMeasure->setEnabled(false); // 未识别节点时禁用
        return;
    }
    // 2. 解析当前节点ID（从nodeIdToName反向映射）
    m_currentNodeName = "";
    m_currentNodeId = 0;
    QMap<quint8, QString> idToName = nodeIdToName();
    for (auto it = idToName.begin(); it != idToName.end(); ++it) {
        if (nodeText.contains(it.value())) {
            m_currentNodeName = it.value();
            m_currentNodeId = it.key();
            break;
        }
    }
    // 3. 判断是否测距，更新action文本
    bool isRanging = m_nodeRangingState.value(m_currentNodeId, false);
    if (isRanging) {
        ui->actionMeasure->setText("取消测距");
    } else {
        ui->actionMeasure->setText("恢复测距");
    }
    ui->actionMeasure->setEnabled(true); // 识别节点后启用
}

void MainWindow::onActionMeasureTriggered()
{
    if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        QString errorLog = QString("[%1] 发送失败：请先建立TCP连接！")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(errorLog);
        qDebug() << errorLog;
        return;
    }
    // 检查当前节点是否识别
    if (m_currentNodeName.isEmpty() || m_currentNodeId == 0) {
        QString errorLog = QString("[%1] 发送测距指令失败：未识别当前节点！")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(errorLog);
        qDebug() << errorLog;
        return;
    }
    //  判断当前文本，确定指令码
    quint8 cmd = 0;
    QString logPrefix = "";
    if (ui->actionMeasure->text() == "取消测距") {
        cmd = 0x0D; // 取消测距指令码
        logPrefix = "发送取消测距指令";
    } else {
        cmd = 0x0E; // 恢复测距指令码
        logPrefix = "发送恢复测距指令";
    }
    QByteArray sendData;
    quint16 dataLen = 0;
    sendData.append(reinterpret_cast<const char*>(&dataLen), 2);
    sendData.append(reinterpret_cast<const char*>(&cmd), 1);
    qint64 sentBytes = tcpSocket->write(sendData);
    printsenddata(sendData);
    if (sentBytes == sendData.size()) {
        QString successLog = QString("[%1] %2（指令码：0x%3")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                 .arg(logPrefix)
                                 .arg(QString::number(cmd, 16).toUpper());
        ui->tip_textEdit->append(successLog);
        qDebug() << successLog;
    } else {
        QString failLog = QString("[%1] %2失败：仅发送%3字节（预期%4字节）")
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                              .arg(logPrefix)
                              .arg(sentBytes)
                              .arg(sendData.size());
        ui->tip_textEdit->append(failLog);
        qDebug() << failLog;
    }
}

void MainWindow::onActionBaseSetTriggered()
{
    BaseSet dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        // 1. 获取选择的基座信息
        QMap<int, quint8> baseIds = dialog.selectedBaseIds();
        QMap<int, QString> baseNames = dialog.selectedBaseNames();

        // 2. 检查TCP连接
        if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
            QString errorLog = QString("[%1] 基座设置失败：请先建立TCP连接！")
                                   .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            ui->tip_textEdit->append(errorLog);
            qDebug() << errorLog;
            return;
        }
        // 3. 组装数据帧（数据长度2字节 + 指令码1字节 + 数据体6字节）
        QByteArray sendData;
        quint16 dataLen = 6; // 数据体长度（3个基座×2字节）
        quint16 bigEndianLen = qToBigEndian(dataLen); // 转换为大端序
        sendData.append(reinterpret_cast<const char*>(&bigEndianLen), 2);
        quint8 cmd = 0x13;
        sendData.append(reinterpret_cast<const char*>(&cmd), 1);
        // 组装数据体（3个基座，每个2字节）
        for (int i = 0; i < 3; ++i) {
            quint8 baseSeq = static_cast<quint8>(i); // 基座序号：0=基座1，1=基座2，2=基座3
            quint8 nodeId = baseIds.value(i);
            quint8 nodeIdByte = (nodeId << 1) & 0xFE; // 高7位为ID，低1位置0

            sendData.append(reinterpret_cast<const char*>(&baseSeq), 1);
            sendData.append(reinterpret_cast<const char*>(&nodeIdByte), 1);
        }
        // 4. 发送数据
        tcpSocket->flush();
        qint64 sentBytes = tcpSocket->write(sendData);
        printsenddata(sendData);

        // 5. 打印日志
        if (sentBytes == sendData.size()) {
            // 组装日志内容
            QString log = QString("[%1] 基座设置完成：基座1：%2 基座2：%3 基座3：%4")
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                              .arg(baseNames.value(0))
                              .arg(baseNames.value(1))
                              .arg(baseNames.value(2));
            ui->tip_textEdit->append(log);
            qDebug() << log << "发送数据：" << sendData.toHex(' ').toUpper();
        } else {
            QString failLog = QString("[%1] 基座设置指令发送失败：仅发送%2字节（预期%3字节）")
                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                  .arg(sentBytes)
                                  .arg(sendData.size());
            ui->tip_textEdit->append(failLog);
            qDebug() << failLog;
        }
    }
}

void MainWindow::onActionJoinstartTriggered()
{
    if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        QString errorLog = QString("[%1] 发送失败：请先建立TCP连接！")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(errorLog);
        qDebug() << errorLog;
        return;
    }
    QByteArray sendData;
    quint16 dataLen = 0;
    sendData.append(reinterpret_cast<const char*>(&dataLen), 2);
    quint8 cmd = 0x11;
    sendData.append(reinterpret_cast<const char*>(&cmd), 1);
    qint64 sentBytes = tcpSocket->write(sendData);
    printsenddata(sendData);
    if (sentBytes == sendData.size()) {
        QString successLog = QString("[%1] 发送入网启动指令")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(successLog);
        qDebug() << successLog;
    } else {
        QString failLog = QString("[%1] 入网启动指令发送失败：仅发送%2字节（预期%3字节）")
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                              .arg(sentBytes)
                              .arg(sendData.size());
        ui->tip_textEdit->append(failLog);
        qDebug() << failLog;
    }
}

void MainWindow::onActionQuitstartTriggered()
{
    if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        QString errorLog = QString("[%1] 发送失败：请先建立TCP连接！")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(errorLog);
        qDebug() << errorLog;
        return;
    }
    QByteArray sendData;
    quint16 dataLen = 0;
    sendData.append(reinterpret_cast<const char*>(&dataLen), 2);
    quint8 cmd = 0x12;
    sendData.append(reinterpret_cast<const char*>(&cmd), 1);
    qint64 sentBytes = tcpSocket->write(sendData);
    printsenddata(sendData);
    if (sentBytes == sendData.size()) {
        QString successLog = QString("[%1] 发送退网启动指令")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(successLog);
        qDebug() << successLog;
    } else {
        QString failLog = QString("[%1] 退网启动指令发送失败：仅发送%2字节（预期%3字节）")
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                              .arg(sentBytes)
                              .arg(sendData.size());
        ui->tip_textEdit->append(failLog);
        qDebug() << failLog;
    }
}

void MainWindow::onActionApplyslotTriggered()
{
    if (!tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        QString errorLog = QString("[%1] 发送失败：请先建立TCP连接！")
                               .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        ui->tip_textEdit->append(errorLog);
        qDebug() << errorLog;
        return;
    }
    // 获取动态空闲时隙数量
    int freeSlotNum = num_freeslot;
    // 创建申请时隙对话框，传入空闲时隙数量
    ApplySlot dlg(freeSlotNum, this);
    // 连接对话框的确认信号到主窗口的处理函数
    connect(&dlg, &ApplySlot::applyConfirmed, this, &MainWindow::onApplyConfirmed);
    // 显示对话框
    dlg.exec();
}

void MainWindow::onApplyConfirmed(int transNum, int measureNum)
{
    // 在tip_textEdit中显示申请信息
    ui->tip_textEdit->append(QString("[时隙申请] 申请传输时隙：%1个，申请测距时隙：%2个")
                                 .arg(transNum).arg(measureNum));
    // 向下位机发送消息
    quint8 byte1 = (0 << 7) | (transNum & 0x7F); // bit7=0 + 低7位为transNum
    quint8 byte2 = (1 << 7) | (measureNum & 0x7F); // bit7=1 + 低7位为measureNum
    QByteArray dataBody;
    dataBody.append(static_cast<char>(byte1));
    dataBody.append(static_cast<char>(byte2));
    QByteArray packet;
    quint16 dataLen = static_cast<quint16>(dataBody.size());
    packet.append(static_cast<char>((dataLen >> 8) & 0xFF));
    packet.append(static_cast<char>(dataLen & 0xFF));
    packet.append(static_cast<char>(0x0F));
    packet.append(dataBody);
    tcpSocket->write(packet);
    printsenddata(packet);
    tcpSocket->flush();
}
void ReplaceChainDialog::updateNodeState(const QString& name, int slot)
{
    if (!m_nodes.contains(name)) return;
    if (slot == 0) m_nodes[name]->setState(NodeItem::NotJoined);
    else if (slot == 1) m_nodes[name]->setState(NodeItem::Joined);
    else m_nodes[name]->setState(NodeItem::Base);
}

void ReplaceChainDialog::updateNodeDeviceId(const QString& name, const QString& id)
{
    if (!m_nodes.contains(name)) return;
    m_nodes[name]->setDeviceId(id);
}

void MainWindow::onReplaceChainConfirmed(const QString& tipText)
{
    if (!tcpSocket || !tcpSocket->isOpen()) {
        ui->tip_textEdit->append("TCP 未连接，无法发送数据");
        return;
    }
    ui->tip_textEdit->append(tipText);
}
