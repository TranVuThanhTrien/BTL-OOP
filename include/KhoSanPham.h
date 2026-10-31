#pragma once
#include "SanPham.h"
// Người phụ trách: A | MỚI (skeleton cũ thiếu): nơi DUY NHẤT giữ danh sách sản phẩm trong bộ nhớ.
// DonHangService và các route /api/san-pham, /api/quan-ly/san-pham đều đi qua lớp này (không ai giữ vector<SanPham> riêng).
class KhoSanPham {
    vector<SanPham> m_ds;
public:
    KhoSanPham() = default;
    explicit KhoSanPham(vector<SanPham> ds) : m_ds(std::move(ds)) {}
    const vector<SanPham>& tatCa() const { return m_ds; }
    // D-21: tuKhoa (chứa, không phân biệt hoa thường), maShop ("" = tất cả), sapXep: "phoBien" | "moiNhat" | "banChay" | "giaTang" | "giaGiam" | "" (mặc định phoBien)
    vector<SanPham> tim(const string& tuKhoa, const string& maShop, const string& sapXep) const;
    const SanPham& lay(const string& maSP) const;     // không có -> OrderException
    SanPham&       lay(const string& maSP);
    void them(SanPham sp);                            // trùng mã -> OrderException
    void xoa(const string& maSP);
    void nhapKho(const string& maSP, int soLuong);    // = lay(maSP).tangTon(soLuong)
};
