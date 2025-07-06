#pragma once

#include "config.hpp"
#include "requests.hpp"
#include "models.hpp"
#include <nlohmann_json.hpp>
#include <optional>
#include <vector>

namespace pk {
	struct sResponseData {
		bool success;
		unsigned int status;
		nlohmann::json body;
	};

	class cPluralKit {
	public:
		cPluralKit(const sConfigData& config);

		std::optional<models::sSystem> getSystem(const std::string& id);
		std::optional<models::sMember> getMember(const std::string& id);
		std::vector<models::sMember> getMembers(const std::string& system);
		std::vector<models::sMember> getFronters(const std::string& system);
		void setFronters(const std::vector<std::string>& members);
		
		inline std::vector<models::sMember> getMembers(const models::sSystem& system) {
			return getMembers(system.id);
		}

		inline std::vector<models::sMember> getFronters(const models::sSystem& system) {
			return getFronters(system.id);
		}

	private:
		sResponseData getEndpoint(const std::string& path);

	private:
		cHttpClient mClient;
		std::string mBasePath;
		std::string mAuthToken;
	};
}