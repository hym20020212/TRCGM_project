#pragma once
#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include "nodeitem.h"

namespace Ui {
class ReplaceChainDialog;
}

class ReplaceChainDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ReplaceChainDialog(QWidget *parent = nullptr);
    ~ReplaceChainDialog();

    // 同步主拓扑
    void updateNodeState(const QString& name, int slot);
    void updateNodeDeviceId(const QString& name, const QString& id);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::ReplaceChainDialog *ui;
    QGraphicsScene *m_scene;

    QMap<QString, NodeItem*> m_nodes;

    NodeItem* m_firstSelected = nullptr;
    QList<QGraphicsLineItem*> m_arrows;

    void drawNodes();
    void addArrow(NodeItem* from, NodeItem* to);

private slots:
    // Reset和Confirm按钮的槽函数
    void on_pushButtonReset_clicked();
    void on_pushButtonConfirm_clicked();

signals:
    void replaceChainConfirmed(const QString& tipText);
};
