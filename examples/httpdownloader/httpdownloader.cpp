// httpdownloader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "downloader.h"

int main()
{
    HttpDownloader downloader(L"https://www.baidu.com?q=1");
    if (downloader.Request()) {
        printf("%s", downloader.ResponseData().c_str());
    }

    return 0;
}

