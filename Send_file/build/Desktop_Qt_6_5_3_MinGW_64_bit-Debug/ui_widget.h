/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *pushButton_selectFile;
    QPushButton *pushButton_send;
    QLineEdit *lineEdit_filePath;
    QTextEdit *textEdit_log;
    QLabel *label_filename;
    QLineEdit *lineEdit_filename;
    QLabel *label_fileSize;
    QSpinBox *spinBox_fileSize;
    QLabel *label_ratio;
    QComboBox *comboBox_ratio;
    QPushButton *pushButton_generate;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(570, 461);
        pushButton_selectFile = new QPushButton(Widget);
        pushButton_selectFile->setObjectName("pushButton_selectFile");
        pushButton_selectFile->setGeometry(QRect(70, 220, 101, 31));
        QFont font;
        font.setPointSize(12);
        pushButton_selectFile->setFont(font);
        pushButton_send = new QPushButton(Widget);
        pushButton_send->setObjectName("pushButton_send");
        pushButton_send->setGeometry(QRect(70, 280, 101, 31));
        pushButton_send->setFont(font);
        lineEdit_filePath = new QLineEdit(Widget);
        lineEdit_filePath->setObjectName("lineEdit_filePath");
        lineEdit_filePath->setGeometry(QRect(190, 220, 161, 31));
        textEdit_log = new QTextEdit(Widget);
        textEdit_log->setObjectName("textEdit_log");
        textEdit_log->setGeometry(QRect(190, 270, 351, 171));
        label_filename = new QLabel(Widget);
        label_filename->setObjectName("label_filename");
        label_filename->setGeometry(QRect(120, 30, 61, 31));
        label_filename->setFont(font);
        lineEdit_filename = new QLineEdit(Widget);
        lineEdit_filename->setObjectName("lineEdit_filename");
        lineEdit_filename->setGeometry(QRect(190, 30, 161, 31));
        lineEdit_filename->setFont(font);
        label_fileSize = new QLabel(Widget);
        label_fileSize->setObjectName("label_fileSize");
        label_fileSize->setGeometry(QRect(70, 80, 111, 31));
        label_fileSize->setFont(font);
        spinBox_fileSize = new QSpinBox(Widget);
        spinBox_fileSize->setObjectName("spinBox_fileSize");
        spinBox_fileSize->setGeometry(QRect(190, 80, 81, 31));
        spinBox_fileSize->setFont(font);
        label_ratio = new QLabel(Widget);
        label_ratio->setObjectName("label_ratio");
        label_ratio->setGeometry(QRect(130, 130, 61, 31));
        label_ratio->setFont(font);
        comboBox_ratio = new QComboBox(Widget);
        comboBox_ratio->setObjectName("comboBox_ratio");
        comboBox_ratio->setGeometry(QRect(190, 130, 131, 31));
        pushButton_generate = new QPushButton(Widget);
        pushButton_generate->setObjectName("pushButton_generate");
        pushButton_generate->setGeometry(QRect(70, 170, 281, 31));
        pushButton_generate->setFont(font);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        pushButton_selectFile->setText(QCoreApplication::translate("Widget", "\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        pushButton_send->setText(QCoreApplication::translate("Widget", "\345\217\221\351\200\201\346\226\207\344\273\266", nullptr));
        label_filename->setText(QCoreApplication::translate("Widget", "\346\226\207\344\273\266\345\220\215\357\274\232", nullptr));
        label_fileSize->setText(QCoreApplication::translate("Widget", "\346\226\207\344\273\266\345\244\247\345\260\217\357\274\210KB\357\274\211\357\274\232", nullptr));
        label_ratio->setText(QCoreApplication::translate("Widget", "0\345\215\240\346\257\224\357\274\232", nullptr));
        pushButton_generate->setText(QCoreApplication::translate("Widget", "\347\224\237\346\210\220\346\226\207\344\273\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
