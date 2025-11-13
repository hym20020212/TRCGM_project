/********************************************************************************
** Form generated from reading UI file 'transferrate.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRANSFERRATE_H
#define UI_TRANSFERRATE_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Transferrate
{
public:
    QChartView *chartView;
    QLabel *label_xAxis;
    QLabel *label_yAxis;

    void setupUi(QWidget *Transferrate)
    {
        if (Transferrate->objectName().isEmpty())
            Transferrate->setObjectName("Transferrate");
        Transferrate->resize(431, 331);
        chartView = new QChartView(Transferrate);
        chartView->setObjectName("chartView");
        chartView->setGeometry(QRect(0, -20, 461, 321));
        label_xAxis = new QLabel(Transferrate);
        label_xAxis->setObjectName("label_xAxis");
        label_xAxis->setGeometry(QRect(420, 280, 51, 16));
        QFont font;
        font.setPointSize(10);
        label_xAxis->setFont(font);
        label_yAxis = new QLabel(Transferrate);
        label_yAxis->setObjectName("label_yAxis");
        label_yAxis->setGeometry(QRect(80, 20, 71, 21));
        label_yAxis->setFont(font);

        retranslateUi(Transferrate);

        QMetaObject::connectSlotsByName(Transferrate);
    } // setupUi

    void retranslateUi(QWidget *Transferrate)
    {
        Transferrate->setWindowTitle(QCoreApplication::translate("Transferrate", "Form", nullptr));
        label_xAxis->setText(QString());
        label_yAxis->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Transferrate: public Ui_Transferrate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRANSFERRATE_H
