#include "generatefiledialog.h"
#include "ui_generatefiledialog.h"
#include <QDir>
#include <QRandomGenerator>
#include <QMessageBox>

GenerateFileDialog::GenerateFileDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GenerateFileDialog)
{
    ui->setupUi(this);
    setWindowTitle("生成二进制文件");
    // 初始化比例选择框（0.1~0.9，9档）
    ui->comboBox_ratio->addItems({"0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9"});
    ui->comboBox_ratio->setCurrentIndex(0);

    // 初始化文件大小（1~1000KB）
    ui->spinBox_fileSize->setRange(1, 1000);
    ui->spinBox_fileSize->setValue(100);

    // 初始化默认文件名
    ui->lineEdit_filename->setText("auto_gen_data.bin");
}

GenerateFileDialog::~GenerateFileDialog()
{
    delete ui;
}

// 生成文件的核心逻辑
bool GenerateFileDialog::generateBinaryFile(const QString &filePath, qint64 totalBytes, double zeroRatio)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "错误", "无法创建文件：" + file.errorString());
        return false;
    }

    // 二进制文件总大小 = totalBytes（用户设置的大小）
    // 每次写入1个uint32_t（4字节），计算需要写入的次数
    qint64 totalWriteBytes = 0;
    const int bytesPerUnit = sizeof(uint32_t);  // 4字节/单位（32比特）
    QRandomGenerator *rg = QRandomGenerator::global();

    while (totalWriteBytes < totalBytes) {
        uint32_t data = 0;  // 32位二进制数据

        // 生成32位随机0/1比特（按比例控制0和1）
        for (int i = 0; i < 32; ++i) {
            double randVal = rg->generateDouble();  // 0~1随机数
            if (randVal >= zeroRatio) {  // 1的比例 = 1 - zeroRatio
                data |= (1 << (31 - i));  // 第i位设为1（高位在前）
            }
            // 0的情况无需处理（默认bit为0）
        }

        // 写入二进制数据（直接写uint32_t的内存）
        qint64 written = file.write((char*)&data, bytesPerUnit);
        if (written <= 0) {
            file.close();
            return false;
        }

        totalWriteBytes += written;

        // 避免超出用户设置的总大小（最后一次可能不足4字节，这里简化处理为截断）
        if (totalWriteBytes + bytesPerUnit > totalBytes) {
            break;
        }
    }

    file.close();
    return true;
}


// “确定”按钮：生成文件并关闭窗口
void GenerateFileDialog::on_pushButton_confirm_clicked()
{
    QString fileName = ui->lineEdit_filename->text().trimmed();
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "错误", "文件名不能为空！");
        return;
    }

    int fileSizeKB = ui->spinBox_fileSize->value();
    qint64 totalBytes = fileSizeKB * 1024;  // 二进制文件总字节数
    double zeroRatio = ui->comboBox_ratio->currentText().toDouble();

    // 生成文件路径（当前目录）
    m_generatedFilePath = QDir::currentPath() + "/" + fileName;
    QFileInfo fileInfo(m_generatedFilePath);
    if (fileInfo.exists()) {
        // 询问是否覆盖已存在的文件
        if (QMessageBox::question(this, "提示", "文件已存在，是否覆盖？") != QMessageBox::Yes) {
            return;
        }
    }

    // 生成文件
    bool success = generateBinaryFile(m_generatedFilePath, fileSizeKB * 1024, zeroRatio);
    if (success) {
        QMessageBox::information(this, "成功", "文件生成成功！\n路径：" + m_generatedFilePath);
        accept();  // 关闭对话框（返回QDialog::Accepted）
    } else {
        QMessageBox::critical(this, "失败", "文件生成失败！");
    }
}

// 供主窗口获取生成的文件路径
QString GenerateFileDialog::getGeneratedFilePath() const
{
    return m_generatedFilePath;
}
