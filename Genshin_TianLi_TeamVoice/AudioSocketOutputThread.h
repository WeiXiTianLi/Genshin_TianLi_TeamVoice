#pragma once

#include <QThread>
#include <QMutex>
#include <QIODevice>
#include <QWebSocket>
#include <QAudioFormat>
#include <QAudioOutput>

#include "VoiceFrame.h"
class AudioSocketOutputThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioSocketOutputThread(QWebSocket* ioWebSocket, QObject* parent = nullptr);
    ~AudioSocketOutputThread();

    QAudioDeviceInfo m_audioOutputDevice= QAudioDeviceInfo::defaultOutputDevice();

    QWebSocket* webSocket;


    QAudioOutput* m_OutPut = nullptr;
    QIODevice* m_AudioIo = nullptr;

    QAudioFormat formatOutput;

    QByteArray m_PCMDataBuffer;
    int m_CurrentPlayIndex = 0;

    QMutex m_Mutex;
    // ����״̬
    volatile bool m_IsPlaying = true;


    // ----------- ���������� ----------------------------------------
    // �������
    void addAudioBuffer(char* pData, int len);
    // ��յ�ǰ������
    void cleanAllAudioBuffer(void);
    // ------------- End ----------------------------------------------

    virtual void run(void) override;//���߳��������к���run

    void setAudioOutputDevice(QAudioDeviceInfo audioDevice);

    void setAudioOutputFormat(int sampleRate, int channelCount, int sampleSize);

    void setOutputVolumn(qreal volumn);

    void startOutput();
    void stopOutput();

public slots:
    void webSocket_binaryMessageReceived(const QByteArray& message);
signals:
    void dbChange(int dbValue);
};
