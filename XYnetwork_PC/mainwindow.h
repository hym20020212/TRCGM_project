#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QTcpSocket>
#include <QMainWindow>
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
    bool connectToNode(const QString &ip, int port);
};
#endif // MAINWINDOW_H

