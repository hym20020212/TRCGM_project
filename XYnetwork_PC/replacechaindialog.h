#ifndef REPLACECHAINDIALOG_H
#define REPLACECHAINDIALOG_H

#include <QDialog>

namespace Ui {
class ReplaceChainDialog;
}

class ReplaceChainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceChainDialog(QWidget *parent = nullptr);
    ~ReplaceChainDialog();
    int selectedChain() const;

private:
    Ui::ReplaceChainDialog *ui;
};

#endif // REPLACECHAINDIALOG_H
