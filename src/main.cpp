#include <curl/curl.h>
#include <bsoncxx/json.hpp>
#include <cstdlib>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <typeinfo>
#include <vector>
#include "calibrator.h"
#include "fetchData.h"
using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* buffer = static_cast<std::string*>(userp);
    buffer->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

auto get_data(std::string symbol, std::string api_key) {
    std ::string url_price = "https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol=" + symbol + "&outputsize=compact&datatype=json&apikey=" + api_key;
    std ::string response;
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url_price.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std ::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std ::endl;
    }
    curl_easy_cleanup(curl);
    json json_response = json::parse(response);
    if (json_response) {
        std ::cout << "JSON parsed successfully!" << std ::endl;
    } else {
        std ::cerr << "JSON parse error: " << std ::endl;
    }
    
}
using namespace std;
int main() {
    vector <int> a = {1, 2, 3, 4, 5};
    a.pop_back();
    a.pop_back();
    for (int i = 0; i < a.size(); i++) {
        cout << a[i] << endl;
    }

    while (!a.empty()) {
        cout << a.back() << endl;
        a.pop_back();
    }
    // are you will to do what it take to make it happen ? i will not give up until i succeed    
    return 0;
}