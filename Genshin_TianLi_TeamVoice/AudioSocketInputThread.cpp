#include "AudioSocketInputThread.h"

AudioSocketInputThread::AudioSocketInputThread(QWebSocket* ioWebSocket, QObject* parent)
    : QThread(parent)
{
    webSocket = ioWebSocket;
}

AudioSocketInputThread::~AudioSocketInputThread()
{
    stopInput();

    delete input;
    delete inputDevice;
}

void AudioSocketInputThread::setAudioInputDevice(const QAudioDeviceInfo& audioDevice)
{
    m_audioInputDevice = audioDevice;
}

void AudioSocketInputThread::setAudioInputFormat(int sampleRate, int channelCount, int sampleSize) 
{
    formatInput.setSampleRate(sampleRate);
    formatInput.setChannelCount(channelCount);
    formatInput.setSampleSize(sampleSize);
    formatInput.setCodec("audio/pcm");
    formatInput.setSampleType(QAudioFormat::SignedInt);
    formatInput.setByteOrder(QAudioFormat::LittleEndian);
    input = new QAudioInput(m_audioInputDevice, formatInput, this);
}

void AudioSocketInputThread::startInput() {
    qDebug() << "audio begins to send";
    inputDevice = input->start();
    connect(inputDevice, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void AudioSocketInputThread::stopInput() {
    qDebug() << "input Stop!";
    input->stop();
}

void AudioSocketInputThread::onReadyRead()
{
    VoiceFrame vp;
    memset(&vp.data, 0, sizeof(vp));
    vp.lens = inputDevice->read(vp.data, 960);
    int num = webSocket->sendBinaryMessage(QByteArray((const char*)&vp, sizeof(vp)));
    qDebug() << num;
}



