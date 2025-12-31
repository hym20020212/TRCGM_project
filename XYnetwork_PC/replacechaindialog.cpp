#include "replacechaindialog.h"
#include "ui_replacechaindialog.h"
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QtMath>
#include <QMouseEvent>
#include <QDateTime>
#include <QGraphicsPolygonItem>

ReplaceChainDialog::ReplaceChainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceChainDialog)
{
    ui->setupUi(this);

    m_scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(m_scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->installEventFilter(this);
    drawNodes();
}

ReplaceChainDialog::~ReplaceChainDialog()
{
    delete ui;
}

void ReplaceChainDialog::drawNodes()
{
    // MX、GZ
    m_nodes["MX"]  = new NodeItem("MX",  QPointF(300, -120), 12);
    m_nodes["GZ1"] = new NodeItem("GZ1", QPointF(200, -50), 12);
    m_nodes["GZ2"] = new NodeItem("GZ2", QPointF(400, -50), 12);

    // BY
    QStringList by = {"BY1","BY2","BY3","BY4","BY5","BY6"};
    int byCount = by.size();
    int spacing = 150;        // 水平间距
    int centerX = 325;        // scene 宽 650 / 2
    int byY = 50;
    int byTotalWidth = (byCount - 1) * spacing;
    int byStartX = centerX - byTotalWidth / 2;

    for (int i = 0; i < byCount; ++i) {
        m_nodes[by[i]] =
            new NodeItem(by[i],
                         QPointF(byStartX + i * spacing, byY),
                         12);
    }
    int djRows = 4;
    int djCols = 5;
    int djCount = 20;
    int djStartIndex = 1;     // DJ1
    int djRowSpacing = 70;    // 行间距
    int djFirstRowY = byY + 80;
    int djTotalWidth = (djCols - 1) * spacing;
    int djStartX = centerX - djTotalWidth / 2;
    int index = 0;
    for (int row = 0; row < djRows; ++row) {
        for (int col = 0; col < djCols; ++col) {

            if (index >= djCount)
                break;

            QString name = QString("DJ%1").arg(djStartIndex + index);

            QPointF pos(djStartX + col * spacing,
                        djFirstRowY + row * djRowSpacing);

            m_nodes[name] = new NodeItem(name, pos, 10);

            ++index;
        }
    }
    for (auto n : m_nodes.values()) {
        m_scene->addItem(n);
    }

    m_scene->setSceneRect(0,0,650,350);
}

bool ReplaceChainDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->graphicsView && event->type() == QEvent::MouseButtonPress) {
        auto *me = static_cast<QMouseEvent*>(event);
        QPointF scenePos = ui->graphicsView->mapToScene(me->pos());

        QGraphicsItem *item = m_scene->itemAt(scenePos, QTransform());
        NodeItem *node = dynamic_cast<NodeItem*>(item);

        if (!node)
            return false;

        if (!m_firstSelected) {
            m_firstSelected = node;
        } else {
            addArrow(m_firstSelected, node);
            m_firstSelected = nullptr;
        }
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

void ReplaceChainDialog::addArrow(NodeItem *from, NodeItem *to)
{
    QPointF p1 = from->sceneBoundingRect().center();
    QPointF p2 = to->sceneBoundingRect().center();
    QLineF line(p1, p2);
    QGraphicsLineItem *arrow = m_scene->addLine(line, QPen(Qt::black, 2));

    // 箭头头部
    // double angle = std::atan2(-line.dy(), line.dx());
    // QPointF arrowP1 = p2 + QPointF(sin(angle + M_PI / 3) * 10,
    //                                cos(angle + M_PI / 3) * 10);
    // QPointF arrowP2 = p2 + QPointF(sin(angle + M_PI - M_PI / 3) * 10,
    //                                cos(angle + M_PI - M_PI / 3) * 10);

    // QPolygonF arrowHead;
    // arrowHead << p2 << arrowP1 << arrowP2;
    // m_scene->addPolygon(arrowHead, QPen(Qt::black), QBrush(Qt::black));

    // m_arrows << arrow;
    double arrowSize = 10; // 箭头大小（可按需调整）
    double angle = std::atan2(line.dy(), line.dx()); // 修正角度计算（匹配示例图方向）

    // 计算箭头两个端点（形成指向to的单三角）
    QPointF arrowP1 = p2 - QPointF(
                          cos(angle - M_PI / 6) * arrowSize,
                          sin(angle - M_PI / 6) * arrowSize
                          );
    QPointF arrowP2 = p2 - QPointF(
                          cos(angle + M_PI / 6) * arrowSize,
                          sin(angle + M_PI / 6) * arrowSize
                          );

    // 构造箭头多边形（仅包含箭头头部，无多余线条）
    QPolygonF arrowHead;
    arrowHead << p2 << arrowP1 << arrowP2;

    // 绘制箭头头部（填充黑色，无轮廓）
    QGraphicsPolygonItem* arrowHeadItem = m_scene->addPolygon(
        arrowHead,
        QPen(Qt::NoPen), // 无轮廓线
        QBrush(Qt::black) // 填充黑色
        );

    m_arrows << arrow;
}

void ReplaceChainDialog::on_pushButtonReset_clicked()
{
    // 删除所有箭头线
    for (QGraphicsLineItem* arrow : m_arrows) {
        m_scene->removeItem(arrow); // 从场景移除
        delete arrow;               // 释放内存
    }
    m_arrows.clear(); // 清空箭头列表
    QList<QGraphicsItem*> allItems = m_scene->items();
    for (QGraphicsItem* item : allItems) {
        QGraphicsPolygonItem* arrowHead = dynamic_cast<QGraphicsPolygonItem*>(item);
        if (arrowHead) {
            m_scene->removeItem(arrowHead);
            delete arrowHead;
        }
    }
    //重置选中状态
    m_firstSelected = nullptr;
}

//Confirm按钮：发送确认信号
void ReplaceChainDialog::on_pushButtonConfirm_clicked()
{
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString tipText = QString("[%1]：替换链设置完成").arg(currentTime);
    emit replaceChainConfirmed(tipText);
    this->close();
}
