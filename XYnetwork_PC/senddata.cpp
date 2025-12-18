#include "senddata.h"
#include "ui_senddata.h"
#include <QMessageBox>


SendData::SendData(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SendData)
{
    ui->setupUi(this);
}

SendData::~SendData()
{
    delete ui;
}

void SendData::on_pushButton_senddata_clicked()
{
    QString target = ui->nodeID_comboBox->currentText();
    QString data   = ui->textEdit_senddata->toPlainText().trimmed();

    if (target.isEmpty() || data.isEmpty()) {
        QMessageBox::warning(this, "警告", "节点或数据不能为空！");
        return;
    }

    const quint32 MAX_KB = 100;
    const quint32 MAX_BYTE = MAX_KB * 1024; // 100KB = 102400字节

    // 计算原始输入字符串的字节长度（按UTF-8编码，与最终发送的二进制一致）
    quint32 dataByteLen = data.toUtf8().size();

    // 校验：超过100KB则拦截
    if (dataByteLen > MAX_BYTE) {
        QMessageBox::warning(this, "警告",
                             QString("输入数据超过%1KB限制！\n当前数据字节长度：%2（%3KB）\n最大允许：%4KB")
                                 .arg(MAX_KB)
                                 .arg(dataByteLen)
                                 .arg(dataByteLen / 1024.0, 0, 'f', 1) // 保留1位小数，更直观
                                 .arg(MAX_KB));
        return;
    }

    emit sendDataRequest(target, data);
}

void SendData::on_pushButton_clear_clicked()
{
    ui->textEdit_senddata->clear();
    ui->textEdit_senddata->setFocus();
}
