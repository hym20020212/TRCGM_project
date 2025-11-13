/********************************************************************************
** Form generated from reading UI file 'biterrorratewidget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BITERRORRATEWIDGET_H
#define UI_BITERRORRATEWIDGET_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BitErrorRateWidget
{
public:
    QLabel *label_yAxis;
    QChartView *chartView;
    QLabel *label_xAxis;

    void setupUi(QWidget *BitErrorRateWidget)
    {
        if (BitErrorRateWidget->objectName().isEmpty())
            BitErrorRateWidget->setObjectName("BitErrorRateWidget");
        BitErrorRateWidget->resize(431, 331);
        label_yAxis = new QLabel(BitErrorRateWidget);
        label_yAxis->setObjectName("label_yAxis");
        label_yAxis->setGeometry(QRect(-150, -20, 71, 21));
        QFont font;
        font.setPointSize(10);
        label_yAxis->setFont(font);
        chartView = new QChartView(BitErrorRateWidget);
        chartView->setObjectName("chartView");
        chartView->setGeometry(QRect(0, -20, 461, 321));
        label_xAxis = new QLabel(BitErrorRateWidget);
        label_xAxis->setObjectName("label_xAxis");
        label_xAxis->setGeometry(QRect(380, 280, 51, 16));
        label_xAxis->setFont(font);

        retranslateUi(BitErrorRateWidget);

        QMetaObject::connectSlotsByName(BitErrorRateWidget);
    } // setupUi

    void retranslateUi(QWidget *BitErrorRateWidget)
    {
        BitErrorRateWidget->setWindowTitle(QCoreApplication::translate("BitErrorRateWidget", "Form", nullptr));
        label_yAxis->setText(QString());
        label_xAxis->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class BitErrorRateWidget: public Ui_BitErrorRateWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BITERRORRATEWIDGET_H
