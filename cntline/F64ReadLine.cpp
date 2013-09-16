// F64ReadLine.cpp

#include "stdafx.h"
#include "F64ReadLine.h"


F64ReadLine::~F64ReadLine(void)
{
    if(ringbuff) delete [] ringbuff;
    if(hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
}


F64ReadLine::F64ReadLine()
    : is_good(true)
    , is_eof(false)
    , hFile(INVALID_HANDLE_VALUE)
    , ringbuff(0)
    , ringsz(0x800000)  // 环形圆圈大小为 8 MiB
    , ringidx(0)
    , blocksz(0)
    , codepage(0)
    , eof_flag(false)
{
    ringbuff = new char[MEMPROTECT(ringsz)];
}

void F64ReadLine::open(const wchar_t* fn)
{
    hFile = CreateFileW(fn, GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if(hFile == INVALID_HANDLE_VALUE) {
        is_good = false;
        return;
    }
    GetFileSizeEx(hFile, &filesz);
    blocksz = filesz.QuadPart > ringsz ? ringsz : filesz.QuadPart;
}

// Global in Class
// int blocksz;
// int ringidx;
// const int ringsz;
// char* ringbuff;
// bool eof_flag;
// bool eof;

long long F64ReadLine::cntline()
    // 将这个环形数组的每一圈看作一个有输入输出的容器
{
    if(eof()) return 0;

    long long cnt = 0;

    while(1) {
        if(!(ringidx = ringidx % ringsz)) {
            ReadFile(hFile, ringbuff, ringsz, (LPDWORD)&blocksz, NULL);
            if(blocksz < ringsz) {
                eof_flag = true; // 数组的输入端达到eof
            }
        }
        wchar_t* wbuff = (wchar_t*)(ringbuff + ringidx);
        int widx = ringidx / sizeof(wchar_t);
        int wbsz = blocksz / sizeof(wchar_t);
        switch (codepage)
        {
        case 1200:
            while(widx < wbsz && ringidx < blocksz) {
                if(wbuff[widx++] == L'\x000A') ++cnt;
                ringidx += sizeof(wchar_t);
            }
            break;
        case 1201:
            while(widx < wbsz && ringidx < blocksz) {
                if(wbuff[widx++] == L'\x0A00') ++cnt;
                ringidx += sizeof(wchar_t);
            }
            break;
        default:
            while(ringidx < blocksz) {
                if(ringbuff[ringidx++] == '\n') ++cnt;
            }
            break;
        }
        if(eof_flag && ringidx == blocksz) {
            is_eof = true; // 数组的输出端, 即本身达到了eof
            break;
        }
    }
    return cnt; 
}

void F64ReadLine::setpointer(const int i)
{
        LARGE_INTEGER offset = {0};
        offset.LowPart = i;
        SetFilePointerEx(hFile, offset, NULL, FILE_BEGIN);
}

bool F64ReadLine::isutf8(const char* str, const int len)
    // 代码来源 : http://blog.csdn.net/lufubo/article/details/8232234
{
    int i = 0;       
    const int size = len;
    while(i < size) {         
        int step = 0;         
        if((str[i] & 0x80) == 0x00) {
            step = 1;         
        }else if((str[i] & 0xe0) == 0xc0) {             
            if(i + 1 >= size) 
                return false;             
            if((str[i + 1] & 0xc0) != 0x80) 
                return false;               
            step = 2;         
        }else if((str[i] & 0xf0) == 0xe0) {             
            if(i + 2 >= size) 
                return false;             
            if((str[i + 1] & 0xc0) != 0x80) 
                return false;             
            if((str[i + 2] & 0xc0) != 0x80) 
                return false;               
            step = 3;        
        }else {             
            return false;         
        }          
        i += step;    
    }       
    if(i >= size - 7) 
        return true;      
    return false; 
}

void F64ReadLine::TestEnc(void)
{
    char* buf = new char[MEMPROTECT(32768)];
    int size = 0;
    setpointer(0);
    ReadFile(hFile, buf, 32768, (LPDWORD)&size, NULL);
    do {
        if(size > 2) { // 逆天等级的 bug 发现! 非 unsigned 情况下, 0xFF 和 '\xFF' 不同!
            if(buf[0] == '\xEF' && buf[1] == '\xBB' && buf[2] == '\xBF') {
                (codepage = 65001);
                break;
            }
        }
        if(size > 1) {
            if(buf[0] == '\xFF' && buf[1] == '\xFE') {
                (codepage = 1200);
                break;
            }
            if(buf[0] == '\xFE' && buf[1] == '\xFF') {
                (codepage = 1201);
                break;
            }
            if(TRUE == IsTextUnicode(buf, size, NULL)) {
                (codepage = 1200);
                break;
            }
        }
        if(isutf8(buf, size)) {
            (codepage = 65001);
            break;
        }
        (codepage = GetACP());
        break;
    } while(0);
    setpointer(0);
    delete [] buf;
}


void F64ReadLine::close(void)
{
    if(hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
}
