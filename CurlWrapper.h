#pragma once
#include "curl_low_level.h"

namespace Curl
{
    class CurlWrapper
    {
        CURL* curl = nullptr;
        std::string content = "";
        std::wstring cipher = L"";
        std::string issuer = "";
        std::string subject = "";

        void GetCertificateInfo();
    public:
        CurlWrapper();
        ~CurlWrapper();

        void DisplaySecurePage(const std::string& url);
        void DisplayPageOverIpv6(const std::string& url);
    };
}