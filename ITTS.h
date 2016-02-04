#pragma once

class ITTS
{
public:
    virtual int tts(const char* src_text, const char* des_path) = 0;
};

