#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include "mxmodedialog.h"
#include "replacechaindialog.h"
#include <QTimer>
#include "slotitem.h"

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


    slotScene1 = new QGraphicsScene(this);

    // 先设置一个合理的 sceneRect，宽高稍后会被 initSlotsScene 调整
    slotScene1->setSceneRect(0, 0, 900, 160);

    // 把 scene 设置到 UI 中已经存在的 QGraphicsView（slotcase1_View）
    ui->slotcase1_View->setScene(slotScene1);
    ui->slotcase1_View->setRenderHint(QPainter::Antialiasing);
    ui->slotcase1_View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->slotcase1_View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 延迟 0ms 调用 initSlotsScene，保证 UI 完成布局后 fitInView 生效
    QTimer::singleShot(0, this, [this](){
        initSlotScene();

        updateSlotFromProtocol(5, 1, "DJ3", "");         // 传输
        updateSlotFromProtocol(6, 2, "DJ2", "DJ7");      // 测距

    });
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

void MainWindow::initSlotScene()
{
    // 清空旧内容（如果有）
    if (slotScene1) slotScene1->clear();
    for (SlotItem *it : slotItems) { delete it; }
    slotItems.clear();

    const int total = 80;
    const int itemW = 20;      // 单个图形宽
    const int itemH = 20;
    const int spacingX = 8;    // 横向间距
    const int spacingY = 14;    // 纵向间距
    const int perRow = 20;     // 每行放 20 个（可按需要改）

    // 计算场景宽高
    int cols = perRow;
    int rows = (total + perRow - 1) / perRow;
    int sceneW = cols * (itemW + spacingX);
    int sceneH = rows * (itemH + spacingY);

    slotScene1->setSceneRect(0, 0, sceneW, sceneH);

    for (int i = 0; i < total; ++i) {
        SlotItem *it = new SlotItem(i);
        int row = i / perRow;
        int col = i % perRow;
        int offsetY = 30;
        qreal x = col * (itemW + spacingX);
        qreal y = row * (itemH + spacingY) + offsetY;
        it->setPos(x, y);
        it->setZValue(1.0);

        // 默认信息
        it->setInfoText(QString("时隙 %1\n状态：未分配").arg(i));
        it->setSlotType(SLOT_FREE);

        QGraphicsTextItem *num = new QGraphicsTextItem(QString::number(i));
        num->setDefaultTextColor(Qt::black);
        num->setFont(QFont("Microsoft Yahei", 8));
        slotScene1->addItem(num);
        // 放在圆点下方（绝对坐标）
        num->setPos(x + (itemW - num->boundingRect().width())/2.0,  // 水平居中
                    y + itemH - 4);                                // 垂直在圆下方
        // 让编号在上层，确保不会被圆覆盖
        num->setZValue(2.0);
        slotItems.append(it);
        slotScene1->addItem(it);
    }

    // 图例位置（在场景底部）
    int legendY = sceneH + 50;
    // 圆点大小
    int legendR = 7;
    // 工具函数：画一个圆点 + 文本
    auto drawLegend = [&](int x, QColor color, const QString &text){
        // 画圆
        slotScene1->addEllipse(x, legendY, legendR*2, legendR*2,
                               QPen(Qt::black), QBrush(color));

        // 写文字
        QGraphicsTextItem *t = slotScene1->addText(text);
        t->setDefaultTextColor(Qt::black);
        t->setFont(QFont("Microsoft Yahei", 12));
        t->setPos(x + legendR*2 + 5, legendY - 4);
    };
    // 三个图例
    drawLegend(10,  Qt::green,     "传输");
    drawLegend(110, Qt::blue,      "测距");
    drawLegend(210, Qt::lightGray, "未分配");
    // 更新场景高度，包含图例
    slotScene1->setSceneRect(0, 0, sceneW, legendY + 40);

    // 最后把 scene 缩放到 view 中以保持填满（自适应）
    ui->slotcase1_View->fitInView(slotScene1->sceneRect(), Qt::KeepAspectRatio);
    ui->restslot_1label->setText("80");
}

int MainWindow::countFreeSlots()
{
    int count = 0;
    for (SlotItem *it : slotItems)
    {
        if (it->slotType() == SLOT_FREE)
            count++;
    }
    return count;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (slotScene1 && ui->slotcase1_View->scene()) {
        ui->slotcase1_View->fitInView(slotScene1->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::updateSlotFromProtocol(int idx, int typeCode, const QString &fromName, const QString &toName)
{
    if (idx < 0 || idx >= slotItems.size()) return;
    SlotItem *it = slotItems[idx];
    if (!it) return;

    if (typeCode == 0) { // 未分配
        it->setSlotType(SLOT_FREE);
        it->setInfoText(QString("时隙 %1\n状态：未分配").arg(idx));
    } else if (typeCode == 1) { // 传输
        it->setSlotType(SLOT_TX);
        it->setInfoText(QString("时隙 %1\n状态：传输\n发起：%2").arg(idx).arg(fromName));
    } else if (typeCode == 2) { // 测距
        it->setSlotType(SLOT_RANGE);
        it->setInfoText(QString("时隙 %1\n状态：测距\n发起：%2\n目标：%3").arg(idx).arg(fromName).arg(toName));
    }

    int freeCnt = countFreeSlots();
    ui->restslot_1label->setText(QString::number(freeCnt));
}

