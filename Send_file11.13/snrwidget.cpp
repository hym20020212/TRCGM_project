#include "snrwidget.h"
#include "ui_snrwidget.h"
#include <QPainter>

SnrWidget::SnrWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SnrWidget)
{
    ui->setupUi(this);
    // 初始化图表
    m_chart = new QChart();
    ui->chartView->setChart(m_chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    // 初始化曲线
    m_series = new QLineSeries();
    m_series->setName("实时SNR");
    m_series->setPen(QPen(Qt::green, 2));
    m_chart->addSeries(m_series);

    // 初始化坐标轴（X轴：传输时间，Y轴：固定SNR范围）
    m_axisX = new QDateTimeAxis();
    m_axisX->setFormat("mm:ss");  // 时间格式：分:秒（从00:00开始）
    m_axisX->setTitleText("时间(分：秒)");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);

    m_axisY = new QValueAxis();
    m_axisY->setTitleText("信噪比 (dB)");
    m_axisY->setRange(0, 50);     // 固定纵轴范围（根据实际SNR最大值调整）
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    // 关联曲线与坐标轴
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);

    // 显示图例
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignTop);

    m_startTime = QDateTime();
    m_isMonitoring = false;
}

SnrWidget::~SnrWidget()
{
    delete ui;
}

void SnrWidget::startSnrMonitoring()
{
    m_startTime = QDateTime::currentDateTime();  // 记录传输开始时间
    m_isMonitoring = true;
    m_series->clear();  // 清空历史数据
    // 设置X轴范围（原点到最大预计时长，如100分钟）
    m_axisX->setRange(m_startTime, m_startTime.addSecs(6000));
}

void SnrWidget::stopSnrMonitoring()
{
    m_isMonitoring = false;
}

void SnrWidget::updateSnr(double snr)
{
    if (!m_isMonitoring || !m_startTime.isValid()) {
        return;  // 未开始监控则不更新
    }

    // 当前时间相对于传输开始时间的毫秒数（作为X轴坐标）
    qint64 elapsedMs = m_startTime.msecsTo(QDateTime::currentDateTime());
    QDateTime currentXTime = m_startTime.addMSecs(elapsedMs);

    // 追加数据点
    m_series->append(currentXTime.toMSecsSinceEpoch(), snr);

    // 移除超出最大显示时长的旧数据（仅保留最近10分钟）
    QDateTime threshold = m_startTime.addSecs(600);
    while (!m_series->points().isEmpty()) {
        QDateTime pointTime = QDateTime::fromMSecsSinceEpoch(m_series->points().first().x());
        if (pointTime < threshold) {
            m_series->remove(0);
        } else {
            break;
        }
    }
}
