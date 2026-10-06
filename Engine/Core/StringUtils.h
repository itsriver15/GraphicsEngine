#pragma once
#include <string>
#include <iostream>

namespace nu
{
	inline std::string ToLower(const std::string& str) {
		std::string lower = str;

		for (auto& c : lower) {
			c = std::tolower(c);
		}

		return lower;
	}

	inline std::string ToUpper(const std::string& str) {
		std::string upper = str;

		for (auto& c : upper) {
			c = std::toupper(c);
		}

		return upper;
	}

	inline bool EqualsIgnoreCase(const std::string& first, const std::string& second) {
		if (first.size() != second.size()) return false;
		// only compare the strings themselves if the sizes match
		return ToLower(first) == ToLower(second);
	}
}

