#include "topologyview.h"
#include "qgraphicsview.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPen>
#include <QBrush>
#include <QtMath>
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>

TopologyView::TopologyView(QWidget *parent)
    : QWidget(parent)
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 900, 750);

    m_view = new QGraphicsView(this);
    m_view->setScene(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);
    setLayout(layout);

    QTimer::singleShot(0, this, [this](){
        if (m_view && m_scene)
            m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    });
    setupLegend();
}

QGraphicsScene* TopologyView::scene() const { return m_scene; }

QPointF TopologyView::ellipsePoint(double cx, double cy, double rx, double ry, double t)
{
    return QPointF(cx + rx * qCos(t), cy + ry * qSin(t));
}

void TopologyView::addLabeledCircle(QGraphicsScene* s, double x, double y, double r)
{
    QPen pen(Qt::black);
    QBrush brush(Qt::white);
    s->addEllipse(x - r, y - r, r*2, r*2, pen, brush);
}


void TopologyView::addNode(const QString& name, double x, double y)
{
    NodeItem* item = new NodeItem(name, QPointF(x, y), 12);
    m_scene->addItem(item);
    m_nodes[name] = item;
}

void TopologyView::drawTopology()
{
    m_scene->clear();
    QPen pen(Qt::black, 2);

    //调整这些值可微调布局
    const double sR = 120.0;                 // 小网半径
    const double s1cx = 240.0, s1cy = 170.0; // 小网1 中心
    const double s2cx = 560.0, s2cy = 170.0; // 小网2 中心

    const double bcx = 400.0, bcy = 430.0;   // 大网中心
    const double brx = 320.0, bry = 160.0;   // 大网半轴 (rx, ry)

    // draw small nets (circles)
    m_scene->addEllipse(s1cx - sR, s1cy - sR, sR*2, sR*2, pen);
    QGraphicsTextItem *t3 = m_scene->addText("小网1");
    QFont f1 = t3->font();
    f1.setPointSize(18);
    t3->setFont(f1);
    t3->setPos(s1cx - 22, s1cy - 8);

    m_scene->addEllipse(s2cx - sR, s2cy - sR, sR*2, sR*2, pen);
    QGraphicsTextItem *t4 = m_scene->addText("小网2");
    QFont f2 = t4->font();
    f2.setPointSize(18);
    t4->setFont(f2);
    t4->setPos(s2cx - 22, s2cy - 8);

    // --- draw big net (ellipse) ---
    m_scene->addEllipse(bcx - brx, bcy - bry, brx*2, bry*2, pen);
    QGraphicsTextItem *bigTitle = m_scene->addText("大网");
    QFont ft = bigTitle->font();
    ft.setPointSize(20);   // 这里调整字体大小
    bigTitle->setFont(ft);
    bigTitle->setPos(bcx - 22, bcy - 8);

    const int smallCount = 10;
    for (int i = 0; i < smallCount; ++i) {
        double ang = 1.5 * M_PI * i / smallCount + 0.8*M_PI;
        QPointF p1(s1cx + sR * qCos(ang), s1cy + sR * qSin(ang));
        addNode(QString("DJ%1").arg(i+1), p1.x() , p1.y());

        QPointF p2(s2cx + sR * qCos(ang), s2cy + sR * qSin(ang));
        addNode(QString("DJ%1").arg(i+11), p2.x(), p2.y());
    }

    // --- big net explicit nodes list (BY1..BY6, MX, GZ1, GZ2) ---
    QStringList bigLabels = {"BY6","BY5","BY4","BY3","BY2","BY1","GZ2","GZ1","MX"};
    const int bigCount = bigLabels.size();
    for (int i = 0; i < bigCount; ++i) {
        double ang = 1.5 * M_PI * i / bigCount - 0.2*M_PI;
        QPointF p = ellipsePoint(bcx, bcy, brx, bry, ang);
        addNode(bigLabels.at(i), p.x(), p.y());
    }

    // --- compute tangency-like points and place ZJ nodes ---
    // Compute a parameter t on ellipse pointing roughly from big center to small center.
    auto computeT = [&](double sx, double sy)->double {
        double vx = sx - bcx;
        double vy = sy - bcy;
        return std::atan2(vy / bry, vx / brx);
    };

    double t1 = computeT(s1cx, s1cy);
    QPointF pb1 = ellipsePoint(bcx, bcy, brx, bry, t1);   // big ellipse point
    double ux1 = qCos(t1), uy1 = qSin(t1);
    QPointF ps1(s1cx - sR * ux1, s1cy - sR * uy1);        // small circle boundary

    // place ZJ0 and ZJ1 near the tangent (adjust offsets for visual separation)
    QPointF mid1 = (pb1 + ps1) / 2.0;
    addNode("ZJ0", mid1.x()-58, mid1.y()+10);
    addNode("ZJ1", mid1.x()-16, mid1.y());

    double t2 = computeT(s2cx, s2cy);
    QPointF pb2 = ellipsePoint(bcx, bcy, brx, bry, t2);
    double ux2 = qCos(t2), uy2 = qSin(t2);
    QPointF ps2(s2cx - sR * ux2, s2cy - sR * uy2);

    QPointF mid2 = (pb2 + ps2) / 2.0;
    addNode("ZJ2", mid2.x()+16, mid2.y());
    addNode("ZJ3", mid2.x()+60, mid2.y()+12);
    m_scene->setSceneRect(0, 0, 900, 750);
}

void TopologyView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_view && m_scene) {
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}
void TopologyView::setupLegend()
{

    if (m_legendLayout)
        return;

    m_legendLayout = new QHBoxLayout();
    m_legendLayout->setContentsMargins(5, 5, 5, 5);
    m_legendLayout->setSpacing(20);

    auto makeLegend = [&](const QColor &color, const QString &text) {
        QWidget *icon = new QWidget();
        icon->setFixedSize(16,16);
        icon->setStyleSheet(QString("background:%1; border-radius:8px;")
                                .arg(color.name()));

        QLabel *label = new QLabel(text);
        label->setStyleSheet("font-size:14px;");

        QHBoxLayout *hl = new QHBoxLayout();
        hl->setContentsMargins(0,0,0,0);
        hl->setSpacing(5);
        hl->addWidget(icon);
        hl->addWidget(label);

        QWidget *wrap = new QWidget();
        wrap->setLayout(hl);
        return wrap;
    };

    // 添加三项图例
    m_legendLayout->addWidget(makeLegend(Qt::red,  "未入网"));
    m_legendLayout->addWidget(makeLegend(Qt::green,"已入网"));
    m_legendLayout->addWidget(makeLegend(Qt::blue, "基座节点"));

    // 添加到主布局的底部
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(this->layout());
    if (mainLayout)
        mainLayout->addLayout(m_legendLayout);
}

void TopologyView::updateNodeState(const QString& nodeName, int slot)
{
    if (!m_nodes.contains(nodeName))
        return;

    NodeItem* node = m_nodes[nodeName];

    if (slot == 0)
        node->setState(NodeItem::NotJoined);
    else if (slot == 1)
        node->setState(NodeItem::Joined);
    else
        node->setState(NodeItem::Base);
    m_scene->update();
}
