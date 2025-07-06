#pragma once

#include <string>
#include <nlohmann_json.hpp>

namespace pk {
	struct sConfigData {
		std::string __comment = "Warning: If you change this file, you need to restart the program!";
		std::string hostname = "api.pluralkit.me";
		std::string basePath = "/v2";
		std::string authToken = "";
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(sConfigData, __comment, hostname, basePath, authToken);
}