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
    /// ������Ƶ�����ʽ
    /// </summary>
    void set_input_audio_format();

    /// <summary>
    /// ������Ƶ��������豸
    /// </summary>
    void get_inout_audio_device();

    /// <summary>
    /// ��ȡ��Ƶ�豸�б�
    /// </summary>
    /// <param name="inout">��Ƶ���������Ƶ���</param>
    /// <returns>�豸�����б�</returns>
    QList<QString> get_device_list(QAudio::Mode inout);
private:
    void sendMessage(QString message);

private slots:

    void combobox_currentIndexChanged_inputDevice(const QString& text, QAudio::Mode inout);

    // ��ʼ����
    void pushbutton_startConnect();
    // ֹͣ����
    void pushbutton_stopConnect();
    // �������
    void pushbutton_joinGroup();
    // �˳�����
    void pushbutton_exitGroup();
    // ���Ͷ�����Ϣ
    void pushbutton_sendGroupMessage();
    // ����˽����Ϣ
    void pushbutton_sendUidMessage();
    // ��ʼʰ��
    void set_start_input();
    // ֹͣʰ��
    void set_stop_input();
    // ��ʼ����
    void set_start_output();
    // ֹͣ����
    void set_stop_output();

    void set_input_value(int value);
    void set_output_value(int value);

    void set_input_db(int dbValue);
    void set_output_db(int dbValue);

    // ���������ַ���
    void recviceSocketMessage(QString message);
};
