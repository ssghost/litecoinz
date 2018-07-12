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
#include <openssl/sha.h>

#include <sstream>
#include <string>

#include <boost/filesystem.hpp>

bool LTZ_VerifyParams(std::string file, std::string sha256expected)
{
    std::string msg = "Verifying " + file + "...";
    LogPrintf("%s\n", msg.c_str());

    FILE *fp = fopen(file.c_str(), "rb");
    if(!fp) {
        msg = "Can not open " + file + "!";
        LogPrintf("%s\n", msg.c_str());
    }

    unsigned char buffer[BUFSIZ];
    unsigned char hash[SHA256_DIGEST_LENGTH];

    int len = 0;
    int bytesRead = 0;

    int pos = file.find("sprout");
    std::string initMsg = "Verifying " + file.substr(pos) + "...";
    uiInterface.InitMessage(_(initMsg.c_str()));

    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    while((bytesRead = fread(buffer, 1, BUFSIZ, fp)))
    {
        SHA256_Update(&ctx, buffer, bytesRead);
    }
    SHA256_Final(hash, &ctx);

    fclose(fp);

    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        oss << strprintf("%02x", hash[i]);

    LogPrintf("SHA256SUM: %s\n", oss.str());

    if (!(sha256expected.compare(oss.str()) == 0))
    {
        msg = "Deleting corrupted file " + file + "!";
        LogPrintf("%s\n", msg.c_str());
        initMsg = "Deleting corrupted file " + file.substr(pos) + "!";
        uiInterface.InitMessage(_(initMsg.c_str()));
        boost::filesystem::remove(file.c_str());
        return false;
    }

    return true;
}

bool LTZ_FetchParams(std::string url, std::string file)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;

    std::string msg = "Downloading " + url + "...";
    LogPrintf("%s\n", msg.c_str());

    int pos = file.find("sprout");
    std::string initMsg = "Downloading " + file.substr(pos) + "...";

    uiInterface.InitMessage(_(initMsg.c_str()));

    curl = curl_easy_init();
    if (curl)
    {
        fp = fopen(file.c_str(), "wb");
        if (fp)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

            CURLcode res = curl_easy_perform(curl);
            fclose(fp);

            std::ostringstream oss;
            oss << "CURL Return code: " << curl_easy_strerror(res) << std::endl;
            LogPrintf("%s", oss.str());

            if (res != CURLE_OK)
                return false;
        }
    }
    curl_easy_cleanup(curl);

    return true;
}
