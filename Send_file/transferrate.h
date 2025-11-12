#ifndef TRANSFERRATE_H
#define TRANSFERRATE_H

#include <QWidget>
#include "ui_transferrate.h"
#include <QtCharts>
QT_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_END_NAMESPACE

class Transferrate : public QWidget
{
    Q_OBJECT

public:
    explicit Transferrate(QWidget *parent = nullptr);
    ~Transferrate();
    void updateRates(double rate1s, double rate5s, double rate10s);
    void startTransfer();
    void stopTransfer();


private:
    Ui::Transferrate *ui;
    QChart *m_chart;            // 图表对象
    QLineSeries *m_series1s;    // 1秒平均曲线
    QLineSeries *m_series5s;    // 5秒平均曲线
    QLineSeries *m_series10s;   // 10秒平均曲线
    QDateTimeAxis *m_axisX;     // 时间轴（横轴）
    QValueAxis *m_axisY;        // 速率轴（纵轴）
    QTimer *m_timer;
    QDateTime m_startTime;       // 传输开始时间（作为时间轴原点）
    bool m_isTransferring = false; // 是否正在传输
    double m_realRate1s = 0;     // 真实1秒平均速率
    double m_realRate5s = 0;     // 真实5秒平均速率
    double m_realRate10s = 0;    // 真实10秒平均速率
};

#endif // TRANSFERRATE_H
