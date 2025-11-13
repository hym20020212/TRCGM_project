/********************************************************************************
** Form generated from reading UI file 'manualinputdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANUALINPUTDIALOG_H
#define UI_MANUALINPUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_ManualInputDialog
{
public:
    QTextEdit *textEdit_manualData;
    QPushButton *pushButton_sendManual;
    QLabel *label;

    void setupUi(QDialog *ManualInputDialog)
    {
        if (ManualInputDialog->objectName().isEmpty())
            ManualInputDialog->setObjectName("ManualInputDialog");
        ManualInputDialog->resize(399, 315);
        textEdit_manualData = new QTextEdit(ManualInputDialog);
        textEdit_manualData->setObjectName("textEdit_manualData");
        textEdit_manualData->setGeometry(QRect(40, 50, 311, 171));
        QFont font;
        font.setPointSize(10);
        textEdit_manualData->setFont(font);
        pushButton_sendManual = new QPushButton(ManualInputDialog);
        pushButton_sendManual->setObjectName("pushButton_sendManual");
        pushButton_sendManual->setGeometry(QRect(260, 240, 91, 21));
        pushButton_sendManual->setFont(font);
        label = new QLabel(ManualInputDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(40, 20, 131, 31));

        retranslateUi(ManualInputDialog);

        QMetaObject::connectSlotsByName(ManualInputDialog);
    } // setupUi

    void retranslateUi(QDialog *ManualInputDialog)
    {
        ManualInputDialog->setWindowTitle(QCoreApplication::translate("ManualInputDialog", "Dialog", nullptr));
        pushButton_sendManual->setText(QCoreApplication::translate("ManualInputDialog", "\345\217\221\351\200\201\346\225\260\346\215\256", nullptr));
        label->setText(QCoreApplication::translate("ManualInputDialog", "\350\257\267\350\276\223\345\205\245\346\225\260\346\215\256\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ManualInputDialog: public Ui_ManualInputDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANUALINPUTDIALOG_H
