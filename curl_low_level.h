#pragma once

#define SECURITY_WIN32

#include <iostream>
#include <string>
#include "curl/curl.h"
#include "schannel.h"
#include "Sspi.h"

namespace Curl_C
{
    extern CURL* curl_c;
    extern std::wstring cipher;
    size_t write_callback(void* data, size_t size, size_t nmemb, void* userp);
}