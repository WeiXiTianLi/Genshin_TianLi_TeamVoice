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

    QAudioDeviceInfo m_audioInputDevice;
    QAudioDeviceInfo m_audioOutputDevice;

    QWebSocket* webSocket;

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

class AudioSocketInputThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioSocketInputThread(QWebSocket* webSocket,QObject* parent = nullptr);
    ~AudioSocketInputThread();

    QAudioDeviceInfo m_audioInputDevice;

    QWebSocket* webSocket;

    QAudioInput* input = nullptr;
    QIODevice* inputDevice = nullptr;

    QAudioFormat formatInput;

    struct video {
        int lens;
        char data[960];
    };

    void setAudioInputDevice(QAudioDeviceInfo audioDevice);

    void setAudioInputFormat(int sampleRate, int channelCount, int sampleSize);


    void startInput();
    void stopInput();

public slots:
    void onReadyRead();
};

class AudioSocketOutputThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioSocketOutputThread(QWebSocket* webSocket,QObject* parent = nullptr);
    ~AudioSocketOutputThread();

    QAudioDeviceInfo m_audioOutputDevice;

    QWebSocket* webSocket;


    QAudioOutput* m_OutPut = nullptr;
    QIODevice* m_AudioIo = nullptr;

    QAudioFormat formatOutput;

    QByteArray m_PCMDataBuffer;
    int m_CurrentPlayIndex = 0;

    QMutex m_Mutex;
    // ����״̬
    volatile bool m_IsPlaying = true;

    struct video {
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

    void setAudioOutputDevice(QAudioDeviceInfo audioDevice);

    void setAudioOutputFormat(int sampleRate, int channelCount, int sampleSize);

    void setOutputVolumn(qreal volumn);

    void startOutput();
    void stopOutput();

public slots:
    void webSocket_binaryMessageReceived(const QByteArray& message);
};

class AudioSocketManagment: public QObject
{
        Q_OBJECT
    public:
        explicit AudioSocketManagment(QObject * parent = nullptr);
        ~AudioSocketManagment();


        QWebSocket* webSocket;

        AudioSocketInputThread *audioSocketInputThread;
        AudioSocketOutputThread *audioSocketOutputThread;

        void setAudioInputDevice(QAudioDeviceInfo audioDevice);
        void setAudioOutputDevice(QAudioDeviceInfo audioDevice);

        void setAudioInputFormat(int sampleRate, int channelCount, int sampleSize);
        void setAudioOutputFormat(int sampleRate, int channelCount, int sampleSize);

        void setOutputVolumn(qreal volumn);

        void startInput();
        void startOutput();
        void stopInput();
        void stopOutput();

    //public slots:
        //void onReadyRead();
        //void webSocket_binaryMessageReceived(const QByteArray& message);

};

#endif // AUDIOSENDTHREAD_H


