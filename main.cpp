#include "iostream"
#include "fstream"
#include "curl/curl.h"

bool debugMode = false;

std::string tweet_link;
std::string response;
std::string theLineWeWant;

size_t write_callback(void *ptr, size_t size, size_t nmemb, std::string *data) {
    data->append((char *)ptr, size * nmemb);
    return size * nmemb;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

CURL* curl;
CURLcode res;

void GetADownloadLink()
{
    int lineCount = 0;
    std::string epic;
    std::fstream HTMLFile("site.html");
    while(getline(HTMLFile, epic))
    {
        if(lineCount == 305)
        {
            theLineWeWant = epic;
        }
        lineCount++;
    }
    theLineWeWant.replace(0, 57, "");
    theLineWeWant.replace(theLineWeWant.length() - 1, theLineWeWant.length(), "");
    if(debugMode)
    {
        std::cout<< theLineWeWant << std::endl;
    }
}


void GetTheHTMLFile()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, ("http://twitsave.com/info?url=" + tweet_link).c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            std::cout<< "first curl_easy_perform() failed" << curl_easy_strerror(res) << std::endl;
        }
        else{
            long html_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &html_code);
            std::cout<< "Response code : " << html_code << std::endl;
            std::cout<< "Response : " << response << std::endl;
            std::ofstream HTMLFile("site.html");
            HTMLFile << response;
            HTMLFile.close();
        }
    }
    curl_easy_cleanup(curl);

}

void DownloadTheFile()
{
    FILE *videoFile;
    tweet_link.replace(0, 23, "");
    std::string filename = tweet_link + ".mp4";
    curl = curl_easy_init();
    if(curl)
    {
        videoFile = fopen(filename.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, theLineWeWant.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, videoFile);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            std::cout<< "second curl_easy_perform() failed" << curl_easy_strerror(res) << std::endl;
        }
        else{
            long html_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &html_code);
            std::cout<< "Response code : " << html_code << std::endl;
            std::cout<< "Response : " << response << std::endl;
        }
        curl_easy_cleanup(curl);
        fclose(videoFile);
    }
}

int main(int argc, char* argv[])
{
    tweet_link = argv[1];
    if(tweet_link.find("https://"))
    {
        std::cerr << "Please use an actual tweet link" << std::endl;
        return 0;
    }
    GetTheHTMLFile();
    GetADownloadLink();
    DownloadTheFile();
    remove("site.html");
    return 0;
}