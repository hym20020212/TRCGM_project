/********************************************************************************
** Form generated from reading UI file 'snrwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SNRWIDGET_H
#define UI_SNRWIDGET_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SnrWidget
{
public:
    QChartView *chartView;
    QLabel *label_xAxis;
    QLabel *label_yAxis;

    void setupUi(QWidget *SnrWidget)
    {
        if (SnrWidget->objectName().isEmpty())
            SnrWidget->setObjectName("SnrWidget");
        SnrWidget->resize(431, 331);
        chartView = new QChartView(SnrWidget);
        chartView->setObjectName("chartView");
        chartView->setGeometry(QRect(0, -20, 461, 321));
        label_xAxis = new QLabel(SnrWidget);
        label_xAxis->setObjectName("label_xAxis");
        label_xAxis->setGeometry(QRect(430, 290, 51, 16));
        QFont font;
        font.setPointSize(10);
        label_xAxis->setFont(font);
        label_yAxis = new QLabel(SnrWidget);
        label_yAxis->setObjectName("label_yAxis");
        label_yAxis->setGeometry(QRect(30, 10, 71, 21));
        label_yAxis->setFont(font);

        retranslateUi(SnrWidget);

        QMetaObject::connectSlotsByName(SnrWidget);
    } // setupUi

    void retranslateUi(QWidget *SnrWidget)
    {
        SnrWidget->setWindowTitle(QCoreApplication::translate("SnrWidget", "Form", nullptr));
        label_xAxis->setText(QString());
        label_yAxis->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SnrWidget: public Ui_SnrWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SNRWIDGET_H
