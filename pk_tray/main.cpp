#include <Windows.h>
#include <filesystem>
#include <fstream>
#include "utils.hpp"
#include "tray.hpp"

int WinMain(
	_In_ HINSTANCE hInst, 
	_In_opt_ HINSTANCE hPrevInst, 
	_In_ PSTR lpCmdLine, 
	_In_ int nShowCmd
) {
	pk::sConfigData config;

	auto dataFolder = pk::getDataPath();
	if (dataFolder.has_value()) {
		if (!std::filesystem::exists(*dataFolder)) {
			std::filesystem::create_directories(*dataFolder);
		}

		auto configFile = *dataFolder / "config.json";
		
		if (std::filesystem::exists(configFile)) {
			std::ifstream ifs(configFile);
			config = nlohmann::json::parse(ifs).template get<pk::sConfigData>();
		}
		else {
			auto defaultConfig = nlohmann::json(config);
			std::ofstream ofs(configFile);
			ofs << defaultConfig.dump(4);
		}
	}

	pk::cTrayApp app(hInst, config);
	app.run();

	return 0;
}