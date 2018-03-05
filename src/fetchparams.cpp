// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifdef WIN32
#define CURL_STATICLIB
#endif

#include "fetchparams.h"
#include "ui_interface.h"
#include "util.h"

#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include <string>

size_t LTZ_write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

bool LTZ_FetchParams(std::string url, std::string file)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;

    std::string msg = "Downloading " + file + " file...";
    LogPrintf(msg.c_str());
    uiInterface.InitMessage(_(msg.c_str()));

    curl = curl_easy_init();
    if(curl)
    {
        fp = fopen(file.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, LTZ_write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);

        if(res != CURLE_OK)
            return false;
    }

    return true;
}

