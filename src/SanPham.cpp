#include "SanPham.h"
#include "OrderException.h"

// Người phụ trách: A | nhánh feature/core-domain | LỚP 1
SanPham::SanPham(string ma, string ten, string maNguoiBan, double donGia, double khoiLuong, int tonKho, int daBan, double danhGia)
    : m_ma(std::move(ma)), m_ten(std::move(ten)), m_maNguoiBan(inHoa(std::move(maNguoiBan))),
      m_donGia(lamTronDong(donGia)), m_khoiLuong(khoiLuong), m_danhGia(danhGia),
      m_tonKho(tonKho), m_daBan(daBan) {
    if (m_ma.empty() || m_ten.empty())
        throw OrderException("Mã và tên sản phẩm không được để trống");
    if (!laMaShopHopLe(m_maNguoiBan))
        throw OrderException("Mã người bán không hợp lệ (2-4 chữ cái)");
    if (m_donGia <= 0)
        throw OrderException("Đơn giá sản phẩm phải lớn hơn 0");
    if (m_khoiLuong <= 0)
        throw OrderException("Khối lượng sản phẩm phải lớn hơn 0");
    if (m_tonKho < 0)
        throw OrderException("Tồn kho không được âm");
    if (m_daBan < 0)
        throw OrderException("Số lượng đã bán không được âm");
    if (m_danhGia < 0 || m_danhGia > 5)
        throw OrderException("Đánh giá sản phẩm phải nằm trong khoảng 0 đến 5");
}

void SanPham::datTen(const string& ten) {
    if (ten.empty()) throw OrderException("Tên sản phẩm không được để trống");
    m_ten = ten;
}

void SanPham::datDonGia(double donGia) {
    if (donGia <= 0) throw OrderException("Đơn giá sản phẩm phải lớn hơn 0");
    m_donGia = lamTronDong(donGia);
}

void SanPham::datKhoiLuong(double khoiLuong) {
    if (khoiLuong <= 0) throw OrderException("Khối lượng sản phẩm phải lớn hơn 0");
    m_khoiLuong = khoiLuong;
}

void SanPham::datTonKho(int tonKho) {
    if (tonKho < 0) throw OrderException("Tồn kho không được âm");
    m_tonKho = tonKho;
}

void SanPham::giamTon(int soLuong) {
    if (soLuong <= 0) throw OrderException("Số lượng giảm tồn phải lớn hơn 0");
    if (soLuong > m_tonKho)
        throw HetHangException(m_ten + " chỉ còn " + std::to_string(m_tonKho) + " sản phẩm");
    m_tonKho -= soLuong;
}

void SanPham::tangTon(int soLuong) {
    if (soLuong <= 0) throw OrderException("Số lượng tăng tồn phải lớn hơn 0");
    m_tonKho += soLuong;
}

void SanPham::congDaBan(int soLuong) {
    if (soLuong <= 0) throw OrderException("Số lượng cộng đã bán phải lớn hơn 0");
    m_daBan += soLuong;
}
