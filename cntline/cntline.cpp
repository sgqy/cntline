// cntline.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


int wmain(int argc, wchar_t* argv[])
{
    _setmode(_fileno(stdout), _O_U16TEXT);
    cnt_struct cnt;
    int succ_cnt = 0;
    for(int i = 1; i < argc; ++i) {
        cnt_it(argv[i], cnt, succ_cnt);
    }
    fwprintf_s(stdout, L"Line|%14lld|Size|%14lld|   Total|%d\n", cnt.lines, cnt.fsiz, succ_cnt);
	return 0;
}
