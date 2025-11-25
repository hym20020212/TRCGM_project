#ifndef MXMODEDIALOG_H
#define MXMODEDIALOG_H

#include <QDialog>

namespace Ui {
class MXmodeDialog;
}

class MXmodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MXmodeDialog(QWidget *parent = nullptr);
    ~MXmodeDialog();
    QString selectedMode() const;

private:
    Ui::MXmodeDialog *ui;
};

#endif // MXMODEDIALOG_H
