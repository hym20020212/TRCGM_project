#ifndef ERRORSTATISTIC_H
#define ERRORSTATISTIC_H

#include <QObject>
#include <QVector>
#include <cstdint>

class QLabel;

class ErrorStatistic : public QObject
{
    Q_OBJECT
public:
    // 构造函数：传入UI显示控件的指针（后续用于更新显示）
    explicit ErrorStatistic(QLabel* bitErrorValueLabel,
                            QLabel* byteErrorValueLabel,
                            QLabel* frameErrorValueLabel,
                            QObject *parent = nullptr);
    // 设置发送的基准数据（发送文件的uint32_t数组）
    void setReferenceData(const QVector<uint32_t>& refData);

    // 传入新接收的数据（uint32_t数组），进行比对并更新统计
    void addReceivedData(const QVector<uint32_t>& recvData);

    // 重置统计（用于新文件发送时清空历史数据）
    void reset();
    double getRealTimeBitErrorRate() const;
    double getCumulativeBitErrorRate() const;

private:
    // 计算错误率并更新UI
    void updateStatistics();

private:
    // UI控件指针（用于显示结果）
    QLabel* m_bitErrorLabel;    // 误码率显示控件
    QLabel* m_byteErrorLabel;   // 误Byte率显示控件
    QLabel* m_frameErrorLabel;  // 误帧率显示控件

    // 基准数据（发送的文件数据）
    QVector<uint32_t> m_refData;

    // 接收数据的累计（按顺序存储）
    QVector<uint32_t> m_recvData;

    // 统计变量
    int m_totalBits = 0;      // 总比特数（累计）
    int m_errorBits = 0;      // 错误比特数（累计）
    uint64_t m_totalBytes = 0;     // 总字节数（累计）
    uint64_t m_errorBytes = 0;     // 错误字节数（累计）
    uint64_t m_totalFrames = 0;    // 总帧数（累计）
    uint64_t m_errorFrames = 0;    // 错误帧数（累计）
    int m_batchBits = 0;       // 最近一次批次的比特数
    int m_batchErrorBits = 0;
    int calculateErrorBits(const QVector<uint32_t> &recvData);
};

#endif // ERRORSTATISTIC_H
