// webs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define SECURITY_WIN32

#include <iostream>
#include <string>
#include "curl/curl.h"
#include "schannel.h"
#include "Sspi.h"

// TODO move these in class/functions
CURL* curl = nullptr;
std::wstring cipher(L"");

// prevent dumping the html to the console, sink it
static size_t cb(void* data, size_t size, size_t nmemb, void* userp)
{
    if (cipher.length() == 0) // do this only once
    {
        const struct curl_tlssessioninfo* info = NULL;
        CURLcode res = curl_easy_getinfo(curl, CURLINFO_TLS_SSL_PTR, &info);
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

int main()
{
    std::string text;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.google.com");
    curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &text);
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK)
    {
        struct curl_certinfo* ci;
        res = curl_easy_getinfo(curl, CURLINFO_CERTINFO, &ci);
        if (res == CURLE_OK)
        {
            if (ci->num_of_certs > 0)
            {
                struct curl_slist* slist;
                std::string issuer, subject;
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
                std::cout << "Certificate subject:\t" << subject << std::endl;
                std::cout << "Certificate issuer:\t" << issuer << std::endl;
            }
        }

        std::wcout << L"Cipher used:\t" << (cipher.length() == 0 ? L"Unknown" : cipher.c_str()) << std::endl;

    }

	curl_easy_cleanup(curl);
	curl_global_cleanup();
}
