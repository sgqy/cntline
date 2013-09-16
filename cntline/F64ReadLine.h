// F64ReadLine.h : 用于按行读取文件

#pragma once
class F64ReadLine
{
public:
    ~F64ReadLine(void);
protected:
    bool is_good;
    bool is_eof;
    HANDLE hFile;
    char* ringbuff;
    int ringsz;
    int ringidx;
    int blocksz;
    int codepage;
    LARGE_INTEGER filesz;
    bool eof_flag;
    void setpointer(const int i);
    bool isutf8(const char* p, const int len);
public:
    F64ReadLine();
    void open(const wchar_t* fn);
    long long cntline();
    bool eof(void) {
        return is_eof;
    }
    bool good(void) {
        return is_good;
    }
    void TestEnc(void);
    void close(void);
    int getCodePage() {
        return codepage;
    }
    long long getFsiz() {
        return filesz.QuadPart;
    }
};

