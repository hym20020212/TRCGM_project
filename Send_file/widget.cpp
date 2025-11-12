#include "widget.h"
#include "ui_widget.h"
#include <QRandomGenerator>
#include <QDir>
#include <QThread>
#include <QStringConverter>
#include <QFile>
#include <cstdint>
#include "transferrate.h"
#include <cstdlib>
#include "connectconfigdialog.h"
#include "snrwidget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_isConnected(false)  // 初始未连接
    , m_totalRecvBytes(0)
    , m_isReceiving(false)
    , m_currentIp("")
    , m_currentPort(0)
    , m_totalSentBytes(0)
{
    ui->setupUi(this);
    setWindowTitle("文件发送工具");


    // 初始化传输速率界面
    m_transferRate = new Transferrate(ui->widget_rate);
    m_transferRate->resize(ui->widget_rate->size());

    // 初始化SNR显示界面
    m_snrWidget = new SnrWidget(ui->widget_snr);
    m_snrWidget->resize(ui->widget_snr->size());

    // 初始化SNR显示界面
    m_bitErrorWidget = new BitErrorRateWidget(ui->widget_bite_error);
    m_bitErrorWidget->resize(ui->widget_bite_error->size());


    // 创建传输速率界面
    Transferrate* rateWidget = new Transferrate(ui->widget_rate);
    rateWidget->resize(ui->widget_rate->size());

    // 初始化速率计算定时器（每秒计算一次）
    m_rateTimer = new QTimer(this);
    m_rateTimer->setInterval(1000);  // 1秒间隔
    connect(m_rateTimer, &QTimer::timeout, this, &Widget::calculateRates);

    // 初始化误码率计算定时器（每秒计算一次）
    m_berTimer = new QTimer(this);
    m_berTimer->setInterval(1000);
    connect(m_berTimer, &QTimer::timeout, this, &Widget::calculateBitErrorRates);

    // 初始化统计类：绑定UI显示控件
    m_errorStat = new ErrorStatistic(
        ui->label_bitErrorValue,    // 误码率显示控件
        ui->label_byteErrorValue,   // 误Byte率显示控件
        ui->label_frameErrorValue,  // 误帧率显示控件
        this
        );

    // 初始化TCP套接字
    m_tcpSocket = new QTcpSocket(this);
    // 关联TCP连接/断开信号
    connect(m_tcpSocket, &QTcpSocket::connected, this, &Widget::onTcpConnected);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &Widget::onTcpDisconnected);
    connect(m_tcpSocket, &QTcpSocket::errorOccurred, this, [=](QAbstractSocket::SocketError error) {
        printSendLog("连接错误：" + m_tcpSocket->errorString());
        updateWidgetEnabledState(false);  // 错误时禁用后续功能
    });
    // 关联接收数据信号
     connect(m_tcpSocket, &QTcpSocket::readyRead, this, &Widget::onReadyRead);

    // 初始化日志框（只读）
    ui->textEdit_sendlog->setReadOnly(true);

    ui->label_sendbyte_value->setText("0");
    ui->label_recvbyte_value->setText("0");

    // 初始化SNR显示界面
    SnrWidget* snrWidget = new SnrWidget(ui->widget_snr);
    snrWidget->resize(ui->widget_snr->size());

    // 初始化SNR计算定时器（每秒计算一次）
    m_snrTimer = new QTimer(this);
    m_snrTimer->setInterval(1000);
    connect(m_snrTimer, &QTimer::timeout, this, &Widget::calculateRealSnr);

    // 初始禁用后续功能（未连接时）
    updateWidgetEnabledState(false);
}

Widget::~Widget()
{
    if (m_tcpSocket->isOpen()) {
        m_tcpSocket->close();
    }
    delete m_file;
    delete ui;
}

void Widget::on_pushButton_selectFile_clicked()
{
    // 打开文件选择对话框（过滤文本文件，可根据需要修改）
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择文件",
        QDir::currentPath(),
        "文本文件 (*.txt);;所有文件 (*.*)"
        );

    if (!filePath.isEmpty()) {
        m_filePath = filePath;
        ui->lineEdit_filePath->setText(filePath);  // 显示文件路径
        printSendLog("已选择文件：" + filePath);
    }
}

bool isBinaryStringFile(const QString &filePath) {
    return QFileInfo(filePath).suffix().compare("binstr", Qt::CaseInsensitive) == 0;
}

void Widget::on_pushButton_send_clicked()
{
    // 检查文件是否存在
    if (m_filePath.isEmpty()) {
        printSendLog("错误：请先选择文件");
        return;
    }

    // 打开文件
    m_file = new QFile(m_filePath);
    if (!m_file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        printSendLog("错误：文件打开失败 - " + m_file->errorString());
        delete m_file;
        m_file = nullptr;
        return;
    }

    // 启动速率和SNR监控（确保对象已初始化）
    if (m_transferRate && m_snrWidget) {
        m_transferRate->startTransfer();
        m_rateTimer->start();
        m_snrWidget->startSnrMonitoring();
        m_snrTimer->start();
    }

    // 开始传输时，启动SNR监控
    m_snrWidget->startSnrMonitoring();
    m_snrTimer->start();

    m_bitErrorWidget->startMonitoring();
    m_berTimer->start();

    printSendLog("开始发送文件..."+ m_filePath);
    printSendLog("发送时间：" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    // 清空之前的发送数据缓存（用于统计比对）
    m_sendData.clear();

    if (isBinaryStringFile(m_filePath))
    {
    char binary_str[33] = {0};  // 存储每行32位二进制字符串
    qint64 readLen = 0;

    while ((readLen = m_file->readLine(binary_str, 33)) > 0) {
        // 去除换行符
        QString line = QString(binary_str).trimmed();
        if (line.length() != 32) {
            printSendLog("警告：跳过无效行（长度不是32位）");
            memset(binary_str, 0, 33);  // 清空缓冲区
            continue;
        }

        // 转换32位二进制字符串为uint32_t
        uint32_t num = 0;
        for (int i = 0; i < 32; ++i) {
            if (line[i] == '1') {
                num |= (1 << (31 - i));  // 高位在前
            }
        }

        // 缓存发送的uint32_t数据（用于后续比对）
        m_sendData.append(num);
        qint64 sent = m_tcpSocket->write((char*)&num, sizeof(num));
        if (sent > 0) {
            m_totalSentBytes += sent;
            ui->label_sendbyte_value->setText(QString::number(m_totalSentBytes));
        }

        // 发送uint32_t数据（原始内存）
        m_tcpSocket->flush();  // 立即发送
    }

    // 发送结束标志（0xFFFFFFFF）
    uint32_t end_marker = 0xFFFFFFFF;
    qint64 sent = m_tcpSocket->write((char*)&end_marker, sizeof(end_marker));
    if (sent > 0) {
        m_totalSentBytes += sent;
        ui->label_sendbyte_value->setText(QString::number(m_totalSentBytes));
      }
    }
    else {
        // 方式2：处理二进制文件（视频/图片等）
        const int BUFFER_SIZE = 4096; // 缓冲区大小，可调整
        char buffer[BUFFER_SIZE];
        qint64 bytesRead = 0;

        while ((bytesRead = m_file->read(buffer, BUFFER_SIZE)) > 0) {
            qint64 sent = m_tcpSocket->write(buffer, bytesRead);
            if (sent > 0) {
                m_totalSentBytes += sent;
                ui->label_sendbyte_value->setText(QString::number(m_totalSentBytes));
                m_sentBytesHistory.append(sent);

            }
            // 发送二进制数据
            m_tcpSocket->flush();
        }
        // 二进制文件结束标志（需与服务端约定，例如发送文件大小）
        uint32_t end_marker = 0xFFFFFFFF;
        qint64 sent = m_tcpSocket->write((char*)&end_marker, sizeof(end_marker));
        if (sent > 0) {
            m_totalSentBytes += sent;
            ui->label_sendbyte_value->setText(QString::number(m_totalSentBytes));
        }
    }

    // 关闭文件
    if (m_file) {
        m_file->close();
        delete m_file;
        m_file = nullptr;
    }

    // 将发送数据设置为统计基准（关键：用于后续比对接收数据）
    m_errorStat->setReferenceData(m_sendData);

    printSendLog("文件发送完成，等待回传...");
    printSendLog("发送结束时间：" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    if (m_transferRate && m_snrWidget) {
        m_rateTimer->stop();
        m_transferRate->stopTransfer();
        m_snrTimer->stop();
        m_snrWidget->stopSnrMonitoring();
    }
}

void Widget::calculateRates()
{
    // 计算1秒平均速率（最近1秒的总字节数）
    double rate1s = m_sentBytesHistory.isEmpty() ? 0 :
                        m_sentBytesHistory.last();  // 最后1秒的发送量

    // 计算5秒平均速率（最近5秒的总字节数/5）
    double rate5s = 0;
    int count5s = qMin(5, m_sentBytesHistory.size());
    for (int i = 0; i < count5s; ++i) {
        rate5s += m_sentBytesHistory[m_sentBytesHistory.size() - 1 - i];
    }
    rate5s /= count5s;

    // 计算10秒平均速率（最近10秒的总字节数/10）
    double rate10s = 0;
    int count10s = qMin(10, m_sentBytesHistory.size());
    for (int i = 0; i < count10s; ++i) {
        rate10s += m_sentBytesHistory[m_sentBytesHistory.size() - 1 - i];
    }
    rate10s /= count10s;

    // 更新图表
    m_transferRate->updateRates(rate1s, rate5s, rate10s);
    QApplication::processEvents();
}

// TCP连接状态变化处理
void Widget::onSocketStateChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::UnconnectedState:
        printSendLog("状态：未连接");
        break;
    case QAbstractSocket::ConnectingState:
        printSendLog("状态：正在连接...");
        break;
    case QAbstractSocket::ConnectedState:
        printSendLog("状态：已连接到服务器");
        break;
    default:
        break;
    }
}

// 接收服务器回传数据
void Widget::onReadyRead()
{
    // 1. 读取当前批次的所有数据（支持分批次接收）
    QByteArray newData = m_tcpSocket->readAll();
    if (newData.isEmpty()) {
        printRecvLog("未接收到有效数据");
        return;
    }

    // 2. 首次接收时标记开始状态
    if (!m_isReceiving) {
        m_isReceiving = true;
        m_recvDataBuffer.clear();  // 清空历史缓存
        m_totalRecvBytes = 0;      // 重置累计长度
        printRecvLog("开始接收服务器数据...");
        // 重置统计（新接收周期）
        m_errorStat->reset();
    }

    // 3. 缓存数据并累计总长度
    m_recvDataBuffer.append(newData);
    m_totalRecvBytes += newData.size();
    ui->label_recvbyte_value->setText(QString::number(m_totalRecvBytes));

    // 4. 实时打印当前批次数据的二进制字符串（按uint32_t解析）
    QVector<uint32_t> currentRecvFrames;  // 当前批次接收的帧
    int newUint32Count = newData.size() / sizeof(uint32_t);
    const uint32_t* newDataPtr = reinterpret_cast<const uint32_t*>(newData.data());

    for (int i = 0; i < newUint32Count; ++i) {
        uint32_t num = newDataPtr[i];
        // 实时显示当前uint32_t的32位二进制字符串（跳过结束标志）
        if (num != 0xFFFFFFFF) {
            QString binaryStr;
            for (int bit = 31; bit >= 0; --bit) {
                binaryStr += (num & (1 << bit)) ? '1' : '0';
            }
            printRecvLog(binaryStr);  //显示二进制字符串

            // 添加到当前批次帧列表（用于统计比对）
            currentRecvFrames.append(num);
        } else {
            // 提前检测到结束标志，无需继续显示
            break;
        }
    }

    // 5. 调用统计类比对当前批次数据（实时更新错误率）
    if (!currentRecvFrames.isEmpty()) {
        m_errorStat->addReceivedData(currentRecvFrames);
    }

    // 6. 检查缓存中是否包含结束标志（0xFFFFFFFF）
    bool hasEndMarker = false;
    int totalUint32Count = m_recvDataBuffer.size() / sizeof(uint32_t);
    const uint32_t* allDataPtr = reinterpret_cast<const uint32_t*>(m_recvDataBuffer.data());

    for (int i = 0; i < totalUint32Count; ++i) {
        if (allDataPtr[i] == 0xFFFFFFFF) {
            hasEndMarker = true;
            break;
        }
    }

    // 6. 若检测到结束标志，处理完整数据并保存
    if (hasEndMarker) {
        processAndSaveReceivedData();
    }

    m_currentSnr = calculateSnr(newData);  // 自定义函数：根据接收数据计算SNR
    m_realTimeBer = m_errorStat->getRealTimeBitErrorRate();
    m_cumulativeBer = m_errorStat->getCumulativeBitErrorRate();
}

double Widget::calculateSnr(const QByteArray &data)
{
    // 示例：模拟SNR计算（实际需根据接收数据的信号和噪声强度计算）
    // 这里仅作演示，需替换为真实算法
    return 20 + (rand() % 20);  // 模拟SNR在20~40dB之间波动
}

void Widget::calculateRealSnr()
{
    // 更新SNR显示
    m_snrWidget->updateSnr(m_currentSnr);
    QApplication::processEvents();
}

//处理完整数据并保存（提取结束标志前的有效数据）
void Widget::processAndSaveReceivedData()
{
    const QString savePath = "recv_from_server_utf8.txt";
    QFile recvFile(savePath);

    // 以覆盖模式打开（只保存当前完整接收的数据）
    if (!recvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        printRecvLog("错误：无法打开文件 " + savePath + " - " + recvFile.errorString());
        // 重置状态，允许下次接收
        resetReceiveState();
        return;
    }

    QTextStream out(&recvFile);
    out.setEncoding(QStringConverter::Utf8);

    // 解析缓存中的所有数据，提取结束标志前的有效内容
    int totalUint32Count = m_recvDataBuffer.size() / sizeof(uint32_t);
    const uint32_t* dataPtr = reinterpret_cast<const uint32_t*>(m_recvDataBuffer.data());
    qint64 validBytes = 0;  // 有效数据（不含结束标志）的总字节数

    for (int i = 0; i < totalUint32Count; ++i) {
        uint32_t num = dataPtr[i];
        if (num == 0xFFFFFFFF) {
            printRecvLog("检测到结束标志，停止处理");
            break;  // 遇到结束标志，终止解析
        }

        // 转换为32位二进制字符串并写入文件
        QString binaryStr;
        for (int bit = 31; bit >= 0; --bit) {
            binaryStr += (num & (1 << bit)) ? '1' : '0';
        }
        out << binaryStr << "\n";
        validBytes += sizeof(uint32_t);  // 累计有效字节数
    }

    // 关闭文件并打印总结信息
    recvFile.close();
    printRecvLog("数据接收完毕！");
    printRecvLog("有效数据长度：" + QString::number(validBytes) + "字节");
    printRecvLog("数据已保存到：" + savePath);

    // 重置接收状态，准备下次接收
    resetReceiveState();
}

// 新增：重置接收状态变量
void Widget::resetReceiveState()
{
    m_isReceiving = false;
    m_recvDataBuffer.clear();
    m_totalRecvBytes = 0;
    ui->label_recvbyte_value->setText("0"); // 重置显示为0
}


// 错误处理
void Widget::onErrorOccurred(QAbstractSocket::SocketError error)
{
    printSendLog("错误：" + m_tcpSocket->errorString());
}

// 打印发送日志到textEdit_sendlog
void Widget::printSendLog(const QString &log)
{
    ui->textEdit_sendlog->append(log);  // 添加日志
    // 自动滚动到最新行
    QTextCursor cursor = ui->textEdit_sendlog->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit_sendlog->setTextCursor(cursor);
}

// 打印“接收日志”到textEdit_recvLog
void Widget::printRecvLog(const QString &log)
{
    ui->textEdit_recvlog->append(log);
    // 自动滚动到最新行
    QTextCursor cursor = ui->textEdit_recvlog->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit_recvlog->setTextCursor(cursor);
}

// 初始化TCP连接（包含重连逻辑）
bool Widget::connectToServer(const QString &ip, quint16 port)
{
    m_tcpSocket->connectToHost(ip, port);
    // 等待连接结果（超时3秒）
    return m_tcpSocket->waitForConnected(3000);
}

void Widget::on_pushButton_generate_clicked()
{
    // 弹出生成文件的新窗口
    GenerateFileDialog dialog(this);
    // 等待用户操作（点击确定或取消）
    if (dialog.exec() == QDialog::Accepted) {
        // 生成成功，获取文件路径并更新主窗口
        QString generatedPath = dialog.getGeneratedFilePath();
        if (!generatedPath.isEmpty()) {
            ui->lineEdit_filePath->setText(generatedPath);
            m_filePath = generatedPath;
            printSendLog("文件生成成功：" + generatedPath);
        }
    }
}

void Widget::on_pushButton_connect_clicked()
{

    // 若已连接，点击则断开
    if (m_isConnected) {
        m_tcpSocket->disconnectFromHost();
        return;
    }
    // 未连接：弹出IP端口配置窗口
    ConnectConfigDialog configDlg(this); // 创建配置窗口
    int dialogResult = configDlg.exec(); // 显示模态窗口，等待用户操作

    // 仅当用户点击“确认”按钮时，执行连接逻辑
    if (dialogResult == QDialog::Accepted) {
        // 从配置窗口获取输入的IP和端口
        QString ip = configDlg.getInputIp();
        QString portStr = QString::number(configDlg.getInputPort());
        quint16 port = configDlg.getInputPort();
        m_currentIp = configDlg.getInputIp();
        m_currentPort = configDlg.getInputPort();


    // 获取并校验IP
    if (ip.isEmpty()) {
        printSendLog("错误：IP地址不能为空");
        return;
    }

    // 获取并校验端口号（核心修改）
    if (portStr.isEmpty()) {
        printSendLog("错误：端口号不能为空");
        return;
    }
    bool isNumber;
    portStr.toUInt(&isNumber);  // 转换为无符号整数
    if (!isNumber || port < 1 || port > 65535) {  // 校验范围
        printSendLog("错误：端口号必须是1~65535的整数");
        return;
    }

    // 发起连接
    printSendLog("正在连接 " + ip + ":" + portStr + "...");
    if (connectToServer(ip, port)) {
        // 连接操作已发起（结果由connected/disconnected信号通知）
    }
 }
}

// 连接成功时触发
void Widget::onTcpConnected()
{
    m_isConnected = true;
    printSendLog("连接成功！服务器IP：" + m_currentIp +
             "，端口：" + QString::number(m_currentPort));
    updateWidgetEnabledState(true);  // 启用后续功能
}

// 连接断开时触发
void Widget::onTcpDisconnected()
{
    m_isConnected = false;
    m_totalSentBytes = 0;
    ui->label_sendbyte_value->setText("0");
    ui->label_recvbyte_value->setText("0");
    printSendLog("已断开连接");
    m_snrWidget->stopSnrMonitoring();
    m_snrTimer->stop();
    m_bitErrorWidget->stopMonitoring();
    m_berTimer->stop();
    updateWidgetEnabledState(false);  // 禁用后续功能
}

// 更新控件启用状态
void Widget::updateWidgetEnabledState(bool isConnected)
{
    // 连接成功后，启用文件相关功能；否则禁用
    ui->lineEdit_filePath->setEnabled(isConnected);
    ui->pushButton_selectFile->setEnabled(isConnected);
    ui->pushButton_generate->setEnabled(isConnected);
    ui->pushButton_send->setEnabled(isConnected);
    ui->pushButton_openManualDialog->setEnabled(isConnected);

    if (!isConnected) {
        ui->lineEdit_filePath->clear();
        m_filePath.clear();  // 清空成员变量
    }
}

void Widget::on_pushButton_openManualDialog_clicked()
{
    if (!m_isConnected) {
        printSendLog("错误：请先连接服务器");
        return;
    }

    // 若对话框未创建则创建，否则显示
    if (!m_manualDialog) {
        // 传入TCP套接字和累计发送字节数的引用
        m_manualDialog = new ManualInputDialog(m_tcpSocket, m_totalSentBytes, this);
    }
    m_manualDialog->show(); // 显示对话框
}

void Widget::calculateBitErrorRates()
{
    // 更新误码率显示
    m_bitErrorWidget->updateErrorRates(m_realTimeBer, m_cumulativeBer);
}
