#include "errorstatistic.h"
#include "QLabel"
#include <QString>

ErrorStatistic::ErrorStatistic(QLabel* bitErrorValueLabel,
                               QLabel* byteErrorValueLabel,
                               QLabel* frameErrorValueLabel,
                               QObject *parent)
    : QObject{parent}
    , m_bitErrorLabel(bitErrorValueLabel)
    , m_byteErrorLabel(byteErrorValueLabel)
    , m_frameErrorLabel(frameErrorValueLabel)
{
    // 初始化显示为0%
    reset();
}

// 设置发送的基准数据（发送文件的uint32_t数组）
void ErrorStatistic::setReferenceData(const QVector<uint32_t>& refData)
{
    m_refData = refData;
    reset();  // 新基准数据，重置统计
}

// 传入新接收的数据，比对并更新统计
void ErrorStatistic::addReceivedData(const QVector<uint32_t>& recvData)
{
    // 若基准数据为空，无法比对（直接返回）
    if (m_refData.isEmpty()) return;

    // 遍历新接收的每帧数据（按顺序比对）
    for (const uint32_t& recvFrame : recvData) {
        // 检查是否超出基准数据范围（避免越界）
        if (m_recvData.size() >= m_refData.size()) break;

        // 获取对应位置的基准帧
        uint32_t refFrame = m_refData[m_recvData.size()];

        // 1. 帧级比对（是否完全一致）
        bool frameError = (recvFrame != refFrame);
        if (frameError) {
            m_errorFrames++;  // 错误帧数+1
        }
        m_totalFrames++;     // 总帧数+1

        // 2. 字节级比对（4字节，只要有1位错则计为4字节错误）
        if (frameError) {
            m_errorBytes += 4;  // 错误字节数+4（1帧=4字节）
        }
        m_totalBytes += 4;     // 总字节数+4

        // 3. 比特级比对（逐位比较32位）
        for (int bit = 0; bit < 32; bit++) {
            bool refBit = (refFrame >> (31 - bit)) & 1;  // 高位在前
            bool recvBit = (recvFrame >> (31 - bit)) & 1;
            if (refBit != recvBit) {
                m_errorBits++;  // 错误比特数+1
            }
            m_totalBits++;       // 总比特数+1
        }

        // 保存接收的帧（用于后续累计）
        m_recvData.append(recvFrame);
    }

    // 实时更新UI显示
    updateStatistics();
}

// 计算错误率并更新UI
void ErrorStatistic::updateStatistics()
{
    // 避免除零错误（总数据量为0时显示0%）
    double bitErrorRate = 0.0;
    if (m_totalBits > 0) {
        bitErrorRate = (double)m_errorBits / m_totalBits * 100;
    }

    double byteErrorRate = 0.0;
    if (m_totalBytes > 0) {
        byteErrorRate = (double)m_errorBytes / m_totalBytes * 100;
    }

    double frameErrorRate = 0.0;
    if (m_totalFrames > 0) {
        frameErrorRate = (double)m_errorFrames / m_totalFrames * 100;
    }

    // 更新UI显示（保留2位小数）
    m_bitErrorLabel->setText(QString::number(bitErrorRate, 'f', 2) + "%");
    m_byteErrorLabel->setText(QString::number(byteErrorRate, 'f', 2) + "%");
    m_frameErrorLabel->setText(QString::number(frameErrorRate, 'f', 2) + "%");
}

// 重置统计数据
void ErrorStatistic::reset()
{
    m_totalBits = 0;
    m_errorBits = 0;
    m_totalBytes = 0;
    m_errorBytes = 0;
    m_totalFrames = 0;
    m_errorFrames = 0;
    m_recvData.clear();

    // 重置显示为0%
    m_bitErrorLabel->setText("0.00%");
    m_byteErrorLabel->setText("0.00%");
    m_frameErrorLabel->setText("0.00%");
}


