/********************************************************************************
** Form generated from reading UI file 'connectconfigdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONNECTCONFIGDIALOG_H
#define UI_CONNECTCONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ConnectConfigDialog
{
public:
    QLabel *label_port;
    QLineEdit *lineEdit_ip;
    QLabel *label_ip;
    QLineEdit *lineEdit_port;
    QPushButton *pushButton_confirm;
    QPushButton *pushButton_cancel;

    void setupUi(QDialog *ConnectConfigDialog)
    {
        if (ConnectConfigDialog->objectName().isEmpty())
            ConnectConfigDialog->setObjectName("ConnectConfigDialog");
        ConnectConfigDialog->resize(316, 143);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix1/image/linePainting.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        ConnectConfigDialog->setWindowIcon(icon);
        label_port = new QLabel(ConnectConfigDialog);
        label_port->setObjectName("label_port");
        label_port->setGeometry(QRect(60, 60, 61, 21));
        QFont font;
        font.setPointSize(10);
        label_port->setFont(font);
        lineEdit_ip = new QLineEdit(ConnectConfigDialog);
        lineEdit_ip->setObjectName("lineEdit_ip");
        lineEdit_ip->setGeometry(QRect(110, 30, 141, 21));
        label_ip = new QLabel(ConnectConfigDialog);
        label_ip->setObjectName("label_ip");
        label_ip->setGeometry(QRect(50, 30, 61, 21));
        label_ip->setFont(font);
        lineEdit_port = new QLineEdit(ConnectConfigDialog);
        lineEdit_port->setObjectName("lineEdit_port");
        lineEdit_port->setGeometry(QRect(110, 60, 141, 20));
        pushButton_confirm = new QPushButton(ConnectConfigDialog);
        pushButton_confirm->setObjectName("pushButton_confirm");
        pushButton_confirm->setGeometry(QRect(200, 90, 51, 21));
        pushButton_cancel = new QPushButton(ConnectConfigDialog);
        pushButton_cancel->setObjectName("pushButton_cancel");
        pushButton_cancel->setGeometry(QRect(140, 90, 51, 21));

        retranslateUi(ConnectConfigDialog);

        QMetaObject::connectSlotsByName(ConnectConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *ConnectConfigDialog)
    {
        ConnectConfigDialog->setWindowTitle(QCoreApplication::translate("ConnectConfigDialog", "Dialog", nullptr));
        label_port->setText(QCoreApplication::translate("ConnectConfigDialog", "\347\253\257\345\217\243\345\217\267\357\274\232", nullptr));
        label_ip->setText(QCoreApplication::translate("ConnectConfigDialog", "\346\234\215\345\212\241\345\231\250IP\357\274\232", nullptr));
        pushButton_confirm->setText(QCoreApplication::translate("ConnectConfigDialog", "\347\241\256\350\256\244", nullptr));
        pushButton_cancel->setText(QCoreApplication::translate("ConnectConfigDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConnectConfigDialog: public Ui_ConnectConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECTCONFIGDIALOG_H
