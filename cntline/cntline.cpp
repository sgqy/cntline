// cntline.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


int wmain(int argc, wchar_t* argv[])
{
    _setmode(_fileno(stdout), _O_U16TEXT);
    cnt_struct cnt;
    for(int i = 1; i < argc; ++i) {
        cnt_it(argv[i], cnt);
    }
    fwprintf_s(stdout, L"Line|%14lld|Size|%14lld|   Total|\n", cnt.lines, cnt.fsiz);
	return 0;
}
