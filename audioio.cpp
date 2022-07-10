#include "audioio.h"
#include <QDebug>
#include <QFile>

AudioInfo::AudioInfo(const QAudioFormat &format)
    : m_format(format)
{
}

void AudioInfo::start()
{
    open(QIODevice::WriteOnly);
}

void AudioInfo::stop()
{
    close();
}

qint64 AudioInfo::readData(char * /* data */, qint64 /* maxlen */)
{
    return 0;
}

qreal AudioInfo::calculateLevel(const char *data, qint64 len) const
{
    const int channelBytes = m_format.bytesPerSample();
    const int sampleBytes = m_format.bytesPerFrame();
    const int numSamples = len / sampleBytes;

    float maxValue = 0;
    auto *ptr = reinterpret_cast<const unsigned char *>(data);

    for (int i = 0; i < numSamples; ++i) {
        for (int j = 0; j < m_format.channelCount(); ++j) {
            float value = m_format.normalizedSampleValue(ptr);

            maxValue = qMax(value, maxValue);
            ptr += channelBytes;
        }
    }
    return maxValue;
}

qint64 AudioInfo::writeData(const char *data, qint64 len)
{
    m_level = calculateLevel(data, len);

    emit levelChanged(m_level);

    return len;
}


Generator::Generator(const QAudioFormat &format)
{
    if (format.isValid())
    {
        qDebug() << format;
    }
}

void Generator::start()
{
    open(QIODevice::ReadWrite);
}

void Generator::stop()
{
    close();
}

qint64 Generator::readData(char *data, qint64 len)
{

//    qDebug() << "Generator::readData  " << len << m_buffer.size();
    qint64 total = 0;
    if (!m_buffer.isEmpty())
    {
        const qint64 chunk = qMin(m_buffer.size(), len);
        memcpy(data + total, m_buffer.constData(), chunk);
        total += chunk;
        m_buffer.remove(0, total);
    }

    return total;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    m_buffer = m_buffer + QByteArray(data, len);
//    Q_UNUSED(data);
//    Q_UNUSED(len);

    return len;
}

qint64 Generator::bytesAvailable() const
{
//    qDebug() << "Generator::bytesAvailable  " << m_buffer.size() + QIODevice::bytesAvailable();
    return m_buffer.size() + QIODevice::bytesAvailable();
}

