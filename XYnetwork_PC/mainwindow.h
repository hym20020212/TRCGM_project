#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QTcpSocket>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QVector>
#include "slotitem.h"
#include "topologyview.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void onActionConnectTriggered();
    void onActionMxModeTriggered();
    void onActionReplaceTriggered();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    TopologyView *topology;
    QGraphicsScene *slotScene1 = nullptr;      // 为 slotcase1_View 准备的 scene
    QGraphicsScene *slotScene2 = nullptr;
    QVector<SlotItem*> slotItems;              // 存放 80 个 SlotItem 指针

    void initSlotScene();
    bool connectToNode(const QString &ip, int port);
    void updateSlotFromProtocol(int idx, int typeCode,
                                const QString &fromName,
                                const QString &toName = QString());

    int countFreeSlots();

protected:
    void resizeEvent(QResizeEvent *event) override;
};
#endif // MAINWINDOW_H

