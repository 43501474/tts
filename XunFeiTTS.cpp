#include "XunFeiTTS.h"

#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#include <QApplication>
#include <QDebug>
#include <fstream>
#include <QThread>

using namespace std;

/* wav音频头部格式 */
typedef struct _wave_pcm_hdr
{
    char            riff[4];                // = "RIFF"
    int				size_8;                 // = FileSize - 8
    char            wave[4];                // = "WAVE"
    char            fmt[4];                 // = "fmt "
    int				fmt_size;				// = 下一个结构体的大小 : 16

    short int       format_tag;             // = PCM : 1
    short int       channels;               // = 通道数 : 1
    int				samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
    int				avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
    short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
    short int       bits_per_sample;        // = 量化比特数: 8 | 16

    char            data[4];                // = "data";
    int				data_size;              // = 纯数据长度 : FileSize - 44
} wave_pcm_hdr;

/* 默认wav音频头部数据 */
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
    const char* login_params         = "appid = 56a77ead, work_dir = .";//登录参数,appid与msc库绑定,请勿随意改动
    /* 用户登录 */
    ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://open.voicecloud.cn注册获取
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
    MSPLogout(); //退出登录
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
    out.write(reinterpret_cast<const char*>(&wav_hdr), sizeof(wav_hdr)); //添加wav音频头，使用采样率为16000

    /* 文本合成 */
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
        /* 获取合成音频 */
        const void* data = QTTSAudioGet(m_sessionID, &audio_len, &synth_status, &ret);
        if (MSP_SUCCESS != ret)
            break;
        if (NULL != data)
        {
            out.write(reinterpret_cast<const char*>(data), audio_len);
            wav_hdr.data_size += audio_len; //计算data_size大小
        }
        if (MSP_TTS_FLAG_DATA_END == synth_status)
            break;
        QThread::msleep(150); //防止频繁占用CPU
    }//合成状态synth_status取值请参阅《讯飞语音云API文档》

    if (MSP_SUCCESS != ret)
    {
        qDebug() << "QTTSAudioGet failed";
        return -1;
    }
    /* 修正wav文件头数据的大小 */
    wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

    /* 将修正过的数据写回文件头部,音频文件为wav格式 */
    out.seekp(4);
    out.write(reinterpret_cast<const char*>(&wav_hdr.size_8),sizeof(wav_hdr.size_8)); //写入size_8的值
    out.seekp(40); //将文件指针偏移到存储data_size值的位置
    out.write(reinterpret_cast<const char*>(&wav_hdr.data_size),sizeof(wav_hdr.data_size)); //写入data_size的值

    return 0;
}

