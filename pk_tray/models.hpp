#pragma once

#include <string>
#include <vector>
#include <nlohmann_json.hpp>

#define GET_FIELD_TO(src, target, field) src.at(#field).get_to(target.field)
#define GET_OPT_FIELD_TO(src, target, field) if(src.count(#field) > 0 && !src.at(#field).is_null()) { \
												 src.at(#field).get_to(target.field); \
											 }

namespace pk {
	namespace models {
		struct sSystemPrivacy {
			std::string description_privacy;
			std::string pronoun_privacy;
			std::string member_list_privacy;
			std::string group_list_privacy;
			std::string front_privacy;
			std::string front_history_privacy;
		};

		inline void from_json(const nlohmann::json& j, sSystemPrivacy& privacy) {
			GET_OPT_FIELD_TO(j, privacy, description_privacy);
			GET_OPT_FIELD_TO(j, privacy, pronoun_privacy);
			GET_OPT_FIELD_TO(j, privacy, member_list_privacy);
			GET_OPT_FIELD_TO(j, privacy, group_list_privacy);
			GET_OPT_FIELD_TO(j, privacy, front_privacy);
			GET_OPT_FIELD_TO(j, privacy, front_history_privacy);
		}

		struct sSystem {
			std::string id;
			std::string uuid;
			std::string name;
			std::string description;
			std::string tag;
			std::string pronouns;
			std::string avatar_url;
			std::string banner;
			std::string color;
			std::string created;
			sSystemPrivacy privacy;
		};

		inline void from_json(const nlohmann::json& j, sSystem& sys) {
			GET_FIELD_TO(j, sys, id);
			GET_FIELD_TO(j, sys, uuid);
			GET_OPT_FIELD_TO(j, sys, name);
			GET_OPT_FIELD_TO(j, sys, description);
			GET_OPT_FIELD_TO(j, sys, tag);
			GET_OPT_FIELD_TO(j, sys, pronouns);
			GET_OPT_FIELD_TO(j, sys, avatar_url);
			GET_OPT_FIELD_TO(j, sys, banner);
			GET_OPT_FIELD_TO(j, sys, color);
			GET_OPT_FIELD_TO(j, sys, created);
			GET_OPT_FIELD_TO(j, sys, privacy);
		}

		struct sProxyTag {
			std::string prefix;
			std::string suffix;
		};

		inline void from_json(const nlohmann::json& j, sProxyTag& tag) {
			GET_OPT_FIELD_TO(j, tag, prefix);
			GET_OPT_FIELD_TO(j, tag, suffix);
		}

		struct sMemberPrivacy {
			std::string visibility;
			std::string name_privacy;
			std::string description_privacy;
			std::string birthday_privacy;
			std::string pronoun_privacy;
			std::string avatar_privacy;
			std::string metadata_privacy;
			std::string proxy_privacy;
		};

		inline void from_json(const nlohmann::json& j, sMemberPrivacy& privacy) {
			GET_OPT_FIELD_TO(j, privacy, visibility);
			GET_OPT_FIELD_TO(j, privacy, name_privacy);
			GET_OPT_FIELD_TO(j, privacy, description_privacy);
			GET_OPT_FIELD_TO(j, privacy, birthday_privacy);
			GET_OPT_FIELD_TO(j, privacy, pronoun_privacy);
			GET_OPT_FIELD_TO(j, privacy, avatar_privacy);
			GET_OPT_FIELD_TO(j, privacy, metadata_privacy);
			GET_OPT_FIELD_TO(j, privacy, proxy_privacy);
		}

		struct sMember {
			std::string id;
			std::string uuid;
			std::string system;
			std::string name;
			std::string display_name;
			std::string color;
			std::string birthday;
			std::string pronouns;
			std::string avatar_url;
			std::string webhook_avatar_url;
			std::string banner;
			std::string description;
			std::string created;
			std::vector<sProxyTag> proxy_tags;
			bool keep_proxy;
			bool tts;
			bool autoproxy_enabled;
			int message_count;
			std::string last_message_timestamp;
			sMemberPrivacy privacy;
		};

		inline void from_json(const nlohmann::json& j, sMember& member) {
			GET_FIELD_TO(j, member, id);
			GET_FIELD_TO(j, member, uuid);
			GET_OPT_FIELD_TO(j, member, system);
			GET_FIELD_TO(j, member, name);
			GET_OPT_FIELD_TO(j, member, display_name);
			GET_OPT_FIELD_TO(j, member, color);
			GET_OPT_FIELD_TO(j, member, birthday);
			GET_OPT_FIELD_TO(j, member, pronouns);
			GET_OPT_FIELD_TO(j, member, avatar_url);
			GET_OPT_FIELD_TO(j, member, webhook_avatar_url);
			GET_OPT_FIELD_TO(j, member, banner);
			GET_OPT_FIELD_TO(j, member, description);
			GET_OPT_FIELD_TO(j, member, created);
			GET_OPT_FIELD_TO(j, member, proxy_tags);
			GET_OPT_FIELD_TO(j, member, keep_proxy);
			GET_OPT_FIELD_TO(j, member, tts);
			GET_OPT_FIELD_TO(j, member, autoproxy_enabled);
			GET_OPT_FIELD_TO(j, member, message_count);
			GET_OPT_FIELD_TO(j, member, last_message_timestamp);
			GET_OPT_FIELD_TO(j, member, privacy);
		}

		struct sSwitch {
			std::string id;
			std::string timestamp;
			std::vector<sMember> members;
		};

		inline void from_json(const nlohmann::json& j, sSwitch& sw) {
			GET_FIELD_TO(j, sw, id);
			GET_FIELD_TO(j, sw, timestamp);
			GET_FIELD_TO(j, sw, members);
		}
	}
}