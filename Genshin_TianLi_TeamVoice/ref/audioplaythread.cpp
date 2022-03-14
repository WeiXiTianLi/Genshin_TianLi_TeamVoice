#include "audioplaythread.h"

AudioPlayThread::AudioPlayThread(QObject *parent)
    :QThread(parent)
{
    m_PCMDataBuffer.clear();

    port = 10004;

    udpsocket = new QUdpSocket(this);
    udpsocket->bind(QHostAddress::Any, port);
    connect(udpsocket,SIGNAL(readyRead()),this,SLOT(readyReadSlot()));//�յ��������ݱ��Ϳ�ʼ��outputDeviceд�룬���в���
}

AudioPlayThread::~AudioPlayThread()
{
    delete udpsocket;
    delete m_OutPut;
    delete m_AudioIo;

}

void AudioPlayThread::setCurrentVolumn(qreal volumn){
    m_OutPut->setVolume(volumn);
}

void AudioPlayThread::setAudioDevice(QAudioDeviceInfo audioDevice)
{
    m_audioDevice = audioDevice;
}
void AudioPlayThread::setCurrentSampleInfo(int sampleRate, int sampleSize, int channelCount)
{
    QMutexLocker locker(&m_Mutex);

    // Format
    QAudioFormat nFormat;
    nFormat.setSampleRate(sampleRate);
    nFormat.setSampleSize(sampleSize);
    nFormat.setChannelCount(channelCount);
    nFormat.setCodec("audio/pcm");
    nFormat.setSampleType(QAudioFormat::SignedInt);
    nFormat.setByteOrder(QAudioFormat::LittleEndian);

    if (m_OutPut != nullptr) delete m_OutPut;

    m_OutPut = new QAudioOutput(m_audioDevice,nFormat);
    m_AudioIo = m_OutPut->start();
    //this->start();
}

void AudioPlayThread::run(void)
{
    while (!this->isInterruptionRequested())
    {
        if (!m_IsPlaying)
        {
            break;
        }

        QMutexLocker locker(&m_Mutex);

        if(m_PCMDataBuffer.size() < m_CurrentPlayIndex + FRAME_LEN_60ms){//��������������60ms��Ƶ
            continue;
        }
        else{
            //����960�ֽڵ�����
            char *writeData = new char[FRAME_LEN_60ms];
            memcpy(writeData,&m_PCMDataBuffer.data()[m_CurrentPlayIndex], FRAME_LEN_60ms);
            // д����Ƶ����
            m_AudioIo->write(writeData, FRAME_LEN_60ms);
            m_CurrentPlayIndex += FRAME_LEN_60ms;
            qDebug()<<m_CurrentPlayIndex;
            delete []writeData;

            //������ȳ�����MAX_AUDIO_LEN�ʹ�
            if(m_CurrentPlayIndex > MAX_AUDIO_LEN){
                m_PCMDataBuffer = m_PCMDataBuffer.right(m_PCMDataBuffer.size()-MAX_AUDIO_LEN);
                m_CurrentPlayIndex -= MAX_AUDIO_LEN;
            }
        }
    }
    m_PCMDataBuffer.clear();
    qDebug()<<"audio receiver stop!";
}

void AudioPlayThread::setCurrentBuffer(QByteArray buffer)
{
}

// �������
void AudioPlayThread::addAudioBuffer(char* pData, int len)
{
    QMutexLocker locker(&m_Mutex);

    m_PCMDataBuffer.append(pData, len);
    //m_IsPlaying = true;
}

void AudioPlayThread::cleanAllAudioBuffer(void)
{
    QMutexLocker locker(&m_Mutex);
    m_CurrentPlayIndex = 0;
    m_PCMDataBuffer.clear();
    m_IsPlaying = false;
}

void AudioPlayThread::readyReadSlot(){
    while(udpsocket->hasPendingDatagrams()){
            QHostAddress senderip;
            quint16 senderport;
            qDebug()<<"audio is being received..."<<endl;
            video vp;
            memset(&vp,0,sizeof(vp));
            udpsocket->readDatagram((char*)&vp,sizeof(vp),&senderip,&senderport);
            //outputDevice->write(vp.data,vp.lens);
            addAudioBuffer(vp.data, vp.lens);
    }
}

void AudioPlayThread::stop(){

    udpsocket->close();
    m_OutPut->stop();
    cleanAllAudioBuffer();
}

