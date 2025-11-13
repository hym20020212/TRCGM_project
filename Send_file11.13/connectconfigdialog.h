#ifndef CONNECTCONFIGDIALOG_H
#define CONNECTCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectConfigDialog;
}

class ConnectConfigDialog : public QDialog
{
    Q_OBJECT


public:
    explicit ConnectConfigDialog(QWidget *parent = nullptr);
    ~ConnectConfigDialog();
    QString getInputIp() const;
    quint16 getInputPort() const;


private:
    Ui::ConnectConfigDialog *ui;
};

#endif // CONNECTCONFIGDIALOG_H
