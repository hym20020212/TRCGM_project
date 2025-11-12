#ifndef BITERRORRATEWIDGET_H
#define BITERRORRATEWIDGET_H

#include <QWidget>
#include <QChart>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QChartView>
#include <QDateTime>

namespace Ui {
class BitErrorRateWidget;
}

class BitErrorRateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BitErrorRateWidget(QWidget *parent = nullptr);
    ~BitErrorRateWidget();

    void startMonitoring();   // 开始误码率监控（记录传输开始时间）
    void stopMonitoring();    // 停止误码率监控
    void updateErrorRates(double realTimeBer, double cumulativeBer);  // 更新误码率

private:
    Ui::BitErrorRateWidget *ui;
    QChart *m_chart;
    QLineSeries *m_realTimeSeries;  // 实时误码率曲线
    QLineSeries *m_cumulativeSeries; // 累计误码率曲线
    QDateTimeAxis *m_axisX;
    QValueAxis *m_axisY;
    QDateTime m_startTime;       // 传输开始时间（时间轴原点）
    bool m_isMonitoring = false; // 是否正在监控
};

#endif // BITERRORRATEWIDGET_H
