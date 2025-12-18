#include "mxmodedialog.h"
#include "ui_mxmodedialog.h"

MXmodeDialog::MXmodeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MXmodeDialog)
{
    ui->setupUi(this);
    // 添加运行模式选项
    ui->comboMode->addItem("全自主模式");
    ui->comboMode->addItem("上位机接管模式");

    // 按钮绑定
    connect(ui->btnOK, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

MXmodeDialog::~MXmodeDialog()
{
    delete ui;
}

QString MXmodeDialog::selectedMode() const
{
    return ui->comboMode->currentText();
}

quint8 MXmodeDialog::selectedModeByte() const
{
    // 根据选中项返回对应字节：全自主=0x00，上位机接管=0x01
    return (ui->comboMode->currentIndex() == 0) ? 0x00 : 0x01;
}
