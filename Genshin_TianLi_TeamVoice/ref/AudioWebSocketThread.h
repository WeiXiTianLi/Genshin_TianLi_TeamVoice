#ifndef AUDIOSENDTHREAD_H
#define AUDIOSENDTHREAD_H
//���Ƿ����߳�

#include <QObject>
#include <QThread>
#include <QDebug>

#include <QAudio>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>

#include <QWebSocket>
#include <QMutex>
#include <QMutexLocker>
#include <QByteArray>

#include <QHostAddress>
#include <QNetworkInterface>

#define MAX_AUDIO_LEN 960000 //������ջ��������������ֵ�ͼ���
#define FRAME_LEN_60ms 960 //ÿһ������֡������960�ֽ�

class AudioWebSocketThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioWebSocketThread(QObject *parent = nullptr);
    ~AudioWebSocketThread();

    int port = 10005;
    int port_to = 10004;
    QAudioDeviceInfo m_audioInputDevice;
    QAudioDeviceInfo m_audioOutputDevice;

    QWebSocket* webSocket;
    QHostAddress destaddr;

    QAudioInput *input = nullptr;
    QIODevice* inputDevice = nullptr;

    QAudioOutput* m_OutPut = nullptr;
    QIODevice* m_AudioIo     = nullptr;

    QAudioFormat formatInput;
    QAudioFormat formatOutput;

    QByteArray m_PCMDataBuffer;
    int m_CurrentPlayIndex = 0;

    QMutex m_Mutex;
    // ����״̬
    volatile bool m_IsPlaying = true;

    struct video{
        int lens;
        char data[960];
    };

    // ----------- ���������� ----------------------------------------
   // ���õ�ǰ��PCM Buffer
    void setCurrentBuffer(QByteArray buffer);
    // �������
    void addAudioBuffer(char* pData, int len);
    // ��յ�ǰ������
    void cleanAllAudioBuffer(void);
    // ------------- End ----------------------------------------------

    virtual void run(void) override;//���߳��������к���run


    void setAudioInputDevice(QAudioDeviceInfo audioDevice);
    void setAudioOutputDevice(QAudioDeviceInfo audioDevice);

    void setAudioInputFormat(int sampleRate, int channelCount, int sampleSize);
    void setAudioOutputFormat(int sampleRate, int channelCount, int sampleSize);

    void setOutputVolumn(qreal volumn);

    void startInput();
    void startOutput();
    void stopInput();
    void stopOutput();

public slots:
    void onReadyRead();
    void webSocket_binaryMessageReceived(const QByteArray& message);

};

#endif // AUDIOSENDTHREAD_H
