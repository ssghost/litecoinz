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

void LTZ_VerifyParams(std::string file, std::string sha256expected)
{
    unsigned char buffer[BUFSIZ];
    FILE *fp;
    SHA256_CTX ctx;
    size_t len;

    std::string msg = "Verifying " + file + "...";
    LogPrintf(msg.c_str());

    std::size_t pos = file.find("sprout");
    std::string initMsg = "Verifying " + file.substr(pos) + "...";
    uiInterface.InitMessage(_(initMsg.c_str()));

    fp = fopen(file.c_str(), "r");
    if(!fp) {
        msg = "Can not open " + file + "!";
        LogPrintf(msg.c_str());
    }

    SHA256_Init(&ctx);
    do {
        len = fread(buffer, 1, BUFSIZ, fp);
        SHA256_Update(&ctx, buffer, len);
    } while (len == BUFSIZ);

    SHA256_Final(buffer, &ctx);

    fclose(fp);

    std::ostringstream oss;

    for (len = 0; len < SHA256_DIGEST_LENGTH; ++len)
        oss << strprintf("%02x", buffer[len]);
    oss << std::endl;

    LogPrintf("SHA256SUM: %s", oss.str());

    std::string sha256file = oss.str();

    if(!sha256expected.compare(sha256file))
    {
        msg = "Deleting corrupted file " + file + "!";
        LogPrintf(msg.c_str());
        initMsg = "Deleting corrupted file " + file.substr(pos) + "!";
        uiInterface.InitMessage(_(initMsg.c_str()));
        boost::filesystem::remove(file.c_str());
    }
}

void LTZ_FetchParams(std::string url, std::string file)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    bool ret = true;

    std::string msg = "Downloading " + file + "...";
    LogPrintf(msg.c_str());

    std::size_t pos = file.find("sprout");
    std::string initMsg = "Downloading " + file.substr(pos) + "...";

    uiInterface.InitMessage(_(initMsg.c_str()));

    curl = curl_easy_init();
    if(curl)
    {
        fp = fopen(file.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }

    std::ostringstream oss;
    oss << "CURL Return code: " << curl_easy_strerror(res) << std::endl;
    LogPrintf("%s", oss.str());
}
