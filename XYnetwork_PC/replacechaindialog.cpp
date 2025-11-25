#include "replacechaindialog.h"
#include "ui_replacechaindialog.h"

ReplaceChainDialog::ReplaceChainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceChainDialog)
{
    ui->setupUi(this);
    ui->comboChain->addItem("1级",1);
    ui->comboChain->addItem("2级",2);
    ui->comboChain->addItem("3级",3);
    ui->comboChain->addItem("4级",4);

    connect(ui->btnOK, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

ReplaceChainDialog::~ReplaceChainDialog()
{
    delete ui;
}

int ReplaceChainDialog::selectedChain() const
{
    return ui->comboChain->currentData().toInt();
}
