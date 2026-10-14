#pragma once
#include "SanPham.h"
// Người phụ trách: A | nhánh feature/core-domain | Giỏ hàng đa nhà bán lẻ
class GioHang {
    vector<std::pair<SanPham,int>> m_dongHang;       // sản phẩm + số lượng
public:
    // TODO(A-4): void themSanPham(const SanPham& sp, int soLuong);   // throw HetHangException
    // TODO(A-4): void capNhatSoLuong(const string& maSP, int soLuong);
    // TODO(A-4): void xoaSanPham(const string& maSP);
    // TODO(A-4): void lamTrong();
    // TODO(A-5): double tinhTongTien() const;
    // TODO(A-5): double tinhTongKhoiLuong() const;
    // TODO(A-5): int demNguoiBan() const;
    // TODO(A-5): map<string,double> tongTheoNguoiBan() const;   // tiền từng nhà bán
};
