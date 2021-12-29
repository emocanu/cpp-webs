#include "CurlWrapper.h"

namespace Curl
{
    CurlWrapper::CurlWrapper()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
    }

    CurlWrapper::~CurlWrapper()
    {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    //size_t CurlWrapper::write_callback(void* data, size_t size, size_t nmemb, void* userp)
    static size_t write_callback(void* data, size_t size, size_t nmemb, void* userp)
    {
        std::string* content = (std::string*)userp;
        //chunk->assign((char*)data);
        content->append((char*)data);

        //if (cipher.length() == 0) // do this only once
        //{
        //    const struct curl_tlssessioninfo* info = NULL;
        //    CURLcode res = curl_easy_getinfo(curl, CURLINFO_TLS_SSL_PTR, &info);
        //    if (res == CURLE_OK)
        //    {
        //        if (info->backend == CURLSSLBACKEND_SCHANNEL)
        //        {
        //            SecPkgContext_CipherInfo cipherInfo;
        //            SecHandle* handle = (SecHandle*)(info->internals);
        //            if (handle != nullptr)
        //            {
        //                SECURITY_STATUS status = QueryContextAttributes(handle, SECPKG_ATTR_CIPHER_INFO, (PVOID)&cipherInfo);
        //                if (status == SEC_E_OK)
        //                {
        //                    cipher = cipherInfo.szCipherSuite;
        //                }
        //            }
        //        }
        //    }
        //}

        return size * nmemb;
    }

    void CurlWrapper::GetCertificateInfo()
    {
        struct curl_certinfo* ci;
        CURLcode res = curl_easy_getinfo(curl, CURLINFO_CERTINFO, &ci);
        if (res == CURLE_OK)
        {
            if (ci->num_of_certs > 0)
            {
                struct curl_slist* slist;
                const char* subject_key = "Subject:";
                const char* issuer_key = "Issuer:";
                for (slist = ci->certinfo[0]; slist; slist = slist->next)
                {
                    std::string data = slist->data;
                    size_t pos = data.find(subject_key);
                    if (pos != std::string::npos)
                    {
                        subject = data.substr(pos + strlen(subject_key));
                    }
                    pos = data.find(issuer_key);
                    if (pos != std::string::npos)
                    {
                        issuer = data.substr(pos + strlen(issuer_key));
                    }
                    if (subject.length() > 0 && issuer.length() > 0)
                    {
                        break;
                    }
                }
                //std::cout << "Certificate subject:\t" << subject << std::endl;
                //std::cout << "Certificate issuer:\t" << issuer << std::endl;
            }
        }
    }

    void CurlWrapper::DisplaySecurePage(const std::string& url)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Curl::write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);

        CURLcode res = curl_easy_perform(curl);

        GetCertificateInfo();

        std::cout << "Certificate subject:\t" << subject << std::endl;
        std::cout << "Certificate issuer:\t" << issuer << std::endl;
        std::wcout << L"Cipher used:\t" << (cipher.length() == 0 ? L"Unknown" : cipher.c_str()) << std::endl << std::endl;
        std::cout << "Page content for " << url << ":" << std::endl << content;
    }
}