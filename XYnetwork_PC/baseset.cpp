#include "baseset.h"
#include "ui_baseset.h"

BaseSet::BaseSet(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BaseSet)
{
    ui->setupUi(this);
    // 初始化节点名称→ID映射
    QList<QPair<QString, quint8>> nodeList = {
        {"MX", 0b0000000},    // 000-0000
        {"GZ1", 0b0010000},   // 001-0000
        {"GZ2", 0b0010001},   // 001-0001
        {"BY1", 0b0100000},   // 010-0000
        {"BY2", 0b0100001},   // 010-0001
        {"BY3", 0b0100010},   // 010-0010
        {"BY4", 0b0100011},   // 010-0011
        {"BY5", 0b0100100},   // 010-0100
        {"BY6", 0b0100101},   // 010-0101
        {"ZJ0", 0b0110000},   // 011-0000
        {"ZJ1", 0b0110001},   // 011-0001
        {"ZJ2", 0b0110010},   // 011-0010
        {"ZJ3", 0b0110011}    // 011-0011
    };

    // 清空原有选项
    ui->Base1_comboBox->clear();
    ui->Base2_comboBox->clear();
    ui->Base3_comboBox->clear();
    // 生成有序的选项列表，并同步更新m_nameToId
    m_nameToId.clear();
    QStringList nodes;
    for (const auto& pair : nodeList) {
        nodes.append(pair.first);
        m_nameToId[pair.first] = pair.second;
    }
    // 给三个ComboBox添加选项
    ui->Base1_comboBox->addItems(nodes);
    ui->Base2_comboBox->addItems(nodes);
    ui->Base3_comboBox->addItems(nodes);

    // 默认选择初始值
    ui->Base1_comboBox->setCurrentText("MX");
    ui->Base2_comboBox->setCurrentText("GZ1");
    ui->Base3_comboBox->setCurrentText("GZ2");

    // 关联确认按钮的点击信号（触发accept）
    connect(ui->pushButtonConfirm, &QPushButton::clicked, this, &BaseSet::accept);
}

BaseSet::~BaseSet()
{
    delete ui;
}

QMap<int, quint8> BaseSet::selectedBaseIds() const
{
    QMap<int, quint8> ids;
    ids[0] = m_nameToId.value(ui->Base1_comboBox->currentText());
    ids[1] = m_nameToId.value(ui->Base2_comboBox->currentText());
    ids[2] = m_nameToId.value(ui->Base3_comboBox->currentText());
    return ids;
}

QMap<int, QString> BaseSet::selectedBaseNames() const
{
    QMap<int, QString> names;
    names[0] = ui->Base1_comboBox->currentText();
    names[1] = ui->Base2_comboBox->currentText();
    names[2] = ui->Base3_comboBox->currentText();
    return names;
}
