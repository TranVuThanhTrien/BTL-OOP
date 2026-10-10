#pragma once
#include <exception>
#include <string>
#include <utility>

// Exception gốc của toàn project. what() trả về thông báo tiếng Việt để web hiển thị nguyên văn.
class AppException : public std::exception {
    std::string m_msg;
public:
    explicit AppException(std::string msg) : m_msg(std::move(msg)) {}
    const char* what() const noexcept override { return m_msg.c_str(); }
};

class ThanhToanException : public AppException { using AppException::AppException; };
class KhuyenMaiException : public AppException { using AppException::AppException; };
class TrangThaiException : public AppException { using AppException::AppException; };
class FileException      : public AppException { using AppException::AppException; };
