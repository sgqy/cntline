
#include "stdafx.h"


wchar_t* tp_ansi = L"    ANSI";
wchar_t* tp_UTF8 = L"   UTF-8";
wchar_t* tp_U16L = L"UTF-16LE";
wchar_t* tp_U16B = L"UTF-16BE";

void cnt_it(const wchar_t* fn, cnt_struct& cnt)
{
    try {
    cnt_struct single;
    single.lines = 1;
    wchar_t* type = NULL;

    F64ReadLine frd;
    frd.open(fn);
    if(!frd.good()) {
        throw 0;
    }
    frd.TestEnc();

    switch (frd.getCodePage())
    {
    case 1200:
        type = tp_U16L;
        break;
    case 1201:
        type = tp_U16B;
        break;
    case 65001:
        type = tp_UTF8;
        break;
    default:
        type = tp_ansi;
        break;
    }

    single.fsiz += frd.getFsiz();

    fwprintf_s(stdout, L"Line|-calculating--|Size|%14lld|%s|%s", single.fsiz, type, fn);

    while(1) {
        single.lines += frd.cntline();
        if(frd.eof()) break;
    }
    
    fwprintf_s(stdout, L"\rLine|%14lld\n", single.lines);
    cnt += single;
    } catch (...) {
        fwprintf_s(stdout, L"Line|-             |Size|-             |   ERROR|%s\n", fn);
        return;
    }
}
