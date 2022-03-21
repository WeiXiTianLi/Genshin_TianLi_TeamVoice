#pragma once

#include <QThread>
#include <QIODevice>
#include <QWebSocket>
#include <QAudioInput>
#include <QAudioFormat>

#include "VoiceFrame.h"

class AudioSocketInputThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioSocketInputThread(QWebSocket* ioWebSocket, QObject* parent = nullptr);
    ~AudioSocketInputThread();

    QAudioDeviceInfo m_audioInputDevice = QAudioDeviceInfo::defaultInputDevice();

    QWebSocket* webSocket;

    QAudioInput* input = nullptr;
    QIODevice* inputDevice = nullptr;

    QAudioFormat formatInput;

    void setAudioInputDevice(const QAudioDeviceInfo& audioDevice);

    void setAudioInputFormat(int sampleRate, int channelCount, int sampleSize);


    void startInput();
    void stopInput();

public slots:
    void onReadyRead();

signals:
    void dbChange(int dbValue);
};

