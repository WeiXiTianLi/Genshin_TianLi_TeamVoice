#include "AudioSocketManagment.h"

#include <QUrl>
#include <QDateTime>

AudioSocketManagment::AudioSocketManagment(QObject* parent)
    : QObject(parent)
{

    webSocket = new QWebSocket();
    webSocket->setParent(this);

    connect(webSocket, &QWebSocket::textMessageReceived, this, &AudioSocketManagment::recviceTextMessage);
    //connect(webSocket, &QWebSocket::textMessageReceived, [](QString message) {qDebug() << message;});

    audioSocketInputThread = new AudioSocketInputThread(webSocket);
    audioSocketOutputThread = new AudioSocketOutputThread(webSocket);

    // °ó¶¨·Ö±´Êý
    connect(audioSocketInputThread, &AudioSocketInputThread::dbChange, this, &AudioSocketManagment::dbInputChange);
    connect(audioSocketOutputThread, &AudioSocketOutputThread::dbChange, this, &AudioSocketManagment::dbOutputChange);
}

AudioSocketManagment::~AudioSocketManagment()
{
    delete webSocket;
    delete audioSocketInputThread;
    delete audioSocketOutputThread;
}

void AudioSocketManagment::setAudioInputDevice(const QAudioDeviceInfo& audioDevice)
{
    audioSocketInputThread->setAudioInputDevice(audioDevice);
}

void AudioSocketManagment::setAudioOutputDevice(QAudioDeviceInfo audioDevice)
{
    audioSocketOutputThread->setAudioOutputDevice(audioDevice);
}

void AudioSocketManagment::setAudioInputFormat(int sampleRate, int channelCount, int sampleSize)
{
    audioSocketInputThread->setAudioInputFormat(sampleRate, channelCount, sampleSize);
}

void AudioSocketManagment::setAudioOutputFormat(int sampleRate, int channelCount, int sampleSize)
{
    audioSocketOutputThread->setAudioOutputFormat(sampleRate, channelCount, sampleSize);
}

void AudioSocketManagment::setInputVolumn(qreal volumn)
{
}

void AudioSocketManagment::setOutputVolumn(qreal volumn)
{
    audioSocketOutputThread->setOutputVolumn(volumn);
}

void AudioSocketManagment::startSendRecviceSocket(QString url)
{
    switch (webSocket->state())
    {
    case QAbstractSocket::SocketState::UnconnectedState:
    {
        webSocket->open(QUrl(url));
        break;
    }
    default:
    {
        webSocket->close();
        webSocket->open(QUrl(url));
        break;
    }
    }
}

void AudioSocketManagment::stopSendRecviceSocket()
{
    webSocket->close();
}

void AudioSocketManagment::startInput()
{
    audioSocketInputThread->startInput();
}

void AudioSocketManagment::startOutput()
{
    //audioSocketOutputThread->startOutput();
    //
    audioSocketOutputThread->start();
}

void AudioSocketManagment::stopInput()
{
    audioSocketInputThread->stopInput();
}

void AudioSocketManagment::stopOutput()
{
    audioSocketOutputThread->stopOutput();
}

void AudioSocketManagment::sendTextMessage(const QString& message)
{
    webSocket->sendTextMessage(message);
}