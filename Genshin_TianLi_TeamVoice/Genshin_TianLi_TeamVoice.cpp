#include "Genshin_TianLi_TeamVoice.h"

#include <QJsonObject>
#include <QJsonDocument>
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
    // 开始连接
    connect(ui.pushButton_start, &QPushButton::clicked, this, &Genshin_TianLi_TeamVoice::pushbutton_startConnect);

    // 停止连接
    connect(ui.pushButton_stop, &QPushButton::clicked, this, &Genshin_TianLi_TeamVoice::pushbutton_stopConnect);
    // 加入队伍
    connect(ui.pushButton_joinGroup, &QPushButton::clicked, this, &Genshin_TianLi_TeamVoice::pushbutton_joinGroup);
    // 退出队伍
    connect(ui.pushButton_exitGroup, &QPushButton::clicked, this, &Genshin_TianLi_TeamVoice::pushbutton_exitGroup);
    // 发送队伍消息
    connect(ui.pushButton_sendGroupMge, &QPushButton::clicked, this, &Genshin_TianLi_TeamVoice::pushbutton_sendGroupMessage);
    // 发送私聊消息
    connect(ui.pushButton_sendUIDMge, &QPushButton::clicked, this, &Genshin_TianLi_TeamVoice::pushbutton_sendUidMessage);


    // 开始拾音
    connect(ui.pushButton_getInput, &QPushButton::clicked, this, &Genshin_TianLi_TeamVoice::set_start_input);
    // 开始播音
    connect(ui.pushButton_setOutput, &QPushButton::clicked, this, &Genshin_TianLi_TeamVoice::set_start_output);

    // 分贝数显示
    connect(&audioSocketManagment, &AudioSocketManagment::dbInputChange, this, &Genshin_TianLi_TeamVoice::set_input_db);
    connect(&audioSocketManagment, &AudioSocketManagment::dbInputChange, this, &Genshin_TianLi_TeamVoice::set_input_db);

    // 解析接收字符串
    connect(&audioSocketManagment, &AudioSocketManagment::recviceTextMessage, this, &Genshin_TianLi_TeamVoice::recviceSocketMessage);

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

    for (auto device_var : audio_device_list)
    {
        QString device_name = device_var.deviceName();
        if (device_var.isNull())continue;
        if (device_name[0] == 65533)continue;
        bool is_found = false;
        for (auto var : audio_device_name_vector)
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

void Genshin_TianLi_TeamVoice::sendMessage(QString message)
{
    audioSocketManagment.sendTextMessage(message);
}

void Genshin_TianLi_TeamVoice::combobox_currentIndexChanged_inputDevice(const QString& text, QAudio::Mode inout)
{
    qDebug() << "combobox_currentIndexChanged_inputDevice"<<"\n";

    QList<QAudioDeviceInfo> audio_device_list = QAudioDeviceInfo::availableDevices(inout);

    for (auto device_var : audio_device_list)
    {
        if (text == device_var.deviceName())
        {
            switch (inout)
            {
            case QAudio::AudioInput:
            {
                //pushbutton_stopInput();
                audioDevice_input_info = device_var;
                return;
                break;
            }
            case QAudio::AudioOutput: 
            {
                //pushbutton_stopOutput();
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

void Genshin_TianLi_TeamVoice::pushbutton_startConnect()
{
    qDebug() << "开始连接";
    QString url = "";
    if (ui.lineEdit_ip_port->text()=="")
    {
        url = ui.lineEdit_ip->text();
        qDebug() << "连接url:"<<url;
        audioSocketManagment.startSendRecviceSocket(url);
    }
    else
    {
        if (ui.lineEdit_UID->text() == "")
        {
            url = ui.lineEdit_ip->text() + ":" + ui.lineEdit_ip_port->text();
            qDebug() << "连接url:" << url;
            audioSocketManagment.startSendRecviceSocket(url);
        }
        else
        {
            url = ui.lineEdit_ip->text() + ":" + ui.lineEdit_ip_port->text()+"?id="+ ui.lineEdit_UID->text();
            qDebug() << "连接url:" << url;
            audioSocketManagment.startSendRecviceSocket(url);
        }
    }
    ui.textEdit->append(url);
}

void Genshin_TianLi_TeamVoice::pushbutton_stopConnect()
{
    audioSocketManagment.stopSendRecviceSocket();
}

void Genshin_TianLi_TeamVoice::pushbutton_joinGroup()
{
    // 加入队伍

    QString id_Group = ui.lineEdit_toGroupUID->text();

    QJsonDocument jdc;
    QJsonObject job1;
    QJsonObject job2;
    job1.insert("connectTo",id_Group);
    job2.insert("control", job1);
    jdc.setObject(job2);

    QString message = jdc.toJson();
    sendMessage(message);
}

void Genshin_TianLi_TeamVoice::pushbutton_exitGroup()
{
    // 退出队伍
    QString id_Group = ui.lineEdit_toGroupUID->text();

    QJsonDocument jdc;
    QJsonObject job1;
    QJsonObject job2;
    job1.insert("connectTo", id_Group);
    job1.insert("exitRoom", 1);
    job2.insert("control", job1);
    jdc.setObject(job2);

    QString message = jdc.toJson();
    sendMessage(message);
}

void Genshin_TianLi_TeamVoice::pushbutton_sendGroupMessage()
{
    // 发送队伍消息
    QString msg = ui.lineEdit->text();

    QJsonDocument jdc;
    QJsonObject job;
    job.insert("message2room", msg);
    jdc.setObject(job);

    QString message = jdc.toJson();
    sendMessage(message);
}

void Genshin_TianLi_TeamVoice::pushbutton_sendUidMessage()
{
    // 发送私聊消息
    
    QString msg = ui.lineEdit_2->text();
    QString id = ui.lineEdit_toUID->text();

    QJsonDocument jdc;
    QJsonObject job;
    QJsonObject job1;
    job1.insert("toId", id);
    job1.insert("message", msg);
    job.insert("message2id", job1);
    job.insert("sender", id);
    jdc.setObject(job);
    
    QString message = jdc.toJson();
    sendMessage(message);
}


void Genshin_TianLi_TeamVoice::pushbutton_startInput()
{
    qDebug() << "pushbutton_startInput " << audioDevice_input_info.deviceName();

    audioSocketManagment.setAudioInputDevice(audioDevice_input_info);
    audioSocketManagment.setAudioInputFormat(8000, 1, 16);
    audioSocketManagment.startInput();
}
void Genshin_TianLi_TeamVoice::pushbutton_stopInput()
{
    qDebug() << "pushbutton_stopInput\n";
    audioSocketManagment.stopInput();
    set_input_db(0);
}
void Genshin_TianLi_TeamVoice::pushbutton_startOutput()
{
    qDebug() << "pushbutton_startOutput\n";

    audioSocketManagment.setAudioOutputDevice(audioDevice_output_info);
    audioSocketManagment.setAudioOutputFormat(8000, 1, 16);
    audioSocketManagment.setOutputVolumn(100);
    audioSocketManagment.startOutput();
    //audsend.run();
}
void Genshin_TianLi_TeamVoice::pushbutton_stopOutput()
{
    qDebug() << "pushbutton_stopOutput\n";
    audioSocketManagment.stopOutput();
    set_output_db(0);
}
void Genshin_TianLi_TeamVoice::set_input_value(int value)
{
    audioSocketManagment.setInputVolumn(value);
}
void Genshin_TianLi_TeamVoice::set_output_value(int value)
{
    audioSocketManagment.setOutputVolumn(value);
}

void Genshin_TianLi_TeamVoice::set_input_db(int dbValue)
{
    ui.progressBar_input->setValue(dbValue);
}
void Genshin_TianLi_TeamVoice::set_output_db(int dbValue)
{
    ui.progressBar_output->setValue(dbValue);
}

void Genshin_TianLi_TeamVoice::recviceSocketMessage(QString message)
{
    qDebug().noquote() << message;
    QJsonParseError err_rpt;
    QJsonDocument  Doc = QJsonDocument::fromJson(message.toStdString().c_str(), &err_rpt);//字符串格式化为JSON
    if (err_rpt.error != QJsonParseError::NoError)
    {
        qDebug() << tr("tr_JSOR_TypeError");
        return;
    }
    else {
        QString type = Doc.object().find("type")->toString();
        QString msg = Doc.object().find("message")->toString();
        QString type_out = tr("tr_UnDefind:");
        qDebug() << type<<" : " << msg;

        if (type == "user")
        {
            type_out = tr("tr_ShiLiao:");
        }
        if (type == "room")
        {
            type_out =tr("tr_FangJian:");
        }

        ui.textEdit->append(type_out + msg);
    }


}
