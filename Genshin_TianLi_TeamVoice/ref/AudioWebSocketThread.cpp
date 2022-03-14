#include "AudioWebSocketThread.h"

#include <QDateTime>

AudioWebSocketThread::AudioWebSocketThread(QObject *parent)
    : QThread(parent)
{

    webSocket = new QWebSocket();
    webSocket->setParent(this);
    webSocket->open(QUrl("ws://ddns.minemc.top:54321?id="+ QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch())));

    connect(webSocket, &QWebSocket::binaryMessageReceived, this, &AudioWebSocketThread::webSocket_binaryMessageReceived);
    //qDebug()<<webSocket->error()<< webSocket->errorString();
}
AudioWebSocketThread::~AudioWebSocketThread(){
    //delete udpSocket;
    delete webSocket;
    delete input;
    delete inputDevice;
    delete m_OutPut;
    delete m_AudioIo;

}

void AudioWebSocketThread::setCurrentBuffer(QByteArray buffer)
{
}

void AudioWebSocketThread::addAudioBuffer(char* pData, int len)
{
    QMutexLocker locker(&m_Mutex);

    m_PCMDataBuffer.append(pData, len);
}

void AudioWebSocketThread::cleanAllAudioBuffer(void)
{
    QMutexLocker locker(&m_Mutex);
    m_CurrentPlayIndex = 0;
    m_PCMDataBuffer.clear();
    m_IsPlaying = false;
}

void AudioWebSocketThread::run(void)
{
    qDebug() << "Start Output Run!";

    while (!this->isInterruptionRequested())
    {
        qDebug() << " Run....";

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

void AudioWebSocketThread::setAudioInputDevice(QAudioDeviceInfo audioDevice)
{
    m_audioInputDevice = audioDevice;
}

void AudioWebSocketThread::setAudioOutputDevice(QAudioDeviceInfo audioDevice)
{
    m_audioOutputDevice = audioDevice;
}

void AudioWebSocketThread::setAudioInputFormat(int sampleRate, int channelCount, int sampleSize){
    formatInput.setSampleRate(sampleRate);
    formatInput.setChannelCount(channelCount);
    formatInput.setSampleSize(sampleSize);
    formatInput.setCodec("audio/pcm");
    formatInput.setSampleType(QAudioFormat::SignedInt);
    formatInput.setByteOrder(QAudioFormat::LittleEndian);

    input = new QAudioInput(m_audioInputDevice,formatInput, this);

}

void AudioWebSocketThread::setAudioOutputFormat(int sampleRate, int channelCount, int sampleSize)
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

void AudioWebSocketThread::setOutputVolumn(qreal volumn)
{
    m_OutPut->setVolume(volumn);
}

void AudioWebSocketThread::startInput(){
    qDebug()<<"audio begins to send";
    inputDevice = input->start();
    connect(inputDevice,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
}

void AudioWebSocketThread::startOutput()
{
    run();
}

void AudioWebSocketThread::stopInput(){
    qDebug()<<"input Stop!";
    input->stop();
}

void AudioWebSocketThread::stopOutput()
{
    m_OutPut->stop();
    cleanAllAudioBuffer();
}

void AudioWebSocketThread::webSocket_binaryMessageReceived(const QByteArray& message)
{
    //video vp;
    //memset(&vp, 0, sizeof(vp));
    
    video *vp_ptr = (video*)message.data();
    addAudioBuffer(vp_ptr->data, vp_ptr->lens);
    //qDebug() <<"Received byte:" << message.size();
}

void AudioWebSocketThread::onReadyRead()
{
    video vp;
    memset(&vp.data,0,sizeof(vp));
    vp.lens = inputDevice->read(vp.data, 960);
    int num = webSocket->sendBinaryMessage(QByteArray((const char*)&vp, sizeof(vp)));
}
