#include "XunFeiTTS.h"

#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#include <QApplication>
#include <QDebug>
#include <fstream>
#include <QThread>

using namespace std;

/* wav��Ƶͷ����ʽ */
typedef struct _wave_pcm_hdr
{
    char            riff[4];                // = "RIFF"
    int				size_8;                 // = FileSize - 8
    char            wave[4];                // = "WAVE"
    char            fmt[4];                 // = "fmt "
    int				fmt_size;				// = ��һ���ṹ��Ĵ�С : 16

    short int       format_tag;             // = PCM : 1
    short int       channels;               // = ͨ���� : 1
    int				samples_per_sec;        // = ������ : 8000 | 6000 | 11025 | 16000
    int				avg_bytes_per_sec;      // = ÿ���ֽ��� : samples_per_sec * bits_per_sample / 8
    short int       block_align;            // = ÿ�������ֽ��� : wBitsPerSample / 8
    short int       bits_per_sample;        // = ����������: 8 | 16

    char            data[4];                // = "data";
    int				data_size;              // = �����ݳ��� : FileSize - 44
} wave_pcm_hdr;

/* Ĭ��wav��Ƶͷ������ */
wave_pcm_hdr default_wav_hdr =
{
    { 'R', 'I', 'F', 'F' },
    0,
    {'W', 'A', 'V', 'E'},
    {'f', 'm', 't', ' '},
    16,
    1,
    1,
    16000,
    32000,
    2,
    16,
    {'d', 'a', 't', 'a'},
    0
};


XunFeiTTS::XunFeiTTS()
    :m_bInit(false)
    ,m_sessionID(nullptr)
{
    int         ret                  = MSP_SUCCESS;
    const char* login_params         = "appid = 56a77ead, work_dir = .";//��¼����,appid��msc���,��������Ķ�
    /* �û���¼ */
    ret = MSPLogin(NULL, NULL, login_params); //��һ���������û������ڶ������������룬�����������ǵ�¼�������û������������http://open.voicecloud.cnע���ȡ
    if (MSP_SUCCESS != ret)
    {
        qDebug() << "MSPLogin failed";
        m_bInit = false;
    }

    const char* session_begin_params = "voice_name = xiaoyan, text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
    m_sessionID = QTTSSessionBegin(session_begin_params, &ret);
    if (MSP_SUCCESS != ret)
    {
        qDebug() << "QTTSSessionBegin failed";
        m_bInit = false;
    }

    m_bInit = true;
}

XunFeiTTS::~XunFeiTTS()
{
    QTTSSessionEnd(m_sessionID, "Normal");
    MSPLogout(); //�˳���¼
}

int XunFeiTTS::tts(const char* src_text, const char* des_path)
{
    if (false == m_bInit)
    {
        qDebug() << "TTS init failed";
        return -1;
    }

    wave_pcm_hdr wav_hdr      = default_wav_hdr;
    ofstream out(des_path, ofstream::out|ofstream::binary);
    if (!out)
    {
        qDebug() << "Open file failed: " << des_path;
        return -1;
    }
    out.write(reinterpret_cast<const char*>(&wav_hdr), sizeof(wav_hdr)); //���wav��Ƶͷ��ʹ�ò�����Ϊ16000

    /* �ı��ϳ� */
    int ret = QTTSTextPut(m_sessionID, src_text, (unsigned int)strlen(src_text), NULL);
    if (MSP_SUCCESS != ret)
    {
        qDebug() << "QTTSTextPut failed, error code: " << ret;
        return -1;
    }

    int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
    unsigned int audio_len;
    while (true)
    {
        /* ��ȡ�ϳ���Ƶ */
        const void* data = QTTSAudioGet(m_sessionID, &audio_len, &synth_status, &ret);
        if (MSP_SUCCESS != ret)
            break;
        if (NULL != data)
        {
            out.write(reinterpret_cast<const char*>(data), audio_len);
            wav_hdr.data_size += audio_len; //����data_size��С
        }
        if (MSP_TTS_FLAG_DATA_END == synth_status)
            break;
        QThread::msleep(150); //��ֹƵ��ռ��CPU
    }//�ϳ�״̬synth_statusȡֵ����ġ�Ѷ��������API�ĵ���

    if (MSP_SUCCESS != ret)
    {
        qDebug() << "QTTSAudioGet failed";
        return -1;
    }
    /* ����wav�ļ�ͷ���ݵĴ�С */
    wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

    /* ��������������д���ļ�ͷ��,��Ƶ�ļ�Ϊwav��ʽ */
    out.seekp(4);
    out.write(reinterpret_cast<const char*>(&wav_hdr.size_8),sizeof(wav_hdr.size_8)); //д��size_8��ֵ
    out.seekp(40); //���ļ�ָ��ƫ�Ƶ��洢data_sizeֵ��λ��
    out.write(reinterpret_cast<const char*>(&wav_hdr.data_size),sizeof(wav_hdr.data_size)); //д��data_size��ֵ

    return 0;
}

