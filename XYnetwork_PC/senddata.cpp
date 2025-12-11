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

    emit sendDataRequest(target, data);
}

void SendData::on_pushButton_clear_clicked()
{
    ui->textEdit_senddata->clear();
    ui->textEdit_senddata->setFocus();
}
