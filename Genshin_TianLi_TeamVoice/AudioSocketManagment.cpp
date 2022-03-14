#include "AudioSocketManagment.h"

#include <QUrl>
#include <QDateTime>

AudioSocketManagment::AudioSocketManagment(QObject* parent)
    : QObject(parent)
{

    webSocket = new QWebSocket();
    webSocket->setParent(this);
    webSocket->open(QUrl("ws://ddns.minemc.top:54321?id=" + QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch())));

    audioSocketInputThread = new AudioSocketInputThread(webSocket);
    audioSocketOutputThread = new AudioSocketOutputThread(webSocket);
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

void AudioSocketManagment::setOutputVolumn(qreal volumn)
{
    audioSocketOutputThread->setOutputVolumn(volumn);
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