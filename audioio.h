#ifndef AUDIOIO_H
#define AUDIOIO_H


#include <QAudioSource>
#include <QMediaDevices>

#include <QByteArray>
#include <QScopedPointer>

class AudioInfo : public QIODevice
{
    Q_OBJECT

public:
    AudioInfo(const QAudioFormat &format);

    void start();
    void stop();

    qreal level() const { return m_level; }

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

    qreal calculateLevel(const char *data, qint64 len) const;

signals:
    void levelChanged(qreal level);

private:
    const QAudioFormat m_format;
    qreal m_level = 0.0; // 0.0 <= m_level <= 1.0
};

class Generator : public QIODevice
{
    Q_OBJECT

public:
    Generator(const QAudioFormat &format);

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;
    qint64 size() const override { return m_buffer.size(); }

private:
    QByteArray m_buffer;
};

#endif // AUDIOIO_H
