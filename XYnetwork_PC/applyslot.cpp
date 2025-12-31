#include "applyslot.h"
#include "ui_applyslot.h"
#include <QMessageBox>
#include <QIntValidator>

ApplySlot::ApplySlot(int freeSlotNum, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ApplySlot)
    , m_freeSlotNum(freeSlotNum)
{
    ui->setupUi(this);
    this->adjustSize(); // 让窗口自动调整为布局的最小尺寸
    this->setMinimumSize(this->size());

    // 设置标签显示动态空闲时隙数量
    ui->label_freeslot->setText(QString("%1").arg(m_freeSlotNum));

    // 限制输入框只能输入正整数
    ui->lineEditnum_trans->setValidator(new QIntValidator(0, 999, this));
    ui->lineEditnum_measure->setValidator(new QIntValidator(0, 999, this));

    // 取消按钮绑定（也可通过ui设计师的信号槽自动绑定）
    connect(ui->pushButton_cancel, &QPushButton::clicked, this, &ApplySlot::reject);
}

ApplySlot::~ApplySlot()
{
    delete ui;
}

int ApplySlot::getTransSlotNum() const
{
    // 转换输入的传输时隙数量，默认0
    return ui->lineEditnum_trans->text().toInt();
}

int ApplySlot::getMeasureSlotNum() const
{
    // 转换输入的测距时隙数量，默认0
    return ui->lineEditnum_measure->text().toInt();
}

void ApplySlot::on_pushButton_confirm_clicked()
{
    int transNum = getTransSlotNum();
    int measureNum = getMeasureSlotNum();

    // 校验输入是否为非负数（虽有输入校验，做双重保障）
    if (transNum < 0 || measureNum < 0)
    {
        QMessageBox::warning(this, "输入错误", "请输入有效的非负整数！");
        return;
    }

    // 判断申请总数是否超过空闲时隙
    if (transNum + measureNum > m_freeSlotNum)
    {
        QMessageBox::warning(this, "申请失败",
                             QString("申请的时隙总数（%1）超过可用动态时隙数量（%2）！")
                                 .arg(transNum + measureNum).arg(m_freeSlotNum));
        return;
    }

    // 发送确认信号，通知主窗口
    emit applyConfirmed(transNum, measureNum);
    // 关闭对话框
    accept();
}

void ApplySlot::on_pushButton_cancel_clicked()
{
    // 取消申请，直接关闭对话框
    reject();
}
