#pragma once

#include <Windows.h>
#include <winhttp.h>
#include <string>

namespace pk {
	enum class eHttpFlags {
		None,
		Async = 0x10000000,
		SecureDefaults = 0x30000000
	};

	class cHttpRequest;
	class cHttpClient {
	public:
		cHttpClient(const std::string& userAgent, eHttpFlags flags = eHttpFlags::None);
		cHttpClient(const cHttpClient&) = delete;
		cHttpClient(cHttpClient&&) noexcept;
		~cHttpClient();

		void connect(const std::string& server, unsigned int port = 0);
		cHttpRequest get(const std::string& path);
		cHttpRequest post(const std::string& path);

	private:
		HINTERNET mSession = NULL;
		HINTERNET mServer = NULL;
	};

	class cHttpRequest {
	friend class cHttpClient;
	
	private:
		cHttpRequest(HINTERNET handle);

	public:
		cHttpRequest(const cHttpRequest&) = delete;
		cHttpRequest(cHttpRequest&&) noexcept;
		~cHttpRequest();

		void addHeader(const std::string& name, const std::string& value);
		void setBody(const std::string& body);
		void send();

		bool recieve();
		unsigned int getStatus() const;
		size_t available() const;
		size_t readData(void* buffer, size_t bytesToRead);
		std::string readString();

	private:
		HINTERNET mHandle = NULL;
		std::string mRequestBody = "";
	};
}
