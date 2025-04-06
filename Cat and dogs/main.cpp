#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <cstdlib>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* out = (std::ofstream*)userp;
    size_t totalSize = size * nmemb;
    out->write((char*)contents, totalSize);
    return totalSize;
}

void downloadImage(const std::string& url, const std::string& filename) {
    CURL* curl;
    CURLcode res;
    std::ofstream outFile(filename, std::ios::binary);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error downloading image: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    outFile.close();
}

int main() {
    std::string baseUrl;
    std::string url;
    int imageCount = 0;

    std::cout << "Press Enter to download a random image (cat or dog)!" << std::endl;

    while (true) {
        std::cin.get();
        if (rand() % 2 == 0) {
            baseUrl = "http://localhost:5000/get_random_image";
        } else {
            baseUrl = "http://localhost:5000/get_random_image";
        }

        url = baseUrl;
        std::string filename = "image" + std::to_string(imageCount) + ".jpg";
        downloadImage(url, filename);
        std::cout << "Downloaded: " << filename << std::endl;
        imageCount++;
    }

    return 0;
}

