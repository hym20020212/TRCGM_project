/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *pushButton_send;
    QLineEdit *lineEdit_filePath;
    QPushButton *pushButton_generate;
    QLabel *label_filePath;
    QLabel *label_sendlog;
    QPushButton *pushButton_connect;
    QPushButton *pushButton_openManualDialog;
    QLabel *label_bitErrorRate;
    QLabel *label_bitErrorValue;
    QLabel *label_byteErrorRate;
    QLabel *label_frameErrorRate;
    QLabel *label_byteErrorValue;
    QLabel *label_frameErrorValue;
    QWidget *widget_rate;
    QTextEdit *textEdit_sendlog;
    QLabel *label_connect;
    QLabel *label_generate;
    QLabel *label_recvlog;
    QTextEdit *textEdit_recvlog;
    QPushButton *pushButton_selectFile;
    QLabel *label_select;
    QLabel *label_num_sendbyte;
    QLabel *label_num_recvbyte;
    QLabel *label_sendbyte_value;
    QLabel *label_recvbyte_value;
    QWidget *widget_snr;
    QLabel *label_sendmanual;
    QPushButton *pushButton_playback;
    QLabel *label;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(1195, 684);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix1/image/AppIcon.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        Widget->setWindowIcon(icon);
        pushButton_send = new QPushButton(Widget);
        pushButton_send->setObjectName("pushButton_send");
        pushButton_send->setGeometry(QRect(30, 610, 231, 21));
        QFont font;
        font.setPointSize(10);
        pushButton_send->setFont(font);
        lineEdit_filePath = new QLineEdit(Widget);
        lineEdit_filePath->setObjectName("lineEdit_filePath");
        lineEdit_filePath->setGeometry(QRect(30, 580, 231, 21));
        pushButton_generate = new QPushButton(Widget);
        pushButton_generate->setObjectName("pushButton_generate");
        pushButton_generate->setGeometry(QRect(100, 0, 81, 61));
        pushButton_generate->setFont(font);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/new/prefix1/image/edit.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_generate->setIcon(icon1);
        pushButton_generate->setIconSize(QSize(24, 24));
        label_filePath = new QLabel(Widget);
        label_filePath->setObjectName("label_filePath");
        label_filePath->setGeometry(QRect(30, 560, 81, 21));
        label_filePath->setFont(font);
        label_sendlog = new QLabel(Widget);
        label_sendlog->setObjectName("label_sendlog");
        label_sendlog->setGeometry(QRect(30, 80, 61, 21));
        label_sendlog->setFont(font);
        pushButton_connect = new QPushButton(Widget);
        pushButton_connect->setObjectName("pushButton_connect");
        pushButton_connect->setGeometry(QRect(20, 0, 81, 61));
        pushButton_connect->setFont(font);
        pushButton_connect->setText(QString::fromUtf8(""));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/new/prefix1/image/linePainting.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_connect->setIcon(icon2);
        pushButton_connect->setIconSize(QSize(24, 24));
        pushButton_openManualDialog = new QPushButton(Widget);
        pushButton_openManualDialog->setObjectName("pushButton_openManualDialog");
        pushButton_openManualDialog->setGeometry(QRect(260, 0, 81, 61));
        pushButton_openManualDialog->setFont(font);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/new/prefix1/image/command.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_openManualDialog->setIcon(icon3);
        pushButton_openManualDialog->setIconSize(QSize(24, 24));
        label_bitErrorRate = new QLabel(Widget);
        label_bitErrorRate->setObjectName("label_bitErrorRate");
        label_bitErrorRate->setGeometry(QRect(270, 80, 51, 16));
        label_bitErrorRate->setFont(font);
        label_bitErrorValue = new QLabel(Widget);
        label_bitErrorValue->setObjectName("label_bitErrorValue");
        label_bitErrorValue->setGeometry(QRect(320, 80, 51, 16));
        label_bitErrorValue->setFont(font);
        label_byteErrorRate = new QLabel(Widget);
        label_byteErrorRate->setObjectName("label_byteErrorRate");
        label_byteErrorRate->setGeometry(QRect(380, 80, 61, 16));
        label_byteErrorRate->setFont(font);
        label_frameErrorRate = new QLabel(Widget);
        label_frameErrorRate->setObjectName("label_frameErrorRate");
        label_frameErrorRate->setGeometry(QRect(520, 80, 51, 16));
        label_frameErrorRate->setFont(font);
        label_byteErrorValue = new QLabel(Widget);
        label_byteErrorValue->setObjectName("label_byteErrorValue");
        label_byteErrorValue->setGeometry(QRect(440, 80, 61, 16));
        label_byteErrorValue->setFont(font);
        label_frameErrorValue = new QLabel(Widget);
        label_frameErrorValue->setObjectName("label_frameErrorValue");
        label_frameErrorValue->setGeometry(QRect(570, 80, 51, 16));
        label_frameErrorValue->setFont(font);
        widget_rate = new QWidget(Widget);
        widget_rate->setObjectName("widget_rate");
        widget_rate->setGeometry(QRect(270, 100, 441, 271));
        textEdit_sendlog = new QTextEdit(Widget);
        textEdit_sendlog->setObjectName("textEdit_sendlog");
        textEdit_sendlog->setGeometry(QRect(30, 100, 231, 211));
        label_connect = new QLabel(Widget);
        label_connect->setObjectName("label_connect");
        label_connect->setGeometry(QRect(30, 40, 61, 20));
        QFont font1;
        font1.setPointSize(9);
        label_connect->setFont(font1);
        label_generate = new QLabel(Widget);
        label_generate->setObjectName("label_generate");
        label_generate->setGeometry(QRect(120, 40, 61, 21));
        label_recvlog = new QLabel(Widget);
        label_recvlog->setObjectName("label_recvlog");
        label_recvlog->setGeometry(QRect(30, 310, 61, 21));
        label_recvlog->setFont(font);
        textEdit_recvlog = new QTextEdit(Widget);
        textEdit_recvlog->setObjectName("textEdit_recvlog");
        textEdit_recvlog->setGeometry(QRect(30, 330, 231, 221));
        pushButton_selectFile = new QPushButton(Widget);
        pushButton_selectFile->setObjectName("pushButton_selectFile");
        pushButton_selectFile->setGeometry(QRect(180, 0, 81, 61));
        pushButton_selectFile->setFont(font);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/new/prefix1/image/load1.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_selectFile->setIcon(icon4);
        pushButton_selectFile->setIconSize(QSize(24, 24));
        label_select = new QLabel(Widget);
        label_select->setObjectName("label_select");
        label_select->setGeometry(QRect(200, 40, 61, 21));
        label_num_sendbyte = new QLabel(Widget);
        label_num_sendbyte->setObjectName("label_num_sendbyte");
        label_num_sendbyte->setGeometry(QRect(130, 80, 81, 21));
        label_num_sendbyte->setFont(font);
        label_num_recvbyte = new QLabel(Widget);
        label_num_recvbyte->setObjectName("label_num_recvbyte");
        label_num_recvbyte->setGeometry(QRect(130, 310, 81, 21));
        label_num_recvbyte->setFont(font);
        label_sendbyte_value = new QLabel(Widget);
        label_sendbyte_value->setObjectName("label_sendbyte_value");
        label_sendbyte_value->setGeometry(QRect(210, 80, 51, 21));
        label_sendbyte_value->setFont(font);
        label_recvbyte_value = new QLabel(Widget);
        label_recvbyte_value->setObjectName("label_recvbyte_value");
        label_recvbyte_value->setGeometry(QRect(210, 310, 51, 21));
        label_recvbyte_value->setFont(font);
        widget_snr = new QWidget(Widget);
        widget_snr->setObjectName("widget_snr");
        widget_snr->setGeometry(QRect(270, 370, 441, 271));
        label_sendmanual = new QLabel(Widget);
        label_sendmanual->setObjectName("label_sendmanual");
        label_sendmanual->setGeometry(QRect(280, 40, 61, 21));
        pushButton_playback = new QPushButton(Widget);
        pushButton_playback->setObjectName("pushButton_playback");
        pushButton_playback->setGeometry(QRect(340, 0, 81, 61));
        pushButton_playback->setFont(font);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/new/prefix1/image/refresh.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pushButton_playback->setIcon(icon5);
        pushButton_playback->setIconSize(QSize(24, 24));
        label = new QLabel(Widget);
        label->setObjectName("label");
        label->setGeometry(QRect(370, 40, 61, 21));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        pushButton_send->setText(QCoreApplication::translate("Widget", "\345\217\221\351\200\201\346\226\207\344\273\266", nullptr));
        label_filePath->setText(QCoreApplication::translate("Widget", "\345\275\223\345\211\215\346\226\207\344\273\266\350\267\257\345\276\204\357\274\232", nullptr));
        label_sendlog->setText(QCoreApplication::translate("Widget", "\345\217\221\351\200\201\345\214\272\357\274\232", nullptr));
        pushButton_openManualDialog->setText(QString());
        label_bitErrorRate->setText(QCoreApplication::translate("Widget", "\350\257\257\347\240\201\347\216\207\357\274\232", nullptr));
        label_bitErrorValue->setText(QString());
        label_byteErrorRate->setText(QCoreApplication::translate("Widget", "\350\257\257Byte\347\216\207\357\274\232", nullptr));
        label_frameErrorRate->setText(QCoreApplication::translate("Widget", "\350\257\257\345\270\247\347\216\207\357\274\232", nullptr));
        label_byteErrorValue->setText(QString());
        label_frameErrorValue->setText(QString());
        label_connect->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245\346\234\215\345\212\241\345\231\250", nullptr));
        label_generate->setText(QCoreApplication::translate("Widget", "\347\224\237\346\210\220\346\226\207\344\273\266", nullptr));
        label_recvlog->setText(QCoreApplication::translate("Widget", "\346\216\245\346\224\266\345\214\272\357\274\232", nullptr));
        label_select->setText(QCoreApplication::translate("Widget", "\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        label_num_sendbyte->setText(QCoreApplication::translate("Widget", "\345\217\221\351\200\201byte\346\225\260\357\274\232", nullptr));
        label_num_recvbyte->setText(QCoreApplication::translate("Widget", "\346\216\245\346\224\266byte\346\225\260\357\274\232", nullptr));
        label_sendbyte_value->setText(QString());
        label_recvbyte_value->setText(QString());
        label_sendmanual->setText(QCoreApplication::translate("Widget", "\346\211\213\345\212\250\350\276\223\345\205\245", nullptr));
        pushButton_playback->setText(QString());
        label->setText(QCoreApplication::translate("Widget", "\345\233\236\346\224\276", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
