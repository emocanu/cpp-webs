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
            }
        }
    }

    void CurlWrapper::DisplaySecurePage(const std::string& url)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);
        Curl_C::curl_c = curl;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Curl_C::write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);

        CURLcode res = curl_easy_perform(curl);

        GetCertificateInfo();
        std::cout << std::endl << "Displays secure page information for " << url << std::endl << std::endl;
        std::cout << "Certificate subject:\t" << subject << std::endl;
        std::cout << "Certificate issuer:\t" << issuer << std::endl;
        cipher = Curl_C::cipher;
        std::wcout << L"Cipher used:\t" << (cipher.length() == 0 ? L"Unknown" : cipher.c_str()) << std::endl << std::endl;
        std::cout << "Page content:" << std::endl << std::endl << content;
    }
}