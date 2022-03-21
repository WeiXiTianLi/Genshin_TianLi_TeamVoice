#include "AudioSocketOutputThread.h"

AudioSocketOutputThread::AudioSocketOutputThread(QWebSocket* ioWebSocket, QObject* parent)
    : QThread(parent)
{
    m_PCMDataBuffer.clear();
    webSocket = ioWebSocket;
    connect(webSocket, &QWebSocket::binaryMessageReceived, this, &AudioSocketOutputThread::webSocket_binaryMessageReceived);
}

AudioSocketOutputThread::~AudioSocketOutputThread()
{

    for (auto audioOutputUserThread : audioOutputUserThreads)
    {
        delete audioOutputUserThread;
    }

    cv_updata.notify_all();
    std::lock_guard<std::mutex> lock(mutex_updata);

    stopOutput();
    delete m_OutPut;
    //delete m_AudioIo;
}

void AudioSocketOutputThread::addAudioBuffer(char* pData, int len)
{
    std::lock_guard<std::mutex> lock(mutex_updata);
    m_PCMDataBuffer.append(pData, len);
    cv_updata.notify_all();
}

void AudioSocketOutputThread::cleanAllAudioBuffer(void)
{
    std::lock_guard<std::mutex> lock(mutex_updata);
    m_CurrentPlayIndex = 0;
    m_PCMDataBuffer.clear();
    m_IsPlaying = false;
}

void AudioSocketOutputThread::run(void)
{
    while (!this->isInterruptionRequested())
    {
        if (!m_IsPlaying)
        {
            break;
        }
        std::unique_lock<std::mutex> lock(mutex_updata);
        cv_updata.wait_for(lock, std::chrono::milliseconds(60), []()->bool {return false; });
        lock.unlock();
        if (m_PCMDataBuffer.size() < m_CurrentPlayIndex + FRAME_LEN_60ms) {//缓冲区不够播放60ms音频
            continue;
        }
        else {
            //拷贝960字节的数据
            char* writeData = new char[FRAME_LEN_60ms];
            memcpy(writeData, &m_PCMDataBuffer.data()[m_CurrentPlayIndex], FRAME_LEN_60ms);

            //20.0 * log10(abs((Convert_8to16bit(vp.data[0], vp.data[1]))))
            dbChange(20.0 * log10(abs((Convert_8to16bit(writeData[0], writeData[1])))));

            // 写入音频数据
            m_AudioIo->write(writeData, FRAME_LEN_60ms);
            m_CurrentPlayIndex += FRAME_LEN_60ms;
            //qDebug() << m_CurrentPlayIndex;
            delete[]writeData;

            //如果长度超过了MAX_AUDIO_LEN就从
            if (m_CurrentPlayIndex > MAX_AUDIO_LEN) {
                m_PCMDataBuffer = m_PCMDataBuffer.right(m_PCMDataBuffer.size() - MAX_AUDIO_LEN);
                m_CurrentPlayIndex -= MAX_AUDIO_LEN;
            }
        }
    }
    m_PCMDataBuffer.clear();
}


void AudioSocketOutputThread::setAudioOutputDevice(QAudioDeviceInfo audioDevice)
{
    m_audioOutputDevice = audioDevice;
    for (auto audioOutputUserThread :audioOutputUserThreads)
    {
        audioOutputUserThread->setAudioOutputDevice(m_audioOutputDevice);
    }
}

void AudioSocketOutputThread::setAudioOutputFormat(int sampleRate, int channelCount, int sampleSize)
{
    std::lock_guard<std::mutex> lock(mutex_updata);

    sampleRate_User = sampleRate;
    channelCount_User = channelCount;
    sampleSize_User = sampleSize;

    formatOutput.setSampleRate(sampleRate);
    formatOutput.setSampleSize(sampleSize);
    formatOutput.setChannelCount(channelCount);
    formatOutput.setCodec("audio/pcm");
    formatOutput.setSampleType(QAudioFormat::UnSignedInt);
    formatOutput.setByteOrder(QAudioFormat::LittleEndian);

    if (m_OutPut != nullptr) delete m_OutPut;

    m_OutPut = new QAudioOutput(m_audioOutputDevice, formatOutput);
    m_AudioIo = m_OutPut->start();

    for (auto audioOutputUserThread : audioOutputUserThreads)
    {
        audioOutputUserThread->setAudioOutputFormat(sampleRate, channelCount, sampleSize);
    }
}

void AudioSocketOutputThread::setOutputVolumn(qreal volumn)
{
    if (m_OutPut != nullptr) m_OutPut->setVolume(volumn);
}

void AudioSocketOutputThread::startOutput()
{
    start();

    for (auto audioOutputUserThread : audioOutputUserThreads)
    {
        audioOutputUserThread->startOutput();
    }
}

void AudioSocketOutputThread::stopOutput()
{
    if (m_OutPut != nullptr)m_OutPut->stop();
    cleanAllAudioBuffer();

    for (auto audioOutputUserThread : audioOutputUserThreads)
    {
        audioOutputUserThread->stopOutput();
    }
}

void AudioSocketOutputThread::webSocket_binaryMessageReceived(const QByteArray& message)
{
    VoiceFrame* vp_ptr = (VoiceFrame*)message.data();
    int uid = vp_ptr->uid;

    // 如果uid没有，就在共用音频输出设备输出，并直接返回
    if (uid == 0)
    {
        addAudioBuffer(vp_ptr->data, vp_ptr->lens);
        return;
    }

    // 尝试查询uid，找到对应的输出线程
    if (audioOutputUserThreads[uid] == nullptr)
    {
        audioOutputUserThreads[uid] = new AudioOutputThread(uid);
        audioOutputUserThreads[uid]->setAudioOutputDevice(m_audioOutputDevice);
        audioOutputUserThreads[uid]->setAudioOutputFormat(sampleRate_User, channelCount_User, sampleSize_User);
        connect(audioOutputUserThreads[uid], &AudioOutputThread::dbChange, [&](int dbValue) {this->dbChange_User(uid, dbValue); });
    }
    audioOutputUserThreads[uid]->addAudioBuffer(vp_ptr->data, vp_ptr->lens);
}

void AudioSocketOutputThread::setAudioOutputDevice_User(int uid, QAudioDeviceInfo audioDevice)
{
    audioOutputUserThreads[uid]->setAudioOutputDevice(audioDevice);
}

void AudioSocketOutputThread::setAudioOutputFormat_User(int uid, int sampleRate, int channelCount, int sampleSize)
{
    audioOutputUserThreads[uid]->setAudioOutputFormat(sampleRate, channelCount, sampleSize);
}

void AudioSocketOutputThread::setOutputVolumn_User(int uid, qreal volumn)
{
    audioOutputUserThreads[uid]->setOutputVolumn(volumn);
}

void AudioSocketOutputThread::startOutput_User(int uid)
{
    audioOutputUserThreads[uid]->startOutput();
}

void AudioSocketOutputThread::stopOutput_User(int uid)
{
    audioOutputUserThreads[uid]->stopOutput();
}



