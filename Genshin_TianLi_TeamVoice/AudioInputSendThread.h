#pragma once

#include <QThread>

#include <QObject>
#include <QDebug>

#include <QAudio>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>

#include <QtNetwork/QUdpSocket>
#include <QHostAddress>

class AudioInputSendThread : public QThread
{
	Q_OBJECT

public:
	explicit AudioInputSendThread(QObject* parent = nullptr);
	~AudioInputSendThread();

    QUdpSocket* udpSocket = nullptr;
    QHostAddress sendAddress;

    QAudioDeviceInfo m_audioDevice;

    QAudioInput* input;
    QIODevice* inputDevice;
    QAudioFormat input_format;

    struct video {
        int lens;
        char data[960];
    };

    void setAudioDevice(QAudioDeviceInfo audioDevice);

    void setAudioFormat(int samplerate, int channelcount, int samplesize);
    void start();
    void stop();

public slots:
    void onReadyRead();
};
