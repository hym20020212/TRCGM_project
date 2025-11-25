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
