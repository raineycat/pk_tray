#include "api.hpp"

namespace pk {
	cPluralKit::cPluralKit(const sConfigData& config)
		: mClient("pk system tray/0.1.0.0 (raineycat/pk_tray)"),
		  mBasePath(config.basePath), mAuthToken(config.authToken) {
		mClient.connect(config.hostname);
	}

	std::optional<models::sSystem> cPluralKit::getSystem(const std::string& id) {
		sResponseData res = getEndpoint("/systems/" + id);
		if (!res.success || res.status != 200) {
			return std::nullopt;
		}

		return res.body.template get<models::sSystem>();
	}

	std::optional<models::sMember> cPluralKit::getMember(const std::string& id) {
		sResponseData res = getEndpoint("/members/" + id);
		if (!res.success || res.status != 200) {
			return std::nullopt;
		}

		return res.body.template get<models::sMember>();
	}

	std::vector<models::sMember> cPluralKit::getMembers(const std::string& system) {
		sResponseData res = getEndpoint("/systems/" + system + "/members");
		if (!res.success || res.status != 200) {
			return std::vector<models::sMember>();
		}

		return res.body.template get<std::vector<models::sMember>>();
	}

	std::vector<models::sMember> cPluralKit::getFronters(const std::string& system) {
		sResponseData res = getEndpoint("/systems/" + system + "/fronters");
		if (!res.success || res.status != 200) {
			return std::vector<models::sMember>();
		}

		auto data = res.body.template get<models::sSwitch>();
		return data.members;
	}

	void cPluralKit::setFronters(const std::vector<std::string>& members) {
		if (mAuthToken.empty()) {
			return;
		}

		nlohmann::json body;
		body["members"] = members;

		cHttpRequest req = mClient.post(mBasePath + "/systems/@me/switches");
		req.addHeader("Authorization", mAuthToken);
		req.addHeader("Content-Type", "application/json");
		req.setBody(body.dump());
		req.send();
		
		req.recieve();
		unsigned int status = req.getStatus();
		auto resp = req.readString();
		assert(status == 200);
	}

	sResponseData cPluralKit::getEndpoint(const std::string& path) {
		cHttpRequest req = mClient.get(mBasePath + path);
		if (!mAuthToken.empty()) {
			req.addHeader("Authorization", mAuthToken);
		}
		req.send();

		sResponseData data;
		data.success = false;

		if (!req.recieve()) {
			return data;
		}

		std::string bodyText = req.readString();
		if (bodyText.empty()) {
			return data;
		}

		data.success = true;
		data.status = req.getStatus();
		data.body = nlohmann::json::parse(bodyText);
		return data;
	}
}