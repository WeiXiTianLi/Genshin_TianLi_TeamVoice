#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Genshin_TianLi_TeamVoice.h"

#include <QAudio>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>

#include "AudioSocketManagment.h"

class Genshin_TianLi_TeamVoice : public QMainWindow
{
    Q_OBJECT

public:
    Genshin_TianLi_TeamVoice(QWidget *parent = Q_NULLPTR);

private:
    Ui::Genshin_TianLi_TeamVoiceClass ui;

private:
    QAudioFormat in_audio_format;
    QAudioDeviceInfo audioDevice_input_info = QAudioDeviceInfo::defaultInputDevice();
    QAudioDeviceInfo audioDevice_output_info = QAudioDeviceInfo::defaultOutputDevice();

    QAudioInput* input_audio_device = nullptr;
    QAudioOutput* output_audio_device = nullptr;


    AudioSocketManagment audioSocketManagment;
private:
    /// <summary>
    /// 设置音频输入格式
    /// </summary>
    void set_input_audio_format();

    /// <summary>
    /// 设置音频输入输出设备
    /// </summary>
    void get_inout_audio_device();

    /// <summary>
    /// 获取音频设备列表
    /// </summary>
    /// <param name="inout">音频输入或者音频输出</param>
    /// <returns>设备名称列表</returns>
    QList<QString> get_device_list(QAudio::Mode inout);
private:
    void sendMessage(QString message);

private slots:

    void combobox_currentIndexChanged_inputDevice(const QString& text, QAudio::Mode inout);

    // 开始连接
    void pushbutton_startConnect();
    // 停止连接
    void pushbutton_stopConnect();
    // 加入队伍
    void pushbutton_joinGroup();
    // 退出队伍
    void pushbutton_exitGroup();
    // 发送队伍消息
    void pushbutton_sendGroupMessage();
    // 发送私聊消息
    void pushbutton_sendUidMessage();
    // 开始拾音
    void set_start_input();
    // 停止拾音
    void set_stop_input();
    // 开始播音
    void set_start_output();
    // 停止播音
    void set_stop_output();

    void set_input_value(int value);
    void set_output_value(int value);

    void set_input_db(int dbValue);
    void set_output_db(int dbValue);

    // 解析接收字符串
    void recviceSocketMessage(QString message);
};
