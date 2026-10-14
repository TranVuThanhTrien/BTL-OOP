#pragma once
#include "Common.h"
// Người phụ trách: C | nhánh feature/payment-storage | PUSH & MERGE NGAY TUẦN 1 (cả nhóm dùng)
class OrderException : public std::exception {
    string m_msg;
public:
    explicit OrderException(string msg) : m_msg(std::move(msg)) {}
    const char* what() const noexcept override { return m_msg.c_str(); }
};
class HetHangException             : public OrderException { using OrderException::OrderException; };
class MaGiamGiaKhongHopLeException : public OrderException { using OrderException::OrderException; };
class ChuyenTrangThaiSaiException  : public OrderException { using OrderException::OrderException; };
class ThanhToanThatBaiException    : public OrderException { using OrderException::OrderException; };
class FileDataException            : public OrderException { using OrderException::OrderException; };
class DangNhapThatBaiException     : public OrderException { using OrderException::OrderException; };   // sai mật khẩu, tài khoản bị khóa, token sai
class KhongCoQuyenException        : public OrderException { using OrderException::OrderException; };   // thiếu quyền (HTTP 403)
// TODO(C-1): thêm hàm lấy tên lớp exception để server trả về trường "loi" trong JSON
