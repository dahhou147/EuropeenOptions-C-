#include <iostream>
#include <vector>
#include "calibrator.h"
#include "fetchData.h"
#include <curl/curl.h>
#include <cstdlib>
#include <stdexcept>

int main() {
	try {
		curl_global_init(CURL_GLOBAL_DEFAULT);

		const char* keyEnv = std::getenv("ALPHAVANTAGE_API_KEY");
		std::string apiKey = keyEnv ? std::string(keyEnv) : std::string("4BB1GER4AEU7BZUV");

		std::vector<DailyBar> data = fetch_alpha_vantage_daily_adjusted("AAPL", apiKey, "compact");
		for (const auto& bar : data) {
			std::cout << bar.date << ": " << bar.adjustedClose << std::endl;
		}

		curl_global_cleanup();
		return 0;
	} catch (const std::exception& e) {
		std::cerr << "Erreur: " << e.what() << std::endl;
		curl_global_cleanup();
		return 1;
	}
}