#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include <random>
#include <curl/curl.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

std::string getRandomImage() {
    CURL* curl;
    CURLcode res;
    std::string image_data;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        if (rand() % 2 == 0) {
            curl_easy_setopt(curl, CURLOPT_URL, "https://cataas.com/cat");
        } else {
            curl_easy_setopt(curl, CURLOPT_URL, "https://dog.ceo/api/breeds/image/random");
        }

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
            [](void* contents, size_t size, size_t nmemb, void* userp) {
                ((std::string*)userp)->append((char*)contents, size * nmemb);
                return size * nmemb;
            });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &image_data);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error fetching image: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return image_data;
}

void handle_request(tcp::socket& socket) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);

        std::string image_data = getRandomImage();
        http::response<http::string_body> res{ http::status::ok, req.version() };
        res.set(http::field::content_type, "image/jpeg");
        res.body() = image_data;
        res.prepare_payload();

        http::write(socket, res);
    } catch (const std::exception& e) {
        std::cerr << "Error handling request: " << e.what() << std::endl;
    }
}

int main() {
    try {
        net::io_context ioc;
        tcp::acceptor acceptor{ ioc, {tcp::v4(), 5000} };

        std::cout << "Server is running on http://localhost:5000\n";

        while (true) {
            tcp::socket socket{ ioc };
            acceptor.accept(socket);
            handle_request(socket);
        }
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}
