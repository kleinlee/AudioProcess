#include "audioprocess.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>

AudioProcess::AudioProcess()
{
    m_url = QUrl(QStringLiteral("wss://voiceapi.xiaoi.com/api/v1/metalVoice?voiceName=aliyun-siyue&enableTimestamps=1"));
//    m_url = QUrl(QStringLiteral("wss://voiceapi.xiaoi.com/api/v1/metalVoice"));

    qDebug() << "WebSocket server:" << m_url;
    connect(&m_webSocket, &QWebSocket::connected, this, &AudioProcess::onConnected);
    m_webSocket.open(QUrl(m_url));

//    QFile file("E:/2333.raw");
//    file.open(QIODevice::ReadOnly);
//    buffer_audio = file.readAll();
//    file.close();
//    QTimer* timer = new QTimer(this);
//    connect(timer, &QTimer::timeout, this, &AudioProcess::onUpdate);
//    timer->start(20);


    m_audioFormat.setSampleRate(16000);
    m_audioFormat.setChannelCount(1);
    m_audioFormat.setSampleFormat(QAudioFormat::Int16);
    m_audioInput.reset(new QAudioSource(QMediaDevices::defaultAudioInput(), m_audioFormat));


    m_generator.reset(new Generator(m_audioFormat));
    m_audioOutput.reset(new QAudioSink(QMediaDevices::defaultAudioOutput(), m_audioFormat));
    m_generator->start();

    // 使m_audioOutput初始状态维持在QAudio::IdleState
    m_audioOutput->start();

    // m_audioOutput状态变化为IdleState->StoppedState->ActiveState(说话开始)->IdleState(说话结束)
    //因此，在每次说话结束时要看一下是否还有buffer要说话
    connect(m_audioOutput.data(), SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
}

void AudioProcess::handleStateChanged(QAudio::State newState)
{
////    qDebug() << "QAudio::IdleState" << newState;
//    switch (newState) {
//        case QAudio::IdleState:
//            // Finished playing (no more data)
//            makeAudioSpeak();
//            break;

//        case QAudio::StoppedState:
//            // Stopped for other reasons
//            if (m_audioOutput->error() != QAudio::NoError) {
//            }
//            break;

//        default:
//            break;
//    }
}


void AudioProcess::play()
{
    qDebug() << "YYYYYYYYYY " << m_audioInput->state() << m_audioOutput->state();

    auto *io = m_audioInput->start();
    if (!io)
        return;

    connect(io, &QIODevice::readyRead,
        [this, io]() {
        static const qint64 BufferSize = 4096;
        const qint64 len = qMin(m_audioInput->bytesAvailable(), BufferSize);

        QByteArray buffer(len, 0);
        qint64 l = io->read(buffer.data(), len);

//        qDebug() << "buffer size" << buffer.size() << m_buffer_audio.size() << m_buffer_audio_recived.size();
        if (l > 0) {
            m_buffer_audio += buffer;
        }

        // 发送音频数据
        while (m_buffer_audio.size() > 320)
        {
            QByteArray sss = m_buffer_audio.sliced(0, 320);
            qint64 sent_size = m_webSocket.sendBinaryMessage(sss);
    //        qDebug() << "sent_size  " << sent_size;
            m_buffer_audio.remove(0, sent_size);
        }

        makeAudioSpeak();
    });

//    QFile file("E:/2333.raw");
//    file.open(QIODevice::WriteOnly);
//    file.write(buffer_audio);
//    file.close();
}

void AudioProcess::play2()
{
    m_audioOutput->stop();
    m_generator->writeData(m_buffer_audio_recived, m_buffer_audio_recived.size());
    m_buffer_audio_recived.clear();
    m_audioOutput->start(m_generator.data());
}

void AudioProcess::processASR(QUrl fileUrl)
{

}

void AudioProcess::makeAudioSpeak()
{
    if (m_buffer_audio_recived.size() >= 0) {
        m_generator->writeData(m_buffer_audio_recived, m_buffer_audio_recived.size());
        m_buffer_audio_recived.clear();
        if (m_audioOutput->state() == QAudio::IdleState)
        {
            qDebug() << "m_audioOutput->state() == QAudio::IdleState";
            m_audioOutput->start(m_generator.data());
        }
    }
}

void AudioProcess::onBytesWritten(qint64 bytes)
{
    qDebug() << "onBytesWritten " << bytes;
}

void AudioProcess::onUpdate()
{
    static int tag_ = 0;
//    tag_ = (tag_ + 1)%10;

}

void AudioProcess::onConnected()
{
    qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &AudioProcess::onTextMessageReceived);
    connect(&m_webSocket, &QWebSocket::binaryMessageReceived,
            this, &AudioProcess::onBinaryMessageReceived);
    m_webSocket.sendTextMessage(QStringLiteral("[start]"));
}

void AudioProcess::onTextMessageReceived(QString message)
{
    qDebug() << "onTextMessageReceived Message received:" << message;
}
//! [onTextMessageReceived]

void AudioProcess::onBinaryMessageReceived(QByteArray message)
{
    static int hhhh_ = 0;
    hhhh_ ++;
    qDebug() << "onBinaryMessageReceived Message received:" << message.size() << hhhh_;
    m_buffer_audio_recived += message;
}

void AudioProcess::onTextFrameReceived(QByteArray frame, bool isLastFrame)
{
    qDebug() << "onTextFrameReceived Message received:" << frame;
}
void AudioProcess::onBinaryFrameReceived(QByteArray frame, bool isLastFrame)
{
    qDebug() << "onBinaryFrameReceived Message received:" << frame;
}
