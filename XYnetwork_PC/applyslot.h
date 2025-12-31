#ifndef APPLYSLOT_H
#define APPLYSLOT_H

#include <QDialog>
#include <QString>

namespace Ui {
class ApplySlot;
}

class ApplySlot : public QDialog
{
    Q_OBJECT

public:
    explicit ApplySlot(int freeSlotNum, QWidget *parent = nullptr);
    ~ApplySlot();

    // 获取输入的传输时隙数量
    int getTransSlotNum() const;
    // 获取输入的测距时隙数量
    int getMeasureSlotNum() const;

signals:
    // 确认申请的信号，携带传输和测距时隙数量
    void applyConfirmed(int transNum, int measureNum);

private slots:
    // 确认按钮点击槽函数
    void on_pushButton_confirm_clicked();
    // 取消按钮点击槽函数
    void on_pushButton_cancel_clicked();

private:
    Ui::ApplySlot *ui;
    int m_freeSlotNum; // 存储动态空闲时隙数量
};

#endif // APPLYSLOT_H
