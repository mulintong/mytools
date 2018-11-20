#pragma  once

#include <Windows.h>
#include <Winhttp.h>
#include <string>

//
// Support HTTP/HTTPS "GET" only
//
class HttpDownloader {
public:
    HttpDownloader(const std::wstring& url);
    ~HttpDownloader();
    void SetRetryTime(unsigned int retry) { retry_time_ = retry; }
    bool Request();
    const std::string& ResponseData() const { return response_data_; }
    DWORD LastError() const { return last_error_; }

private:
    bool Connect();
    void Close();
    bool StartRequest(const std::wstring& url_path, std::string& data);

    std::wstring user_agent_;

    std::wstring url_;
    std::wstring host_;
    bool use_https_;
    unsigned int retry_time_;

    INTERNET_PORT port_;
    HINTERNET session_; 
    HINTERNET connect_;
    HINTERNET request_;

    DWORD last_error_;

    std::string response_data_;
};