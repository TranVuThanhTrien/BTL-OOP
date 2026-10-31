#pragma once
#include "Common.h"
// Người phụ trách: C | nhánh feature/payment-storage | ĐÃ CHỐT v1 – merge ngay tuần 1 (cả nhóm dùng)
// Mỗi lớp lỗi có: loai() = tên lớp (server trả trường "loi" trong JSON) và maHttp() (D-7: 400 mặc định, 401 đăng nhập, 403 quyền).
class OrderException : public std::exception {
    string m_msg;
public:
    explicit OrderException(string msg) : m_msg(std::move(msg)) {}
    const char* what() const noexcept override { return m_msg.c_str(); }
    virtual string loai() const { return "OrderException"; }
    virtual int    maHttp() const { return 400; }
};
#define DT08_EXCEPTION(Ten, Ma)                                         \
    class Ten : public OrderException {                                 \
    public:                                                             \
        using OrderException::OrderException;                           \
        string loai() const override { return #Ten; }                   \
        int maHttp() const override { return Ma; }                      \
    };
DT08_EXCEPTION(HetHangException,             400)
DT08_EXCEPTION(MaGiamGiaKhongHopLeException, 400)
DT08_EXCEPTION(ChuyenTrangThaiSaiException,  400)
DT08_EXCEPTION(ThanhToanThatBaiException,    400)
DT08_EXCEPTION(FileDataException,            500)   // lỗi đọc/ghi file: lỗi phía server
DT08_EXCEPTION(DangNhapThatBaiException,     401)   // sai mật khẩu, tài khoản bị khóa, token sai/thiếu
DT08_EXCEPTION(KhongCoQuyenException,        403)   // thiếu quyền
#undef DT08_EXCEPTION
// Dùng trong catch của server: res.status = e.maHttp(); body = { "loi": e.loai(), "thongBao": e.what() }
