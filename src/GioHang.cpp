#include "GioHang.h"
#include "OrderException.h"

// Người phụ trách: A | nhánh feature/core-domain | LỚP 2
void GioHang::themSanPham(const SanPham& sp, int soLuong) {
    if (soLuong <= 0) {
        throw OrderException("Số lượng thêm vào giỏ phải lớn hơn 0");
    }
    for (auto& dong : m_dongHang) {
        if (dong.first.ma() == sp.ma()) {
            int moi = dong.second + soLuong;
            if (moi > sp.tonKho()) {
                throw HetHangException(sp.ten() + " chỉ còn " + std::to_string(sp.tonKho()) + " sản phẩm");
            }
            dong.first = sp;
            dong.second = moi;
            return;
        }
    }
    if (soLuong > sp.tonKho()) {
        throw HetHangException(sp.ten() + " chỉ còn " + std::to_string(sp.tonKho()) + " sản phẩm");
    }
    m_dongHang.emplace_back(sp, soLuong);
}

void GioHang::capNhatSoLuong(const string& maSP, int soLuong) {
    if (soLuong < 0) {
        throw OrderException("Số lượng cập nhật không được âm");
    }
    if (soLuong == 0) {
        xoaSanPham(maSP);
        return;
    }
    for (auto& dong : m_dongHang) {
        if (dong.first.ma() == maSP) {
            if (soLuong > dong.first.tonKho()) {
                throw HetHangException(dong.first.ten() + " chỉ còn " + std::to_string(dong.first.tonKho()) + " sản phẩm");
            }
            dong.second = soLuong;
            return;
        }
    }
    throw OrderException("Không tìm thấy sản phẩm trong giỏ: " + maSP);
}

void GioHang::xoaSanPham(const string& maSP) {
    m_dongHang.erase(
        std::remove_if(m_dongHang.begin(), m_dongHang.end(),
                       [&](const pair<SanPham, int>& d) { return d.first.ma() == maSP; }),
        m_dongHang.end());
}

void GioHang::lamTrong() {
    m_dongHang.clear();
}

double GioHang::tinhTongTien() const {
    double tong = 0;
    for (const auto& d : m_dongHang) {
        tong += d.first.donGia() * d.second;
    }
    return lamTronDong(tong);
}

double GioHang::tinhTongKhoiLuong() const {
    double kl = 0;
    for (const auto& d : m_dongHang) {
        kl += d.first.khoiLuong() * d.second;
    }
    return kl;
}

map<string, double> GioHang::tongTheoNguoiBan() const {
    map<string, double> kq;
    for (const auto& d : m_dongHang) {
        kq[d.first.maNguoiBan()] = lamTronDong(kq[d.first.maNguoiBan()] + d.first.donGia() * d.second);
    }
    return kq;
}

int GioHang::demNguoiBan() const {
    return static_cast<int>(tongTheoNguoiBan().size());
}

TomTatGio GioHang::tomTat() const {
    TomTatGio tt;
    tt.tongTien = tinhTongTien();
    tt.tongKhoiLuong = tinhTongKhoiLuong();
    tt.theoNguoiBan = tongTheoNguoiBan();
    tt.soNguoiBan = static_cast<int>(tt.theoNguoiBan.size());
    return tt;
}
