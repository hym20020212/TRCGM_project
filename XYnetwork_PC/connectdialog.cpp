#include "connectdialog.h"
#include "ui_connectdialog.h"

ConnectDialog::ConnectDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
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
