#pragma once

#include <QObject>
#include <QWebSocket>
#include <QAudioDeviceInfo>

#include "AudioSocketInputThread.h"
#include "AudioSocketOutputThread.h"

class AudioSocketManagment : public QObject
{
    Q_OBJECT
public:
    explicit AudioSocketManagment(QObject* parent = nullptr);
    ~AudioSocketManagment();


    QWebSocket* webSocket;

    AudioSocketInputThread* audioSocketInputThread;
    AudioSocketOutputThread* audioSocketOutputThread;

    void setAudioInputDevice(const QAudioDeviceInfo& audioDevice);
    void setAudioOutputDevice(QAudioDeviceInfo audioDevice);

    void setAudioInputFormat(int sampleRate, int channelCount, int sampleSize);
    void setAudioOutputFormat(int sampleRate, int channelCount, int sampleSize);

    void setOutputVolumn(qreal volumn);

    void startInput();
    void startOutput();
    void stopInput();
    void stopOutput();

};