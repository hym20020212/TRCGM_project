#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include "mxmodedialog.h"
#include "replacechaindialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);
    connect(ui->actionConnect, &QAction::triggered,
            this, &MainWindow::onActionConnectTriggered);

    connect(ui->actionMxMode, &QAction::triggered,
            this, &MainWindow::onActionMxModeTriggered);
    connect(ui->actionReplace, &QAction::triggered,
            this, &MainWindow::onActionReplaceTriggered);

    QVBoxLayout *lay = new QVBoxLayout(ui->netTopologyView);
    lay->setContentsMargins(0,0,0,0);
    ui->netTopologyView->setLayout(lay);

    TopologyView *tp = new TopologyView(this);
    lay->addWidget(tp);

    tp->drawTopology();



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onActionConnectTriggered()
{
    ConnectDialog dialog(this);

    // 阻塞式显示对话框
    if (dialog.exec() == QDialog::Accepted)
    {
        QString ip = dialog.getIP();
        int port = dialog.getPort();

        bool ok = connectToNode(ip, port);

        if (ok)
            ui->tip_textEdit->append("连接成功!");
        else
            ui->tip_textEdit->append("连接失败!");
    }
    else
    {
        ui->tip_textEdit->append("取消连接");
    }
}

bool MainWindow::connectToNode(const QString &ip, int port)
{
    if (tcpSocket->isOpen())
        tcpSocket->close();

    tcpSocket->connectToHost(ip, port);

    // 等待 3 秒建立连接
    if (tcpSocket->waitForConnected(3000))
    {
        ui->tip_textEdit->append("TCP 连接成功: " + ip + ":" + QString::number(port));
        return true;
    }
    else
    {
        ui->tip_textEdit->append("TCP 连接失败: " + tcpSocket->errorString());
        return false;
    }
}

void MainWindow::onActionMxModeTriggered()
{
    MXmodeDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString mode = dialog.selectedMode();
        ui->tip_textEdit->append("进入" + mode);
    }
    else
    {
        ui->tip_textEdit->append("取消设置运行模式");
    }
}

void MainWindow::onActionReplaceTriggered()
{
    ReplaceChainDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        int chainLevel = dialog.selectedChain();
        ui->tip_textEdit->append("当前为 " + QString::number(chainLevel) + " 级替换链");
    }
    else
    {
        ui->tip_textEdit->append("取消设置替换链");
    }
}
