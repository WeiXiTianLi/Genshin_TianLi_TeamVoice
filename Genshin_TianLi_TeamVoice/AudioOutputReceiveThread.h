#pragma once

#include <QThread>

#include <QObject>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QMutex>
#include <QMutexLocker>
#include <QByteArray>

#include <QtNetwork/QUdpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDebug>

#define MAX_AUDIO_LEN 960000 //������ջ��������������ֵ�ͼ���
#define FRAME_LEN_60ms 960 //ÿһ������֡������960�ֽ�

class AudioOutputReceiveThread : public QThread
{
	Q_OBJECT

public:
	explicit AudioOutputReceiveThread(QObject * parent = nullptr);
	~AudioOutputReceiveThread();
    // ----------- ���������� ----------------------------------------
    // ���õ�ǰ��PCM Buffer
    void setCurrentBuffer(QByteArray buffer);
    // �������
    void addAudioBuffer(char* pData, int len);
    // ��յ�ǰ������
    void cleanAllAudioBuffer(void);
    // ------------- End ----------------------------------------------
    void setAudioDevice(QAudioDeviceInfo audioDevice);
    // ���õ�ǰ�Ĳ����ʡ�����λ����ͨ����Ŀ
    void setCurrentSampleInfo(int sampleRate, int sampleSize, int channelCount);

    virtual void run(void) override;//���߳��������к���run

    // ��������
    void setCurrentVolumn(qreal volumn);

    void stop();//ֹͣ

private:
    QAudioDeviceInfo m_audioDevice;
    QAudioOutput* m_OutPut = nullptr;
    QIODevice* m_AudioIo = nullptr;

    QByteArray m_PCMDataBuffer;
    int m_CurrentPlayIndex = 0;

    QMutex m_Mutex;
    // ����״̬
    volatile bool m_IsPlaying = true;

    //for Audio
    QUdpSocket* udpsocket;

    struct video {
        int lens;
        char data[960];
    };

private slots:
    void readyReadSlot();
};
