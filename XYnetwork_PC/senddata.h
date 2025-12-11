#ifndef SENDDATA_H
#define SENDDATA_H

#include <QDialog>

namespace Ui {
class SendData;
}

class SendData : public QDialog
{
    Q_OBJECT

public:
    explicit SendData(QWidget *parent = nullptr);
    ~SendData();

private slots:
    void on_pushButton_senddata_clicked();
    void on_pushButton_clear_clicked();

signals:
    void sendDataRequest(const QString &targetNode, const QString &payload);

private:
    Ui::SendData *ui;
};




#endif // SENDDATA_H
