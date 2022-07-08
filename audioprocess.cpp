#include "audioprocess.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>

AudioProcess::AudioProcess()
{
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
//    qDebug() << "QAudio::IdleState" << newState;
    switch (newState) {
        case QAudio::IdleState:
            // Finished playing (no more data)
            makeAudioSpeak();
            break;

        case QAudio::StoppedState:
            // Stopped for other reasons
            if (m_audioOutput->error() != QAudio::NoError) {
            }
            break;

        default:
            break;
    }
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

        qDebug() << "buffer size" << buffer.size();
        if (l > 0) {
            m_buffer_audio += buffer;
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
}

void AudioProcess::processASR(QUrl fileUrl)
{

}

void AudioProcess::makeAudioSpeak()
{
    if (m_buffer_audio.size() > 10000 and m_audioOutput->state() == QAudio::IdleState) {
        m_audioOutput->stop();
        m_generator->writeData(m_buffer_audio, 10000);
        m_buffer_audio.remove(0, 10000);
        m_audioOutput->start(m_generator.data());
    }
}
