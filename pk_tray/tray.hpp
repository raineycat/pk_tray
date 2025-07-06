#pragma once

#include <Windows.h>
#include "config.hpp"
#include "api.hpp"

namespace pk {
	class cTrayApp {
	public:
		cTrayApp(HINSTANCE instance, const sConfigData& config);
		cTrayApp(const cTrayApp&) = delete;
		~cTrayApp();

		void run() const;
		LRESULT handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		void createWindow();
		void createMenu();
		void createNotification(HWND window);
		
		void handleMenuClick(HWND hwnd, WORD item);
		void showSystemDialog(const models::sSystem& sys);
		void uncheckAllMenuItems(HMENU menu);

	private:
		HINSTANCE mInstance;
		cPluralKit mPK;
		std::string mWindowClass;
		HWND mWindow = NULL;
		HMENU mContextMenu = NULL;
		NOTIFYICONDATAA mTrayIcon{ 0 };
		UINT mTaskbarRestartMsg = 0;
	};
}