#ifndef SNRWIDGET_H
#define SNRWIDGET_H

#include <QWidget>
#include <QChart>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QChartView>
#include <QDateTime>

namespace Ui {
class SnrWidget;
}

class SnrWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SnrWidget(QWidget *parent = nullptr);
    ~SnrWidget();

    void startSnrMonitoring();   // 开始SNR监控（记录传输开始时间）
    void stopSnrMonitoring();    // 停止SNR监控
    void updateSnr(double snr);  // 更新实时SNR值

private:
    Ui::SnrWidget *ui;
    QChart *m_chart;
    QLineSeries *m_series;
    QDateTimeAxis *m_axisX;
    QValueAxis *m_axisY;
    QDateTime m_startTime;       // 传输开始时间（时间轴原点）
    bool m_isMonitoring = false; // 是否正在监控
};

#endif // SNRWIDGET_H
