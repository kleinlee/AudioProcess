#ifndef AUDIOPROCESS_H
#define AUDIOPROCESS_H

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QAudioSource>
#include <QAudioFormat>
#include <QMediaDevices>
#include <QAudioSink>
#include <QtWebSockets/QWebSocket>
#include "audioio.h"

class AudioProcess : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(AudioProcess)
public:
    AudioProcess();

    Q_INVOKABLE void processASR(QUrl fileUrl);
    Q_INVOKABLE void play();
    Q_INVOKABLE void play2();

private:

    QScopedPointer<QAudioSource> m_audioInput;
    QScopedPointer<QAudioSink>   m_audioOutput;

    QAudioFormat m_audioFormat;
    QAudioDevice *m_devices = nullptr;

    QScopedPointer<Generator> m_generator;

    bool m_audio_play_mode = true;

    QIODevice* m_IO_input;
    QIODevice* m_IO_output;

    QByteArray m_buffer_audio;
    QByteArray buffer_audio;

    QByteArray m_buffer_audio_recived;

    int m_byte_num = 0;

    QWebSocket m_webSocket;
    QUrl m_url;

private slots:
    void handleStateChanged(QAudio::State newState);

    void makeAudioSpeak();



    void onBytesWritten(qint64 bytes);


    void onConnected();
    void onTextMessageReceived(QString message);
    void onBinaryMessageReceived(QByteArray message);

    void onTextFrameReceived(QByteArray frame, bool isLastFrame);
    void onBinaryFrameReceived(QByteArray frame, bool isLastFrame);

    void onUpdate();
};

#endif // AUDIOPROCESS_H
