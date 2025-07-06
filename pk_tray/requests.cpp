#include "requests.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <sstream>

namespace pk {
	cHttpClient::cHttpClient(const std::string& userAgent, eHttpFlags flags) {
		mSession = WinHttpOpen(
			pk::s2ws(userAgent).c_str(),
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS,
			static_cast<DWORD>(flags));

		if (!mSession) {
			throw std::runtime_error("Failed to create WinHTTP session!");
		}
	}

	cHttpClient::cHttpClient(cHttpClient&& other) noexcept {
		mSession = other.mSession;
		other.mSession = NULL;

		mServer = other.mServer;
		other.mServer = NULL;
	}

	cHttpClient::~cHttpClient() {
		if (mServer)  WinHttpCloseHandle(mServer);
		WinHttpCloseHandle(mSession);
	}

	void cHttpClient::connect(const std::string& server, unsigned int port) {
		mServer = WinHttpConnect(
			mSession,
			pk::s2ws(server).c_str(),
			port,
			0);

		if (!mServer) {
			throw std::runtime_error("Failed to connect to the server!");
		}
	}

	cHttpRequest cHttpClient::get(const std::string& path) {
		HANDLE req = WinHttpOpenRequest(
			mServer,
			L"GET",
			pk::s2ws(path).c_str(),
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			0);

		if (!req) {
			throw std::runtime_error("Failed to open the request!");
		}

		return cHttpRequest(req);
	}

	cHttpRequest cHttpClient::post(const std::string& path) {
		HANDLE req = WinHttpOpenRequest(
			mServer,
			L"POST",
			pk::s2ws(path).c_str(),
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			0);

		if (!req) {
			throw std::runtime_error("Failed to open the request!");
		}

		return cHttpRequest(req);
	}

	cHttpRequest::cHttpRequest(HANDLE handle)
		: mHandle(handle) {

	}

	cHttpRequest::cHttpRequest(cHttpRequest&& other) noexcept {
		mHandle = other.mHandle;
		other.mHandle = NULL;
	}

	cHttpRequest::~cHttpRequest() {
		WinHttpCloseHandle(mHandle);
	}

	void cHttpRequest::addHeader(const std::string& name, const std::string& value) {
		std::wstringstream header;
		header << pk::s2ws(name) << L": ";
		header << pk::s2ws(value) << "\r\n";

		WinHttpAddRequestHeaders(
			mHandle,
			header.str().c_str(),
			(ULONG)-1L,
			WINHTTP_ADDREQ_FLAG_REPLACE | WINHTTP_ADDREQ_FLAG_ADD);
	}

	void cHttpRequest::setBody(const std::string& body) {
		mRequestBody = body + "\r\n";
	}

	void cHttpRequest::send() {
		bool hasBody = !mRequestBody.empty();

		bool result = WinHttpSendRequest(
			mHandle,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0,
			hasBody ? mRequestBody.data() : WINHTTP_NO_REQUEST_DATA,
			hasBody ? mRequestBody.size() + 1 : 0,
			hasBody ? mRequestBody.size() + 1 : 0,
			NULL);

		if (!result) {
			throw std::runtime_error("Failed to send request!");
		}
	}

	bool cHttpRequest::recieve() {
		return WinHttpReceiveResponse(mHandle, NULL);
	}

	unsigned int cHttpRequest::getStatus() const {
		DWORD statusCode;
		DWORD dataSize = sizeof(statusCode);

		bool result = WinHttpQueryHeaders(
			mHandle,
			WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			WINHTTP_HEADER_NAME_BY_INDEX,
			&statusCode, 
			&dataSize, 
			WINHTTP_NO_HEADER_INDEX);

		if (result) {
			return statusCode;
		}

		return 0;
	}

	size_t cHttpRequest::available() const {
		DWORD count;
		if (WinHttpQueryDataAvailable(mHandle, &count)) {
			return count;
		}

		return 0;
	}

	size_t cHttpRequest::readData(void* buffer, size_t bytesToRead) {
		DWORD bytesRead;
		if (WinHttpReadData(mHandle, buffer, static_cast<DWORD>(bytesToRead), &bytesRead)) {
			return bytesRead;
		}

		return 0;
	}

	std::string cHttpRequest::readString() {
		std::stringstream body;
		size_t bodySize;

		while ((bodySize = available()) > 0) {
			char* buffer = new char[bodySize + 1];
			if (!buffer) break;

			memset(buffer, 0, bodySize);
			if (readData(buffer, bodySize) > 0) {
				buffer[bodySize] = 0;
				body << buffer;
			}

			delete[] buffer;
		}

		return body.str();
	}
}