#include "fetchData.h"
#include <curl/curl.h>
#include <json/json.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	static_cast<std::string*>(userp)->append(static_cast<char*>(contents), size * nmemb);
	return size * nmemb;
}

std::vector<DailyBar> fetch_alpha_vantage_daily_adjusted(
	const std::string& symbol,
	const std::string& apiKey,
	const std::string& outputSize
) {
	std::vector<DailyBar> out;

	std::string url_adj = "https://www.alphavantage.co/query?function=TIME_SERIES_DAILY_ADJUSTED"
					"&symbol=" + symbol + "&outputsize=" + outputSize +
					"&datatype=json&apikey=" + apiKey;
	std::string url_daily = "https://www.alphavantage.co/query?function=TIME_SERIES_DAILY"
					"&symbol=" + symbol + "&outputsize=" + outputSize +
					"&datatype=json&apikey=" + apiKey;

	auto perform_request = [](const std::string& url) -> std::string {
		CURL* curl = curl_easy_init();
		if (!curl) throw std::runtime_error("curl_easy_init failed");
		std::string body;
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "heston-calibrator/1.0");
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			curl_easy_cleanup(curl);
			throw std::runtime_error(curl_easy_strerror(res));
		}
		curl_easy_cleanup(curl);
		return body;
	};

	// 1) Tenter l'ajusté
	std::string body = perform_request(url_adj);
	{
		Json::Value root;
		Json::CharReaderBuilder builder;
		std::string errs;
		std::istringstream iss(body);
		if (Json::parseFromStream(builder, iss, &root, &errs)) {
			if (root.isMember("Error Message")) {
				// ... existing code ...
			}
			if (!root.isMember("Note")) {
				const Json::Value& series = root["Time Series (Daily)"];
				if (series.isObject()) {
					std::vector<std::string> dates = series.getMemberNames();
					std::sort(dates.begin(), dates.end());
					for (const std::string& date : dates) {
						const Json::Value& bar = series[date];
						if (!bar.isObject() || !bar.isMember("5. adjusted close")) continue;
						const std::string v = bar["5. adjusted close"].asString();
						out.push_back({date, std::stod(v)});
					}
					if (!out.empty()) return out; // Succès avec l'ajusté
				}
			}
		}
	}

	// 2) Fallback: non ajusté (gratuit)
	body = perform_request(url_daily);
	{
		Json::Value root;
		Json::CharReaderBuilder builder;
		std::string errs;
		std::istringstream iss(body);
		if (!Json::parseFromStream(builder, iss, &root, &errs)) {
			throw std::runtime_error("JSON parse error: " + errs);
		}
		if (root.isMember("Error Message")) throw std::runtime_error("Alpha Vantage error: invalid request/symbol");
		if (root.isMember("Note")) throw std::runtime_error("Alpha Vantage rate limit hit. Try later.");
		const Json::Value& series = root["Time Series (Daily)"];
		if (!series.isObject()) {
			std::ostringstream msg;
			msg << "Réponse invalide d'Alpha Vantage: champ 'Time Series (Daily)' absent.";
			throw std::runtime_error(msg.str());
		}
		std::vector<std::string> dates = series.getMemberNames();
		std::sort(dates.begin(), dates.end());
		for (const std::string& date : dates) {
			const Json::Value& bar = series[date];
			if (!bar.isObject() || !bar.isMember("4. close")) continue;
			const std::string v = bar["4. close"].asString();
			out.push_back({date, std::stod(v)});
		}
	}
	return out;
}
