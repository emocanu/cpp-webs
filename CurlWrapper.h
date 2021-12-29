#pragma once
#define SECURITY_WIN32

#include <iostream>
#include <string>
#include "curl/curl.h"
#include "schannel.h"
#include "Sspi.h"

namespace Curl
{
    class CurlWrapper
    {
        CURL* curl = nullptr;
        std::string content = "";
        std::wstring cipher = L"";
        std::string issuer = "";
        std::string subject = "";

        //size_t __cdecl write_callback(void* data, size_t size, size_t nmemb, void* userp);
        void GetCertificateInfo();
    public:
        CurlWrapper();
        ~CurlWrapper();

        void DisplaySecurePage(const std::string& url);
    };
}