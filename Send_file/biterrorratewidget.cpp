#include "biterrorratewidget.h"
#include "ui_biterrorratewidget.h"
#include <QPainter>

BitErrorRateWidget::BitErrorRateWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BitErrorRateWidget)
{
    ui->setupUi(this);

    // 初始化图表
    m_chart = new QChart();
    ui->chartView->setChart(m_chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    // 初始化两条曲线
    m_realTimeSeries = new QLineSeries();
    m_realTimeSeries->setName("实时误码率");
    m_realTimeSeries->setPen(QPen(Qt::red, 2));

    m_cumulativeSeries = new QLineSeries();
    m_cumulativeSeries->setName("累计误码率");
    m_cumulativeSeries->setPen(QPen(Qt::blue, 2));

    m_chart->addSeries(m_realTimeSeries);
    m_chart->addSeries(m_cumulativeSeries);

    // 初始化坐标轴（X轴：传输时间，Y轴：固定误码率范围）
    m_axisX = new QDateTimeAxis();
    m_axisX->setFormat("mm:ss");  // 时间格式：分:秒（从00:00开始）
    m_axisX->setTitleText("时间(分：秒)");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);

    m_axisY = new QValueAxis();
    m_axisY->setTitleText("误码率");
    m_axisY->setRange(0, 1);      // 固定纵轴范围（误码率在0~1之间）
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    // 关联曲线与坐标轴
    m_realTimeSeries->attachAxis(m_axisX);
    m_realTimeSeries->attachAxis(m_axisY);
    m_cumulativeSeries->attachAxis(m_axisX);
    m_cumulativeSeries->attachAxis(m_axisY);

    // 显示图例
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignTop);

    m_startTime = QDateTime();
    m_isMonitoring = false;
}

BitErrorRateWidget::~BitErrorRateWidget()
{
    delete ui;
}

void BitErrorRateWidget::startMonitoring()
{
    m_startTime = QDateTime::currentDateTime();  // 记录传输开始时间
    m_isMonitoring = true;
    // 清空历史数据
    m_realTimeSeries->clear();
    m_cumulativeSeries->clear();
    // 设置X轴范围（原点到最大预计时长，如100分钟）
    m_axisX->setRange(m_startTime, m_startTime.addSecs(6000));
}

void BitErrorRateWidget::stopMonitoring()
{
    m_isMonitoring = false;
}

void BitErrorRateWidget::updateErrorRates(double realTimeBer, double cumulativeBer)
{
    if (!m_isMonitoring || !m_startTime.isValid()) {
        return;  // 未开始监控则不更新
    }

    // 当前时间相对于传输开始时间的毫秒数（作为X轴坐标）
    qint64 elapsedMs = m_startTime.msecsTo(QDateTime::currentDateTime());
    QDateTime currentXTime = m_startTime.addMSecs(elapsedMs);

    // 追加数据点
    m_realTimeSeries->append(currentXTime.toMSecsSinceEpoch(), realTimeBer);
    m_cumulativeSeries->append(currentXTime.toMSecsSinceEpoch(), cumulativeBer);

    // 移除超出最大显示时长的旧数据（仅保留最近100分钟）
    QDateTime threshold = m_startTime.addSecs(6000);
    auto removeOldData = [this, threshold](QLineSeries *series) {
        while (!series->points().isEmpty()) {
            QDateTime pointTime = QDateTime::fromMSecsSinceEpoch(series->points().first().x());
            if (pointTime < threshold) {
                series->remove(0);
            } else {
                break;
            }
        }
    };
    removeOldData(m_realTimeSeries);
    removeOldData(m_cumulativeSeries);
}
