#include "GioHang.h"
#include "OrderException.h"
// Người phụ trách: A | A-4, A-5

namespace {
void kiemTraSoLuongDuong(int soLuong) {
    if (soLuong <= 0) throw OrderException("Số lượng phải lớn hơn 0");
}
void bao_HetHang(const SanPham& sp, int canCo) {
    throw HetHangException("Sản phẩm " + sp.ma() + " chỉ còn " + std::to_string(sp.tonKho()) + ", không đủ " + std::to_string(canCo));
}
} // namespace

void GioHang::themSanPham(const SanPham& sp, int soLuong) {
    kiemTraSoLuongDuong(soLuong);
    for (auto& dong : m_dongHang) {
        if (dong.first.ma() != sp.ma()) continue;
        int moi = dong.second + soLuong;
        if (moi > sp.tonKho()) bao_HetHang(sp, moi);
        dong.first.datTonKho(sp.tonKho());      // giá giữ nguyên (chốt lúc thêm), chỉ cập nhật tồn kho mới nhất
        dong.second = moi;
        return;
    }
    if (soLuong > sp.tonKho()) bao_HetHang(sp, soLuong);
    m_dongHang.emplace_back(sp, soLuong);
}

void GioHang::capNhatSoLuong(const string& maSP, int soLuong) {
    if (soLuong < 0) throw OrderException("Số lượng không được âm");
    auto it = std::find_if(m_dongHang.begin(), m_dongHang.end(),
                           [&](const pair<SanPham,int>& d) { return d.first.ma() == maSP; });
    if (it == m_dongHang.end()) throw OrderException("Sản phẩm " + maSP + " không có trong giỏ hàng");
    if (soLuong == 0) { m_dongHang.erase(it); return; }
    if (soLuong > it->first.tonKho()) bao_HetHang(it->first, soLuong);
    it->second = soLuong;
}

void GioHang::xoaSanPham(const string& maSP) {
    auto it = std::find_if(m_dongHang.begin(), m_dongHang.end(),
                           [&](const pair<SanPham,int>& d) { return d.first.ma() == maSP; });
    if (it == m_dongHang.end()) throw OrderException("Sản phẩm " + maSP + " không có trong giỏ hàng");
    m_dongHang.erase(it);
}

void GioHang::lamTrong() { m_dongHang.clear(); }

double GioHang::tinhTongTien() const {
    double tong = 0;
    for (const auto& d : m_dongHang) tong += d.first.donGia() * d.second;
    return lamTronDong(tong);
}
double GioHang::tinhTongKhoiLuong() const {
    double kg = 0;
    for (const auto& d : m_dongHang) kg += d.first.khoiLuong() * d.second;
    return kg;
}
map<string,double> GioHang::tongTheoNguoiBan() const {
    map<string,double> kq;
    for (const auto& d : m_dongHang) kq[d.first.maNguoiBan()] += d.first.donGia() * d.second;
    for (auto& cap : kq) cap.second = lamTronDong(cap.second);
    return kq;
}
int GioHang::demNguoiBan() const { return (int)tongTheoNguoiBan().size(); }

TomTatGio GioHang::tomTat() const {
    TomTatGio t;
    t.tongTien = tinhTongTien();
    t.tongKhoiLuong = tinhTongKhoiLuong();
    t.theoNguoiBan = tongTheoNguoiBan();
    t.soNguoiBan = (int)t.theoNguoiBan.size();
    return t;
}
