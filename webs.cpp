#include "CurlWrapper.h"

int main()
{
	Curl::CurlWrapper curl;
	curl.DisplaySecurePage("https://www.example.com");
	curl.DisplaySecurePage("https://www.google.com");
}
