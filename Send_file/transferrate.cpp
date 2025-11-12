#include "transferrate.h"
#include "ui_transferrate.h"
#include <QDateTime>
#include <QPainter>

Transferrate::Transferrate(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Transferrate)
    , m_startTime(QDateTime())  // 初始化传输开始时间（无效状态）
    , m_isTransferring(false)
{
    ui->setupUi(this);

    m_chart = new QChart();
    ui->chartView->setChart(m_chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    m_series1s = new QLineSeries();
    m_series1s->setName("每1秒");
    m_series1s->setPen(QPen(Qt::red, 2));

    m_series5s = new QLineSeries();
    m_series5s->setName("每5秒");
    m_series5s->setPen(QPen(Qt::yellow, 2));

    m_series10s = new QLineSeries();
    m_series10s->setName("每10秒");
    m_series10s->setPen(QPen(Qt::blue, 2));

    m_chart->addSeries(m_series1s);
    m_chart->addSeries(m_series5s);
    m_chart->addSeries(m_series10s);

    // 初始化坐标轴
    m_axisX = new QDateTimeAxis();
    m_axisX->setFormat("mm:ss");
    m_axisX->setTitleText("时间(分：秒)");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);

    m_axisY = new QValueAxis();
    m_axisY->setTitleText("传输速率 (B/s)");
    m_axisY->setRange(0, 100000);  // 初始速率范围
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    // 关联曲线与坐标轴
    m_series1s->attachAxis(m_axisX);
    m_series1s->attachAxis(m_axisY);
    m_series5s->attachAxis(m_axisX);
    m_series5s->attachAxis(m_axisY);
    m_series10s->attachAxis(m_axisX);
    m_series10s->attachAxis(m_axisY);

    // 显示图例
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignTop);  // 图例在顶部

}

Transferrate::~Transferrate()
{
    delete ui;
}

void Transferrate::startTransfer()
{
    m_startTime = QDateTime::currentDateTime();  // 传输开始时间作为原点
    m_isTransferring = true;
    // 清空历史数据
    m_series1s->clear();
    m_series5s->clear();
    m_series10s->clear();
    // 重置X轴范围（原点到最大预计时长，如10分钟）
    m_axisX->setRange(
        m_startTime,
        m_startTime.addSecs(600)  // 最大显示10分钟（600秒）
        );
}

void Transferrate::stopTransfer()
{
    m_isTransferring = false;
}

void Transferrate::updateRates(double rate1s, double rate5s, double rate10s)
{
    if (!m_isTransferring || !m_startTime.isValid()) {
        return;  // 未开始传输则不更新
    }

    // 当前时间相对于传输开始时间的毫秒数（作为X轴坐标）
    qint64 elapsedMs = m_startTime.msecsTo(QDateTime::currentDateTime());
    // 转换为QDateTime格式（基于开始时间偏移，确保X轴正确显示）
    QDateTime currentXTime = m_startTime.addMSecs(elapsedMs);

    // 追加数据点（X轴：相对时间，Y轴：真实速率）
    m_series1s->append(currentXTime.toMSecsSinceEpoch(), rate1s);
    m_series5s->append(currentXTime.toMSecsSinceEpoch(), rate5s);
    m_series10s->append(currentXTime.toMSecsSinceEpoch(), rate10s);

    // 移除超出最大显示时长的旧数据（仅保留最近100分钟）
    auto removeOldData = [this](QLineSeries *series) {
        QDateTime threshold = m_startTime.addSecs(6000);  // 100分钟阈值
        while (!series->points().isEmpty()) {
            QDateTime pointTime = QDateTime::fromMSecsSinceEpoch(series->points().first().x());
            if (pointTime < threshold) {
                series->remove(0);
            } else {
                break;
            }
        }
    };
    removeOldData(m_series1s);
    removeOldData(m_series5s);
    removeOldData(m_series10s);
}
