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


    const quint32 MAX_BYTE = 500;

    // 计算原始输入字符串的字节长度（按UTF-8编码，与最终发送的二进制一致）
    quint32 dataByteLen = data.toUtf8().size();

    // 校验：超过100KB则拦截
    if (dataByteLen > MAX_BYTE) {
        QMessageBox::warning(this, "警告",
                             QString("输入数据超过%1Byte限制！\n当前数据字节长度：%2Byte")
                                 .arg(MAX_BYTE)
                                 .arg(dataByteLen));
        return;
    }

    emit sendDataRequest(target, data);
}

void SendData::on_pushButton_clear_clicked()
{
    ui->textEdit_senddata->clear();
    ui->textEdit_senddata->setFocus();
}
