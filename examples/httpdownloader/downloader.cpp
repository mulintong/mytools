#include "stdafx.h"
#include "downloader.h"

#pragma comment(lib, "winhttp.lib")

HttpDownloader::HttpDownloader(const std::wstring& url)
  : url_(url) {
    user_agent_ = L"HttpDownloader example";
    use_https_ = false;
    session_ = nullptr;
    connect_ = nullptr;
    request_ = nullptr;
    retry_time_ = 0;

    last_error_ = 0;
}

HttpDownloader::~HttpDownloader() {
    Close();
}

void HttpDownloader::Close() {
    if (request_) {
        WinHttpCloseHandle(request_);
        request_ = nullptr;
    }

    if (connect_) {
        WinHttpCloseHandle(connect_);
        connect_ = nullptr;
    }

    if (session_) {
        WinHttpCloseHandle(session_);
        session_ = nullptr;
    }
}

bool HttpDownloader::Request(){
    URL_COMPONENTS url_comp = {0};
    url_comp.dwStructSize = sizeof(URL_COMPONENTS);

    url_comp.dwSchemeLength = (DWORD)-1;
    url_comp.dwHostNameLength = (DWORD)-1;
    url_comp.dwUrlPathLength = (DWORD)-1;
    url_comp.dwExtraInfoLength = (DWORD)-1;

    BOOL ret = ::WinHttpCrackUrl(url_.c_str(), static_cast<DWORD>(url_.length()), 0, &url_comp);
    if (!ret) {
        last_error_ = ::GetLastError();
        return false;
    }

    switch (url_comp.nScheme) {
    case INTERNET_SCHEME_HTTP:
        port_ = INTERNET_DEFAULT_HTTP_PORT;
        break;
    case INTERNET_SCHEME_HTTPS:
        port_ = INTERNET_DEFAULT_HTTPS_PORT;
        use_https_ = true;
        break;
    default:
        last_error_ = ERROR_NOT_SUPPORTED;
        return false;
        break;
    }

    host_.assign(url_comp.lpszHostName, url_comp.dwHostNameLength);

    const int kMaxRequestTime = retry_time_ + 1;

    for (int i = 0; i < kMaxRequestTime; ++i) {
        Close();

        if (Connect()) {
            response_data_.clear();
            if (StartRequest(url_comp.lpszUrlPath, response_data_)) {
                last_error_ = 0;
                return true;
            }
        }
    }

    last_error_ = ::GetLastError();

    return false;
}

bool HttpDownloader::Connect() {
    // Use WinHttpOpen to obtain a session handle.
    session_ = WinHttpOpen(user_agent_.c_str(),
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    // Specify an HTTP server.
    if (session_) {
        connect_ = WinHttpConnect(session_, host_.c_str(), port_, 0);
        return true;
    }

    return false;
}

bool HttpDownloader::StartRequest(const std::wstring& url_path, std::string& data) {

    // Create an HTTP request handle.
    if (connect_) {
        request_ = WinHttpOpenRequest(connect_, L"GET", url_path.c_str(),
            NULL, WINHTTP_NO_REFERER, 
            WINHTTP_DEFAULT_ACCEPT_TYPES, 
            use_https_ ? WINHTTP_FLAG_SECURE : 0);
    }

    BOOL bresult = FALSE;
    // Send a request.
    if (request_) {
        bresult = WinHttpSendRequest( request_,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0, WINHTTP_NO_REQUEST_DATA, 0, 
            0, 0);
    }

    // End the request.
    if (bresult) {
        bresult = WinHttpReceiveResponse( request_, NULL);
    }

    // Keep checking for data until there is nothing left.
    if (bresult) {
        DWORD dwsize = 0;
        DWORD download;
        do {
            // Check for available data.    
            if (!WinHttpQueryDataAvailable( request_, &dwsize)) {
                printf( "Error %u in WinHttpQueryDataAvailable.\n",
                    GetLastError());
                break;
            }

            // No more available data.
            if (!dwsize)
                break;

            // Allocate space for the buffer.
            LPSTR out_buffer = new char[dwsize+1];
            if (!out_buffer) {
                printf("Out of memory\n");
                break;
            }

            // Read the Data.
            ZeroMemory(out_buffer, dwsize+1);

            if (!WinHttpReadData( request_, (LPVOID)out_buffer, dwsize, &download)) {
                printf( "Error %u in WinHttpReadData.\n", GetLastError());
            } else {
                printf("%s", out_buffer);
            }

            if (out_buffer != nullptr && download > 0) {
                data.append(out_buffer, dwsize);
            }

            // Free the memory allocated to the buffer.
            delete [] out_buffer;

            // This condition should never be reached since WinHttpQueryDataAvailable
            // reported that there are bits to read.
            if (!download)
                break;
        } while (dwsize > 0);
        return true;
    } else {
        // Report any errors.
        printf( "Error %d has occurred.\n", GetLastError() );
        return false;
    }
}
