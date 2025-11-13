/********************************************************************************
** Form generated from reading UI file 'generatefiledialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GENERATEFILEDIALOG_H
#define UI_GENERATEFILEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_GenerateFileDialog
{
public:
    QLabel *label_filename;
    QLineEdit *lineEdit_filename;
    QLabel *label_fileSize;
    QComboBox *comboBox_ratio;
    QSpinBox *spinBox_fileSize;
    QLabel *label_ratio;
    QPushButton *pushButton_confirm;

    void setupUi(QDialog *GenerateFileDialog)
    {
        if (GenerateFileDialog->objectName().isEmpty())
            GenerateFileDialog->setObjectName("GenerateFileDialog");
        GenerateFileDialog->resize(400, 300);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix1/image/edit.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        GenerateFileDialog->setWindowIcon(icon);
        label_filename = new QLabel(GenerateFileDialog);
        label_filename->setObjectName("label_filename");
        label_filename->setGeometry(QRect(80, 70, 91, 31));
        QFont font;
        font.setPointSize(12);
        label_filename->setFont(font);
        lineEdit_filename = new QLineEdit(GenerateFileDialog);
        lineEdit_filename->setObjectName("lineEdit_filename");
        lineEdit_filename->setGeometry(QRect(180, 70, 161, 31));
        lineEdit_filename->setFont(font);
        label_fileSize = new QLabel(GenerateFileDialog);
        label_fileSize->setObjectName("label_fileSize");
        label_fileSize->setGeometry(QRect(60, 120, 111, 31));
        label_fileSize->setFont(font);
        comboBox_ratio = new QComboBox(GenerateFileDialog);
        comboBox_ratio->setObjectName("comboBox_ratio");
        comboBox_ratio->setGeometry(QRect(180, 170, 81, 31));
        spinBox_fileSize = new QSpinBox(GenerateFileDialog);
        spinBox_fileSize->setObjectName("spinBox_fileSize");
        spinBox_fileSize->setGeometry(QRect(180, 120, 81, 31));
        spinBox_fileSize->setFont(font);
        label_ratio = new QLabel(GenerateFileDialog);
        label_ratio->setObjectName("label_ratio");
        label_ratio->setGeometry(QRect(120, 170, 61, 31));
        label_ratio->setFont(font);
        pushButton_confirm = new QPushButton(GenerateFileDialog);
        pushButton_confirm->setObjectName("pushButton_confirm");
        pushButton_confirm->setGeometry(QRect(280, 210, 61, 21));
        pushButton_confirm->setFont(font);

        retranslateUi(GenerateFileDialog);

        QMetaObject::connectSlotsByName(GenerateFileDialog);
    } // setupUi

    void retranslateUi(QDialog *GenerateFileDialog)
    {
        GenerateFileDialog->setWindowTitle(QCoreApplication::translate("GenerateFileDialog", "Dialog", nullptr));
        label_filename->setText(QCoreApplication::translate("GenerateFileDialog", "\350\256\276\347\275\256\346\226\207\344\273\266\345\220\215\357\274\232", nullptr));
        label_fileSize->setText(QCoreApplication::translate("GenerateFileDialog", "\346\226\207\344\273\266\345\244\247\345\260\217\357\274\210KB\357\274\211\357\274\232", nullptr));
        label_ratio->setText(QCoreApplication::translate("GenerateFileDialog", "0\345\215\240\346\257\224\357\274\232", nullptr));
        pushButton_confirm->setText(QCoreApplication::translate("GenerateFileDialog", "\347\241\256\345\256\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GenerateFileDialog: public Ui_GenerateFileDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENERATEFILEDIALOG_H
