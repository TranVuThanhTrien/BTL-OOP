#include "SanPham.h"
#include "OrderException.h"
// Người phụ trách: A | A-1, A-2, A-22
// Mọi kiểm tra dữ liệu nằm ở các hàm kiểm tra tĩnh bên dưới để ctor và setter dùng chung một quy tắc.

namespace {
void kiemTraMa(const string& ma) {
    if (ma.empty()) throw OrderException("Mã sản phẩm không được để trống");
}
void kiemTraTen(const string& ten) {
    if (ten.empty()) throw OrderException("Tên sản phẩm không được để trống");
}
void kiemTraMaNguoiBan(const string& ma) {
    if (!laMaShopHopLe(ma)) throw OrderException("Mã người bán không hợp lệ (cần 2-4 chữ cái): " + ma);
}
void kiemTraDonGia(double donGia) {
    if (!(donGia > 0)) throw OrderException("Đơn giá phải lớn hơn 0");
}
void kiemTraKhoiLuong(double khoiLuong) {
    if (!(khoiLuong > 0)) throw OrderException("Khối lượng phải lớn hơn 0");
}
void kiemTraTonKho(int tonKho) {
    if (tonKho < 0) throw OrderException("Tồn kho không được âm");
}
void kiemTraDaBan(int daBan) {
    if (daBan < 0) throw OrderException("Số lượng đã bán không được âm");
}
void kiemTraDanhGia(double danhGia) {
    if (!(danhGia >= 0 && danhGia <= 5)) throw OrderException("Điểm đánh giá phải nằm trong khoảng 0 đến 5");
}
void kiemTraSoLuong(int soLuong) {
    if (soLuong <= 0) throw OrderException("Số lượng phải lớn hơn 0");
}
} // namespace

SanPham::SanPham(string ma, string ten, string maNguoiBan, double donGia, double khoiLuong, int tonKho, int daBan, double danhGia)
    : m_ma(std::move(ma)), m_ten(std::move(ten)), m_maNguoiBan(std::move(maNguoiBan)),
      m_donGia(donGia), m_khoiLuong(khoiLuong), m_danhGia(danhGia), m_tonKho(tonKho), m_daBan(daBan) {
    kiemTraMa(m_ma);
    kiemTraTen(m_ten);
    kiemTraMaNguoiBan(m_maNguoiBan);
    kiemTraDonGia(m_donGia);
    kiemTraKhoiLuong(m_khoiLuong);
    kiemTraTonKho(m_tonKho);
    kiemTraDaBan(m_daBan);
    kiemTraDanhGia(m_danhGia);
}

void SanPham::datTen(const string& ten)         { kiemTraTen(ten);               m_ten = ten; }
void SanPham::datDonGia(double donGia)          { kiemTraDonGia(donGia);         m_donGia = donGia; }
void SanPham::datKhoiLuong(double khoiLuong)    { kiemTraKhoiLuong(khoiLuong);   m_khoiLuong = khoiLuong; }
void SanPham::datTonKho(int tonKho)             { kiemTraTonKho(tonKho);         m_tonKho = tonKho; }

void SanPham::giamTon(int soLuong) {
    kiemTraSoLuong(soLuong);
    if (soLuong > m_tonKho)
        throw HetHangException("Sản phẩm " + m_ma + " không đủ hàng (còn " + std::to_string(m_tonKho) + ", cần " + std::to_string(soLuong) + ")");
    m_tonKho -= soLuong;
}
void SanPham::tangTon(int soLuong) {
    kiemTraSoLuong(soLuong);
    m_tonKho += soLuong;
}
void SanPham::congDaBan(int soLuong) {
    kiemTraSoLuong(soLuong);
    m_daBan += soLuong;
}
