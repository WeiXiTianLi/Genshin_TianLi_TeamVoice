#pragma once

#include <condition_variable>
#include <mutex>

#include <QThread>
#include <QIODevice>
#include <QWebSocket>
#include <QAudioFormat>
#include <QAudioOutput>

#include "VoiceFrame.h"
#include "AudioOutputThread.h"

class AudioSocketOutputThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioSocketOutputThread(QWebSocket* ioWebSocket, QObject* parent = nullptr);
    ~AudioSocketOutputThread();

    QMap<int, AudioOutputThread*> audioOutputUserThreads;

    //AudioOutputThread userOutputThread;

    QAudioDeviceInfo m_audioOutputDevice= QAudioDeviceInfo::defaultOutputDevice();

    QWebSocket* webSocket;


    QAudioOutput* m_OutPut = nullptr;
    QIODevice* m_AudioIo = nullptr;
    int sampleRate_User;
    int channelCount_User;
    int sampleSize_User;

    QAudioFormat formatOutput;

    QByteArray m_PCMDataBuffer;
    int m_CurrentPlayIndex = 0;

    std::condition_variable cv_updata;
    std::mutex mutex_updata;

    // 播放状态
    volatile bool m_IsPlaying = true;


    virtual void run(void) override;//多线程重载运行函数run

    void setAudioOutputDevice(QAudioDeviceInfo audioDevice);

    void setAudioOutputFormat(int sampleRate, int channelCount, int sampleSize);

    void setOutputVolumn(qreal volumn);

    void startOutput();
    void stopOutput();
private:
        // ----------- 添加数据相关 ----------------------------------------
    // 添加数据
    void addAudioBuffer(char* pData, int len);
    // 清空当前的数据
    void cleanAllAudioBuffer(void);
    // ------------- End ----------------------------------------------



public slots:
    void webSocket_binaryMessageReceived(const QByteArray& message);
    //void webSocket_binaryMessageReceived_User(int uid, const QByteArray& message);
signals:
    void dbChange(int dbValue);
    void dbChange_User(int uid, int dbValue);

public:
    void setAudioOutputDevice_User(int uid, QAudioDeviceInfo audioDevice);
    void setAudioOutputFormat_User(int uid, int sampleRate, int channelCount, int sampleSize);
    void setOutputVolumn_User(int uid, qreal volumn);
    void startOutput_User(int uid);
    void stopOutput_User(int uid);
};


