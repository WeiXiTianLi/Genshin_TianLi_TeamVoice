#include "AudioInputSendThread.h"

AudioInputSendThread::AudioInputSendThread(QObject *parent)
	: QThread(parent)
{
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 23333);
    sendAddress.setAddress("127.0.0.1");//改成你的目的地址就行
}

AudioInputSendThread::~AudioInputSendThread()
{
    delete udpSocket;
    delete input;
    delete inputDevice;
}

void AudioInputSendThread::setAudioDevice(QAudioDeviceInfo audioDevice)
{
    m_audioDevice = audioDevice;
}

void AudioInputSendThread::setAudioFormat(int samplerate, int channelcount, int samplesize)
{
    input_format.setSampleRate(samplerate);
    input_format.setChannelCount(channelcount);
    input_format.setSampleSize(samplesize);
    input_format.setCodec("audio/pcm");
    input_format.setSampleType(QAudioFormat::SignedInt);
    input_format.setByteOrder(QAudioFormat::LittleEndian);

    input = new QAudioInput(m_audioDevice,input_format);
}

void AudioInputSendThread::start()
{
    qDebug() << "AudioInputSendThread::start" << "\n";

    inputDevice = input->start();
    connect(inputDevice, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void AudioInputSendThread::stop()
{
    qDebug() << "AudioInputSendThread::stop" << "\n";
    input->stop();
}
void AudioInputSendThread::onReadyRead()
{
    video vp;
    memset(&vp.data, 0, sizeof(vp));

    // read audio from input device
    vp.lens = inputDevice->read(vp.data, 960);
    int num = udpSocket->writeDatagram((const char*)&vp, sizeof(vp), sendAddress, 23333);
}