#pragma once
#include "ITTS.h"

class XunFeiTTS: public ITTS
{
public:
    XunFeiTTS();
    ~XunFeiTTS();

    virtual int tts(const char* src_text, const char* des_path) override;

private:
    bool m_bInit;
    const char* m_sessionID;
};


