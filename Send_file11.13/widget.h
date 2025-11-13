#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QFileDialog>
#include <QFile>
#include <QHostAddress>
#include <QDateTime>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "generatefiledialog.h"
#include "errorstatistic.h"
#include "transferrate.h"
#include "snrwidget.h"
#include "manualinputdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_selectFile_clicked();

    void on_pushButton_send_clicked();
    // TCP连接状态变化槽函数
    void onSocketStateChanged(QAbstractSocket::SocketState state);

    // 读取服务器返回数据
    void onReadyRead();

    // 连接错误处理
    void onErrorOccurred(QAbstractSocket::SocketError error);

    void on_pushButton_generate_clicked();
    //连接服务器槽函数
    void on_pushButton_connect_clicked();
    void onTcpConnected();
    void onTcpDisconnected();
    void on_pushButton_openManualDialog_clicked();

private:
    Ui::Widget *ui;
    QTcpSocket *m_tcpSocket;  // TCP套接字
    QFile *m_file;            // 待发送的文件
    QString m_filePath;       // 选中的文件路径
    ErrorStatistic* m_errorStat;  // 统计类对象指针
    QVector<uint32_t> m_sendData; // 缓存发送的文件数据（用于传给统计类）
    QString m_currentIp;
    quint16 m_currentPort;
    Transferrate *m_transferRate;  // 速率图表对象
    QTimer *m_rateTimer;           // 计算速率的定时器
    QList<qint64> m_sentBytesHistory;  // 存储每秒发送的字节数
    QDateTime m_rateStartTime;        // 速率统计开始时间
    SnrWidget *m_snrWidget;      // SNR显示界面
    QTimer *m_snrTimer;          // SNR计算定时器
    ManualInputDialog *m_manualDialog = nullptr;

    double m_currentSnr;         // 实时SNR值
    double calculateSnr(const QByteArray &data);


    bool m_isConnected;// 记录连接状态
    void printSendLog(const QString &log);
    void printRecvLog(const QString &log);
    bool connectToServer(const QString &ip, quint16 port);// 初始化TCP连接
    bool generateBinaryFile(const QString &filePath, qint64 totalBytes, double zeroRatio);//生成随机二进制文件的核心函数
    void updateWidgetEnabledState(bool isConnected);//更新UI控件的启用状态
    void processAndSaveReceivedData();  // 处理并保存接收的完整数据
    void resetReceiveState();           // 重置接收状态变量
    void calculateRates();
    void calculateRealSnr();

    qint64 calculateLineCount(qint64 totalBytes);
    QByteArray m_recvDataBuffer;  // 缓存接收到的数据
    qint64 m_totalRecvBytes;      // 累计接收的总字节数
    bool m_isReceiving;           // 是否处于接收状态（用于判断是否开始新的接收）
    qint64 m_totalSentBytes = 0;

};
#endif // WIDGET_H
