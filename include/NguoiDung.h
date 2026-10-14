#pragma once
#include "Common.h"
// Người phụ trách: A | nhánh feature/core-domain | Phân quyền bằng đa hình
class NguoiDung {                                    // abstract
protected:
    string m_ma, m_tenDangNhap, m_matKhauBam, m_hoTen;
    bool m_biKhoa = false;
public:
    virtual ~NguoiDung() = default;
    virtual string vaiTro() const = 0;                          // "KhachHang" | "NguoiBan" | "QuanLy"
    virtual bool   coQuyen(const string& chucNang) const = 0;   // vd "xemTatCaDon", "suaSanPham", "quanLyTaiKhoan"
    virtual unique_ptr<NguoiDung> clone() const = 0;
    // TODO(A-3): getter/setter, khoa()/moKhoa(), kiemTraMatKhau(const string& matKhauThuan) const
};
// TODO(A-3): class KhachHang : public NguoiDung   // m_diaChi; sở hữu một TaiKhoanVi (ví); chỉ xem đơn của mình
// TODO(A-3): class NguoiBan  : public NguoiDung   // m_maNguoiBan = mã shop 2-4 chữ cái in hoa (TT, DT, NS), cũng là tên đăng nhập; quản lý sản phẩm và đơn của shop mình
// TODO(A-16): NguoiBan có DiaChi lấy hàng, sửa được; KhachHang có DiaChi mặc định, lưu lại sau lần đặt hàng đầu
// TODO(A-15): class ShipperVanChuyen : public NguoiDung   // m_maHang ("GHN" | "GHTK" | "ViettelPost"), tên đăng nhập GHN, GHTK, VTP; chỉ nhận giao và giao thành công đơn của hãng mình
// TODO(A-3): class QuanLy    : public NguoiDung   // toàn quyền: sản phẩm, khuyến mãi, vận chuyển, tài khoản
