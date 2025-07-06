#include "tray.hpp"
#include "utils.hpp"
#include "resources/resource.h"
#include <algorithm>

#define ID_TRAY_APP_ICON 1001
#define ID_TRAY_EXIT 1002
#define ID_TRAY_TEST 1003
#define ID_TRAY_OPEN_CONFIG 1004
#define ID_TRAY_MY_SYSTEM 1005
#define ID_TRAY_SET_FRONT 1006

namespace pk {
	class __declspec(uuid("16766A1D-5F9A-43A5-878C-19F8C8B68747")) cNotification;

	static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		cTrayApp* app;

		if (msg == WM_CREATE) {
			LPCREATESTRUCTA cs = reinterpret_cast<LPCREATESTRUCTA>(lParam);
			app = reinterpret_cast<cTrayApp*>(cs->lpCreateParams);
			if (app) {
				SetWindowLongPtrA(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
			}
		}
		else {
			LONG_PTR ptr = GetWindowLongPtrA(hWnd, GWLP_USERDATA);
			app = reinterpret_cast<cTrayApp*>(ptr);
		}

		if (app) {
			return app->handleMessage(hWnd, msg, wParam, lParam);
		}
		else {
			return DefWindowProcA(hWnd, msg, wParam, lParam);
		}
	}

	struct sSystemInfoData {
		models::sSystem system;
		std::vector<models::sMember> members;
	};

	static LRESULT CALLBACK SystemInfoDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		sSystemInfoData* info = reinterpret_cast<sSystemInfoData*>(lParam);
		HWND memberList;

		switch (msg)
		{
		case WM_INITDIALOG:
			SetWindowTextA(hWnd, info->system.name.c_str());
			SetDlgItemTextA(hWnd, IDC_SYSTEM_NAME, info->system.name.c_str());
			SetDlgItemTextA(hWnd, IDC_SYSTEM_PRONOUNS, info->system.pronouns.c_str());
			SetDlgItemTextA(hWnd, IDC_SYSTEM_DESC, info->system.description.c_str());
			
			memberList = GetDlgItem(hWnd, IDC_MEMBER_LIST);
			for (const auto& member : info->members) {
				int pos = (int)SendMessage(memberList, LB_ADDSTRING, 0, (LPARAM)member.display_name.c_str());
			}

			return (INT_PTR)TRUE;

		case WM_COMMAND:
			switch (HIWORD(wParam)) {
			case BN_CLICKED:
				switch (LOWORD(wParam)) {
				case IDOK:
					EndDialog(hWnd, 1);
					break;
				}
				break;
			}
			break;

		case WM_CLOSE:
			EndDialog(hWnd, LOWORD(wParam));
			break;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	cTrayApp::cTrayApp(HINSTANCE instance, const sConfigData& config)
		: mInstance(instance), mPK(config), mWindowClass("pk_tray window") {
		createWindow();
	}

	cTrayApp::~cTrayApp() {
		if(mContextMenu) DestroyMenu(mContextMenu);
		if(mWindow) DestroyWindow(mWindow);
	}

	void cTrayApp::run() const {
		MSG msg{ 0 };
		while (GetMessageA(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}

	void cTrayApp::createWindow() {
		WNDCLASSA wc{ 0 };
		wc.lpfnWndProc = StaticWindowProc;
		wc.hInstance = mInstance;
		wc.lpszClassName = mWindowClass.c_str();
		wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
		wc.hIcon = LoadIconA(mInstance, MAKEINTRESOURCE(IDI_PK));
		wc.style = CS_HREDRAW | CS_VREDRAW;

		if (!RegisterClassA(&wc)) {
			throw std::runtime_error("Failed to register window class!");
		}

		mWindow = CreateWindowExA(
			0,
			mWindowClass.c_str(),
			"PluralKit system tray",
			0,
			0, 0,
			0, 0,
			NULL,
			NULL,
			mInstance,
			this);

		if (!mWindow) {
			throw std::runtime_error("Failed to create window!");
		}

		ShowWindow(mWindow, SW_HIDE);
	}

	void cTrayApp::createMenu() {
		mContextMenu = CreatePopupMenu();
		AppendMenuA(mContextMenu, MF_STRING | MF_GRAYED, NULL, "[PK tray v0.1.0.0]");
		AppendMenuA(mContextMenu, MF_SEPARATOR, NULL, NULL);

		AppendMenuA(mContextMenu, MF_STRING | MF_GRAYED, NULL, "[System]");
		// AppendMenuA(mContextMenu, MF_STRING, ID_TRAY_TEST, "Test");
		AppendMenuA(mContextMenu, MF_STRING, ID_TRAY_MY_SYSTEM, "View system info");
		AppendMenuA(mContextMenu, MF_SEPARATOR, NULL, NULL);

		AppendMenuA(mContextMenu, MF_STRING | MF_GRAYED, NULL, "[Fronter]");
		auto memberList = mPK.getMembers("@me");
		auto fronters = mPK.getFronters("@me");
		if (memberList.empty()) {
			AppendMenuA(mContextMenu, MF_STRING, NULL, "There are no members!");
			AppendMenuA(mContextMenu, MF_STRING, NULL, "Have you added your token?");
		}
		int i = 0;
		for (const auto& member : memberList) {
			bool isFronting = std::find_if(fronters.begin(), fronters.end(),
				[member](const models::sMember& m) { return m.name == member.name; }) != fronters.end();
			UINT extraFlags = isFronting ? MF_CHECKED : 0;
			AppendMenuA(mContextMenu, MF_STRING | extraFlags, ID_TRAY_SET_FRONT + i, member.name.c_str());
			i++;
		}
		AppendMenuA(mContextMenu, MF_SEPARATOR, NULL, NULL);

		AppendMenuA(mContextMenu, MF_STRING | MF_GRAYED, NULL, "[Options]");
		AppendMenuA(mContextMenu, MF_STRING, ID_TRAY_OPEN_CONFIG, "Open config.json");
		AppendMenuA(mContextMenu, MF_STRING, ID_TRAY_EXIT, "Exit");
	}

	void cTrayApp::createNotification(HWND window) {
		ZeroMemory(&mTrayIcon, sizeof(mTrayIcon));
		mTrayIcon.cbSize = sizeof(mTrayIcon);
		mTrayIcon.hWnd = window;
		mTrayIcon.guidItem = __uuidof(cNotification);
		mTrayIcon.uID = ID_TRAY_APP_ICON;
		mTrayIcon.uFlags = NIF_ICON | NIF_TIP | NIF_SHOWTIP | NIF_MESSAGE;
		mTrayIcon.uCallbackMessage = WM_APP;
		mTrayIcon.hIcon = LoadIconA(mInstance, MAKEINTRESOURCE(IDI_PK));
		LoadString(mInstance, IDS_TOOLTIP, mTrayIcon.szTip, 128);

		if (!Shell_NotifyIconA(NIM_ADD, &mTrayIcon)) {
			throw std::runtime_error("Failed to create notification!");
		}

		mTrayIcon.uVersion = NOTIFYICON_VERSION_4;
		if (!Shell_NotifyIconA(NIM_SETVERSION, &mTrayIcon)) {
			throw std::runtime_error("Failed to set notification version!");
		}
	}

	void cTrayApp::handleMenuClick(HWND hwnd, WORD item) {
		switch (item) {
		case ID_TRAY_EXIT:
			PostQuitMessage(0);
			return;

		case ID_TRAY_TEST:
			MessageBoxA(mWindow, "TEST!!", "PK tray", MB_ICONINFORMATION | MB_OK);
			return;

		case ID_TRAY_OPEN_CONFIG:
		{
			auto path = pk::getDataPath();
			if (path.has_value()) {
				*path /= "config.json";

				ShellExecuteA(
					mWindow,
					"edit",
					path->string().c_str(),
					NULL,
					NULL,
					SW_SHOW);
			}
			return;
		}

		case ID_TRAY_MY_SYSTEM: 
		{
			auto system = mPK.getSystem("@me");
			if (system.has_value()) {
				showSystemDialog(*system);
			}
			else {
				MessageBoxA(
					mWindow,
					"Failed to get your system info! Have you set a token in config.json?",
					"Error",
					MB_ICONERROR | MB_OK);
			}
			return;
		}

		// anything including and after ID_TRAY_SET_FRONT
		default:
		{
			MENUITEMINFOA info{ 0 };
			info.cbSize = sizeof(info);
			info.fMask = MIIM_STRING;

			// get the length of the text
			info.cch = 0;
			info.dwTypeData = NULL;
			if (!GetMenuItemInfoA(mContextMenu, item, FALSE, &info)) {
				return;
			}

			// get the actual text
			info.cch += 1; // null byte
			std::string memberName(info.cch, '\0');
			info.dwTypeData = memberName.data();
			if (!GetMenuItemInfoA(mContextMenu, item, FALSE, &info)) {
				return;
			}
			memberName.resize(info.cch);

			// send the request
			auto allMembers = mPK.getMembers("@me");
			for (const auto& m : allMembers) {
				if (m.name == memberName) {
					mPK.setFronters({ m.id });
					break;
				}
			}

			// reset checkmarks
			uncheckAllMenuItems(mContextMenu);

			// check the new item
			info.dwTypeData = NULL;
			info.cch = 0;
			info.hbmpChecked = NULL;
			info.fMask = MIIM_STATE;
			info.fState = MFS_CHECKED;
			SetMenuItemInfoA(mContextMenu, item, FALSE, &info);
		}
		}
	}

	void cTrayApp::showSystemDialog(const models::sSystem& sys) {
		sSystemInfoData data;
		data.system = sys;
		data.members = mPK.getMembers(sys);

		HWND handle = CreateDialogParamA(
			mInstance,
			MAKEINTRESOURCEA(IDD_SYSTEM_INFO),
			mWindow,
			SystemInfoDialogProc,
			reinterpret_cast<LPARAM>(&data));

		ShowWindow(handle, SW_SHOW);
	}

	void cTrayApp::uncheckAllMenuItems(HMENU menu) {
		int count = GetMenuItemCount(menu);
		for (int i = 0; i < count; i++) {
			MENUITEMINFOA info{ 0 };
			info.cbSize = sizeof(info);
			info.fMask = MIIM_STATE;
			if (!GetMenuItemInfoA(menu, i, TRUE, &info)) {
				continue;
			}

			info.fState &= ~MFS_CHECKED;
			SetMenuItemInfoA(menu, i, TRUE, &info);
		}
	}

	LRESULT cTrayApp::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_CREATE:
			mTaskbarRestartMsg = RegisterWindowMessageA("TaskbarCreated");
			createMenu();
			createNotification(hWnd);
			return 0;

		case WM_APP:
			switch (LOWORD(lParam)) {
			case WM_CONTEXTMENU:
				POINT pt;
				GetCursorPos(&pt);
				SetForegroundWindow(hWnd);
				TrackPopupMenu(mContextMenu, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
				break;
			}
			return 0;

		case WM_COMMAND:
			handleMenuClick(hWnd, LOWORD(wParam));
			return 0;

		case WM_DESTROY:
			Shell_NotifyIcon(NIM_DELETE, &mTrayIcon);
			PostQuitMessage(0);
			return 0;

		default:
			if (msg == mTaskbarRestartMsg) {
				createNotification(mWindow);
				return 0;
			}

			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}
}