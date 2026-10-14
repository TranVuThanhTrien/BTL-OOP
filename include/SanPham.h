#pragma once
#include "Common.h"
// Người phụ trách: A | nhánh feature/core-domain
// m_maNguoiBan là mã shop viết tắt 2-4 chữ cái in hoa: TT (Thời Trang), DT (Điện Tử), NS (Nhà Sách)
class SanPham {
    string m_ma, m_ten, m_maNguoiBan;
    double m_donGia, m_khoiLuong; int m_tonKho;
public:
    SanPham(string ma, string ten, string maNguoiBan, double donGia, double khoiLuong, int tonKho);
    // TODO(A-1): getter/setter; kiểm tra dữ liệu (đơn giá > 0, khối lượng > 0, tồn kho >= 0)
    // TODO(A-2): void giamTon(int soLuong);  // throw HetHangException nếu không đủ
    // TODO(A-22): int daBan() const;  double danhGia() const;  void congDaBan(int soLuong);   // hiển thị "Đã bán" và sao đánh giá, daBan tăng khi đặt hàng thành công
    // TODO(A-2): void tangTon(int soLuong);
};
// NguoiDung, KhachHang, NguoiBan, QuanLy: xem NguoiDung.h
