#ifndef GENERATEFILEDIALOG_H
#define GENERATEFILEDIALOG_H

#include <QDialog>
#include <QFile>

namespace Ui {
class GenerateFileDialog;
}

class GenerateFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenerateFileDialog(QWidget *parent = nullptr);
    ~GenerateFileDialog();

    // 获取生成的文件路径（供主窗口使用）
    QString getGeneratedFilePath() const;

private slots:
    // “确定”按钮的槽函数（触发文件生成）
    void on_pushButton_confirm_clicked();

private:
    Ui::GenerateFileDialog *ui;
    QString m_generatedFilePath;  // 生成的文件路径
    // 生成二进制文件的核心函数（复用之前的逻辑）
    bool generateBinaryFile(const QString &filePath, qint64 totalBytes, double zeroRatio);
    qint64 calculateLineCount(qint64 totalBytes);

};

#endif // GENERATEFILEDIALOG_H
