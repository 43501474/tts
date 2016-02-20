#pragma once
#include "ITTS.h"

#include <fstream>

class XunFeiTTS: public ITTS
{
public:
    XunFeiTTS(const char* des_path);
    ~XunFeiTTS();

    virtual int tts(const char* src_text) override;

private:
    bool m_bInit;
    const char* m_sessionID;
	std::ofstream* m_pOut;
};


