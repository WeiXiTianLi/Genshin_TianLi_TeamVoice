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

    void setInputVolumn(qreal volumn);
    void setOutputVolumn(qreal volumn);

    void startSendRecviceSocket(QString url);
    void stopSendRecviceSocket();

    void startInput();
    void startOutput();
    void stopInput();
    void stopOutput();

public slots:
    void sendTextMessage(const QString& message);
signals:
    void recviceTextMessage(const QString& message);

signals:
    void dbInputChange(int dbValue);
    void dbOutputChange(int dbValue);
};