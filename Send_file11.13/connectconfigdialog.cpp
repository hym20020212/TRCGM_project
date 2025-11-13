#include "connectconfigdialog.h"
#include "ui_connectconfigdialog.h"
#include "widget.h"
#include <cstdint>
#include <cstdlib>

ConnectConfigDialog::ConnectConfigDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConnectConfigDialog)
{
    ui->setupUi(this);
    ui->setupUi(this);
    // 初始化IP和端口控件
    ui->lineEdit_ip->setText("192.168.8.3");  // 默认IP
    ui->lineEdit_port->setText("8889");         // 默认端口
    // 限制端口输入框只能输入数字（通过正则表达式）
    QRegularExpression regExp("^[0-9]+$");
    ui->lineEdit_port->setValidator(new QRegularExpressionValidator(regExp, this));
    // 设置窗口标题
    this->setWindowTitle("连接配置");
    // 取消按钮绑定：关闭窗口
    connect(ui->pushButton_cancel, &QPushButton::clicked, this, &QDialog::close);
    // 确认按钮绑定：关闭窗口并返回“确认”状态
    connect(ui->pushButton_confirm, &QPushButton::clicked, this, [this](){
        this->accept(); // 使exec()返回QDialog::Accepted
    });
}

ConnectConfigDialog::~ConnectConfigDialog()
{
    delete ui;
}

QString ConnectConfigDialog::getInputIp() const
{
    return ui->lineEdit_ip->text().trimmed(); // 去除空格
}

// 获取输入的端口号（转换为16位无符号整数）
quint16 ConnectConfigDialog::getInputPort() const
{
    return ui->lineEdit_port->text().toUShort(); // 自动过滤非数字字符
}

