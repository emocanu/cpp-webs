// webs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "curl/curl.h"

// prevent dumping th ehtml to the console, sink it
static size_t cb(void* data, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    return realsize;
}

int main()
{
    std::string text;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL* curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com");
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
    }

	curl_easy_cleanup(curl);
	curl_global_cleanup();
}
