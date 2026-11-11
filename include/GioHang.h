#pragma once
#include "SanPham.h"
// Người phụ trách: A | nhánh feature/core-domain | LỚP 2 | Giỏ hàng đa nhà bán lẻ
// TomTatGio: số liệu tối thiểu để tính khuyến mãi. GioHang VÀ DonHang đều tạo ra nó, nên KhuyenMai không cần biết ai gọi
// (skeleton cũ để KhuyenMai nhận GioHang, nhưng DonHang chỉ có ChiTietDonHang -> không tính lại được giảm giá).
struct TomTatGio {
    double tongTien = 0, tongKhoiLuong = 0;
    int soNguoiBan = 0;
    map<string,double> theoNguoiBan;                 // mã shop -> tiền hàng của shop
};
class GioHang {
    vector<pair<SanPham,int>> m_dongHang;            // bản sao sản phẩm (chốt giá lúc thêm) + số lượng
public:
    const vector<pair<SanPham,int>>& dongHang() const { return m_dongHang; }
    bool rong() const { return m_dongHang.empty(); }
    void themSanPham(const SanPham& sp, int soLuong);
    void capNhatSoLuong(const string& maSP, int soLuong);
    void xoaSanPham(const string& maSP);
    void lamTrong();
    double tinhTongTien() const;
    double tinhTongKhoiLuong() const;
    int    demNguoiBan() const;
    map<string,double> tongTheoNguoiBan() const;
    TomTatGio tomTat() const;
};
