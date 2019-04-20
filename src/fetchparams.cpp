// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifdef WIN32
#define CURL_STATICLIB
#endif

#include <fetchparams.h>
#include <fs.h>
#include <ui_interface.h>
#include <util.h>

#include <stdio.h>
#include <curl/curl.h>
#include <openssl/sha.h>

#include <sstream>
#include <string>

std::string filename = "";

void printInfo(std::string msg)
{
    std::string info = msg + " " + filename + "...";
    LogPrintf("%s\n", info.c_str());
    uiInterface.InitMessage(_(info.c_str()));
}

bool LTZ_VerifyParams(std::string file, std::string sha256expected)
{
    printInfo("Verifying");

    FILE *fp = fopen(file.c_str(), "rb");
    if(!fp) {
        std::string msg = "Can not open " + file + "!";
        LogPrintf("%s\n", msg.c_str());
    }

    unsigned char buffer[BUFSIZ];
    unsigned char hash[SHA256_DIGEST_LENGTH];

    int len = 0;
    int bytesRead = 0;

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
        printInfo("Deleting corrupted file");
        boost::filesystem::remove(file.c_str());
        return false;
    }

    return true;
}

static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    int perc = 0;
    if ((double)dlnow > 0)
        perc = (double)dlnow * 100 / (double)dltotal;

    std::string initMsg = "Downloading " + filename + " (" + std::to_string(perc) + "%)...";
    uiInterface.InitMessage(_(initMsg.c_str()));

    return 0;
}

bool LTZ_FetchParams(std::string url, std::string file)
{
    CURL *curl;
    CURLcode res = CURLE_OK;

    FILE *fp;

    std::string msg = "Downloading " + url + "...";
    LogPrintf("%s\n", msg.c_str());

    boost::filesystem::path p(file);
    filename = p.filename().string();

    curl = curl_easy_init();
    if (curl)
    {
        fp = fopen(file.c_str(), "wb");
        if (fp)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nullptr);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

            res = curl_easy_perform(curl);
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
