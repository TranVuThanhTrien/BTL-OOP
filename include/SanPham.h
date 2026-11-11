#pragma once
#include "Common.h"
// Người phụ trách: A | nhánh feature/core-domain | LỚP 1 (chỉ phụ thuộc Common.h)
// m_maNguoiBan là mã shop viết tắt 2-4 chữ cái in hoa: TT (Thời Trang), DT (Điện Tử), NS (Nhà Sách)
// Getter/setter đơn giản đã viết sẵn; A chỉ cần cài phần có kiểm tra trong src/SanPham.cpp (ném OrderException/HetHangException).
class SanPham {
    string m_ma, m_ten, m_maNguoiBan;
    double m_donGia, m_khoiLuong, m_danhGia;
    int m_tonKho, m_daBan;
public:
    SanPham(string ma, string ten, string maNguoiBan, double donGia, double khoiLuong, int tonKho, int daBan = 0, double danhGia = 0);
    const string& ma() const { return m_ma; }
    const string& ten() const { return m_ten; }
    const string& maNguoiBan() const { return m_maNguoiBan; }
    double donGia() const { return m_donGia; }
    double khoiLuong() const { return m_khoiLuong; }
    int    tonKho() const { return m_tonKho; }
    int    daBan() const { return m_daBan; }
    double danhGia() const { return m_danhGia; }
    void datTen(const string& ten);
    void datDonGia(double donGia);
    void datKhoiLuong(double khoiLuong);
    void datTonKho(int tonKho);
    void giamTon(int soLuong);
    void tangTon(int soLuong);
    void congDaBan(int soLuong);
};
