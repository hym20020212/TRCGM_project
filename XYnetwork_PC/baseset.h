#ifndef BASESET_H
#define BASESET_H

#include <QDialog>

namespace Ui {
class BaseSet;
}

class BaseSet : public QDialog
{
    Q_OBJECT

public:
    explicit BaseSet(QWidget *parent = nullptr);
    ~BaseSet();
    // 获取选择的基座信息：key=基座序号(1/2/3)，value=节点ID(高7位)
    QMap<int, quint8> selectedBaseIds() const;
    // 获取选择的基座名称：key=基座序号(1/2/3)，value=节点名称
    QMap<int, QString> selectedBaseNames() const;

private:
    Ui::BaseSet *ui;
    QMap<QString, quint8> m_nameToId;
};

#endif // BASESET_H
