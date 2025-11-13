#include "manualinputdialog.h"
#include "ui_manualinputdialog.h"
#include <QMessageBox>

ManualInputDialog::ManualInputDialog(QTcpSocket *tcpSocket, qint64 &totalSentBytes, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ManualInputDialog)
    , m_tcpSocket(tcpSocket)
    , m_totalSentBytes(totalSentBytes)
{
    ui->setupUi(this);
    setWindowTitle("手动输入数据");
    // 设置编辑框为多行输入
    ui->textEdit_manualData->setAcceptRichText(false);
    ui->textEdit_manualData->setPlaceholderText("请输入要发送的数据...");
}

ManualInputDialog::~ManualInputDialog()
{
    delete ui;
}

void ManualInputDialog::on_pushButton_sendManual_clicked()
{
    // 检查TCP连接状态
    if (!m_tcpSocket || m_tcpSocket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "错误", "未连接到服务器，请先连接");
        return;
    }

    // 获取输入数据
    QString data = ui->textEdit_manualData->toPlainText().trimmed();
    if (data.isEmpty()) {
        QMessageBox::warning(this, "错误", "输入数据不能为空");
        return;
    }

    // 转换为字节流并发送
    QByteArray sendData = data.toUtf8();
    qint64 bytesSent = m_tcpSocket->write(sendData);
    if (bytesSent == -1) {
        QMessageBox::warning(this, "发送失败", m_tcpSocket->errorString());
    } else {
        m_totalSentBytes += bytesSent; // 更新累计发送字节数
        QMessageBox::information(this, "发送成功",
                                 QString("数据发送成功！\n发送字节数：%1")
                                     .arg(bytesSent));
        ui->textEdit_manualData->clear(); // 清空输入框
    }
}

