#ifndef MANUALINPUTDIALOG_H
#define MANUALINPUTDIALOG_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class ManualInputDialog;
}

class ManualInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManualInputDialog(QTcpSocket *tcpSocket, qint64 &totalSentBytes, QWidget *parent = nullptr);
    ~ManualInputDialog();

private slots:
    void on_pushButton_sendManual_clicked();

private:
    Ui::ManualInputDialog *ui;
    QTcpSocket *m_tcpSocket;          // 关联主窗口的TCP套接字
    qint64 &m_totalSentBytes;         // 引用主窗口的累计发送字节数
};

#endif // MANUALINPUTDIALOG_H
