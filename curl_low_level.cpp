#include "curl_low_level.h"

namespace Curl_C
{
    CURL* curl_c = nullptr;
    std::wstring cipher = L"";

    size_t write_callback(void* data, size_t size, size_t nmemb, void* userp)
    {
        // here the content appends data read
        std::string* content = (std::string*)userp;
        content->append((char*)data);

        // here take the cipher used from the underlying SSL lib
        if (curl_c != nullptr && cipher.length() == 0) // do this only once
        {
            const struct curl_tlssessioninfo* info = NULL;
            CURLcode res = curl_easy_getinfo(curl_c, CURLINFO_TLS_SSL_PTR, &info);
            if (res == CURLE_OK)
            {
                if (info->backend == CURLSSLBACKEND_SCHANNEL)
                {
                    SecPkgContext_CipherInfo cipherInfo;
                    SecHandle* handle = (SecHandle*)(info->internals);
                    if (handle != nullptr)
                    {
                        SECURITY_STATUS status = QueryContextAttributes(handle, SECPKG_ATTR_CIPHER_INFO, (PVOID)&cipherInfo);
                        if (status == SEC_E_OK)
                        {
                            cipher = cipherInfo.szCipherSuite;
                        }
                    }
                }
            }
        }

        return size * nmemb;
    }
}