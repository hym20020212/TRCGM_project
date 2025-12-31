#include "connectdialog.h"
#include "ui_connectdialog.h"
#define DEFAULT_IP "192.168.1.127"
#define DEFAULT_PORT 8889

ConnectDialog::ConnectDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
    // 设置IP和端口的初始默认值
    ui->lineEditIP->setText(DEFAULT_IP);
    ui->lineEditPort->setText(QString::number(DEFAULT_PORT));
    connect(ui->btnOK, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

QString ConnectDialog::getIP() const
{
    return ui->lineEditIP->text();
}

int ConnectDialog::getPort() const
{
    return ui->lineEditPort->text().toInt();
}
