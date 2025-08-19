#pragma once
#include <string>
#include <vector>

struct DailyBar {
	std::string date;
	double adjustedClose;
};

std::vector<DailyBar> fetch_alpha_vantage_daily_adjusted(
	const std::string& symbol,
	const std::string& apiKey,
	const std::string& outputSize = "compact"
);
