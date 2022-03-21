#include "AudioSocketOutputThread.h"


#define MAX_AUDIO_LEN 960000 //如果接收缓冲区大于这个数值就剪掉
#define FRAME_LEN_60ms 960 //每一个语音帧长度是960字节

AudioSocketOutputThread::AudioSocketOutputThread(QWebSocket* ioWebSocket, QObject* parent)
    : QThread(parent)
{
    m_PCMDataBuffer.clear();
    webSocket = ioWebSocket;
    connect(webSocket, &QWebSocket::binaryMessageReceived, this, &AudioSocketOutputThread::webSocket_binaryMessageReceived);
}

AudioSocketOutputThread::~AudioSocketOutputThread()
{
    stopOutput();
    delete m_OutPut;
    delete m_AudioIo;
}

void AudioSocketOutputThread::addAudioBuffer(char* pData, int len)
{
    QMutexLocker locker(&m_Mutex);
    m_PCMDataBuffer.append(pData, len);
}

void AudioSocketOutputThread::cleanAllAudioBuffer(void)
{
    QMutexLocker locker(&m_Mutex);
    m_CurrentPlayIndex = 0;
    m_PCMDataBuffer.clear();
    m_IsPlaying = false;
}

void AudioSocketOutputThread::run(void)
{
    qDebug() << "Start Output Run!";

    while (!this->isInterruptionRequested())
    {
        //qDebug() << " Run....";

        if (!m_IsPlaying)
        {
            break;
        }

        QMutexLocker locker(&m_Mutex);

        if (m_PCMDataBuffer.size() < m_CurrentPlayIndex + FRAME_LEN_60ms) {//缓冲区不够播放60ms音频
            continue;
        }
        else {
            //拷贝960字节的数据
            char* writeData = new char[FRAME_LEN_60ms];
            memcpy(writeData, &m_PCMDataBuffer.data()[m_CurrentPlayIndex], FRAME_LEN_60ms);
            // 写入音频数据
            m_AudioIo->write(writeData, FRAME_LEN_60ms);
            m_CurrentPlayIndex += FRAME_LEN_60ms;
            qDebug() << m_CurrentPlayIndex;
            delete[]writeData;

            //如果长度超过了MAX_AUDIO_LEN就从
            if (m_CurrentPlayIndex > MAX_AUDIO_LEN) {
                m_PCMDataBuffer = m_PCMDataBuffer.right(m_PCMDataBuffer.size() - MAX_AUDIO_LEN);
                m_CurrentPlayIndex -= MAX_AUDIO_LEN;
            }
        }
    }
    m_PCMDataBuffer.clear();
    qDebug() << "audio receiver stop!";
}


void AudioSocketOutputThread::setAudioOutputDevice(QAudioDeviceInfo audioDevice)
{
    m_audioOutputDevice = audioDevice;
}

void AudioSocketOutputThread::setAudioOutputFormat(int sampleRate, int channelCount, int sampleSize)
{
    QMutexLocker locker(&m_Mutex);

    formatOutput.setSampleRate(sampleRate);
    formatOutput.setSampleSize(sampleSize);
    formatOutput.setChannelCount(channelCount);
    formatOutput.setCodec("audio/pcm");
    formatOutput.setSampleType(QAudioFormat::SignedInt);
    formatOutput.setByteOrder(QAudioFormat::LittleEndian);

    if (m_OutPut != nullptr) delete m_OutPut;

    m_OutPut = new QAudioOutput(m_audioOutputDevice, formatOutput);
    m_AudioIo = m_OutPut->start();
}

void AudioSocketOutputThread::setOutputVolumn(qreal volumn)
{
    m_OutPut->setVolume(volumn);
}

void AudioSocketOutputThread::startOutput()
{
    //run();
}


void AudioSocketOutputThread::stopOutput()
{
    m_OutPut->stop();
    cleanAllAudioBuffer();
}

void AudioSocketOutputThread::webSocket_binaryMessageReceived(const QByteArray& message)
{
    addAudioBuffer(vp_ptr->data, vp_ptr->lens);
    qDebug() <<"Received byte:" << message.size();
}
