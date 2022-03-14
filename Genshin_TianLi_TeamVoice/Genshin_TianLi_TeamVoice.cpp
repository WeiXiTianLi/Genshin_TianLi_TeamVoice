#include "Genshin_TianLi_TeamVoice.h"

#include <QAudioDeviceInfo>
#include <QComboBox>
Genshin_TianLi_TeamVoice::Genshin_TianLi_TeamVoice(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    set_input_audio_format();

    get_inout_audio_device();

    //connect(ui.comboBox_input, &QComboBox::currentIndexChanged,this, &combobox_currentIndexChanged_inputDevice);

    connect(ui.comboBox_input, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
        [=](const QString& text) {combobox_currentIndexChanged_inputDevice(text, QAudio::Mode::AudioInput); });
    connect(ui.comboBox_output, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
        [=](const QString& text) {combobox_currentIndexChanged_inputDevice(text, QAudio::Mode::AudioOutput); });

    connect(ui.pushButton_getInput, &QPushButton::clicked, this, &Genshin_TianLi_TeamVoice::set_start_input);
    connect(ui.pushButton_setOutput, &QPushButton::clicked, this, &Genshin_TianLi_TeamVoice::set_start_output);

}
void Genshin_TianLi_TeamVoice::set_input_audio_format()
{
    in_audio_format.setSampleRate(8000);
    in_audio_format.setChannelCount(2);   //设定声道数目，mono(平声道)的声道数目是1；stero(立体声)的声道数目是2
    in_audio_format.setSampleSize(16);
    in_audio_format.setCodec("audio/pcm");   //编码器
    in_audio_format.setByteOrder(QAudioFormat::LittleEndian);   //设定高低位,LittleEndian（低位优先）/LargeEndian(高位优先)
    in_audio_format.setSampleType(QAudioFormat::SignedInt);

    if (!audioDevice_input_info.isFormatSupported(in_audio_format))
    {
        in_audio_format = audioDevice_input_info.nearestFormat(in_audio_format);
    }
}
void Genshin_TianLi_TeamVoice::get_inout_audio_device()
{
    ui.comboBox_input->clear();
    ui.comboBox_input->addItems(get_device_list(QAudio::AudioInput));

    ui.comboBox_output->clear();
    ui.comboBox_output->addItems(get_device_list(QAudio::AudioOutput));
}

QList<QString> Genshin_TianLi_TeamVoice::get_device_list(QAudio::Mode inout)
{
    QList<QString> audio_device_name_vector;
    QList<QAudioDeviceInfo> audio_device_list = QAudioDeviceInfo::availableDevices(inout);

    for each (auto device_var in audio_device_list)
    {
        QString device_name = device_var.deviceName();
        if (device_var.isNull())continue;
        if (device_name[0] == 65533)continue;
        bool is_found = false;
        for each (auto var in audio_device_name_vector)
        {
            if (device_name == var)
            {
                is_found = true;
            }
        }
        if (is_found)continue;
        audio_device_name_vector.push_back(device_name);
    }
    return audio_device_name_vector;
}

void Genshin_TianLi_TeamVoice::combobox_currentIndexChanged_inputDevice(const QString& text, QAudio::Mode inout)
{
    qDebug() << "combobox_currentIndexChanged_inputDevice"<<"\n";

    QList<QAudioDeviceInfo> audio_device_list = QAudioDeviceInfo::availableDevices(inout);

    for each (auto device_var in audio_device_list)
    {
        if (text == device_var.deviceName())
        {
            switch (inout)
            {
            case QAudio::AudioInput:
            {
                //set_stop_input();
                audioDevice_input_info = device_var;
                return;
                break;
            }
            case QAudio::AudioOutput: 
            {
                //set_stop_output();
                audioDevice_output_info = device_var;
                return;
                break;
            }
            default:
                break;
            }
        }
    }
}

void Genshin_TianLi_TeamVoice::set_start_input()
{
    qDebug() << "set_start_input\n";

    audsend.setAudioInputDevice(audioDevice_input_info);
    audsend.setAudioInputFormat(8000, 1, 16);
    audsend.startInput();


}
void Genshin_TianLi_TeamVoice::set_stop_input()
{
    qDebug() << "set_stop_input\n";

}
void Genshin_TianLi_TeamVoice::set_start_output()
{
    qDebug() << "set_start_output\n";

    audsend.setAudioOutputDevice(audioDevice_output_info);
    audsend.setAudioOutputFormat(8000, 1, 16);
    audsend.setOutputVolumn(100);
    audsend.startOutput();
    //audsend.run();
}
void Genshin_TianLi_TeamVoice::set_stop_output()
{
    qDebug() << "set_stop_output\n";

}
void Genshin_TianLi_TeamVoice::set_input_value(int value)
{

}
void Genshin_TianLi_TeamVoice::set_output_value(int value)
{

}
