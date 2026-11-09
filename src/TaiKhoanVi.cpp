#include "TaiKhoanVi.h"
#include "OrderException.h"

// Người phụ trách: C | nhánh feature/payment-storage | LỚP 1
void TaiKhoanVi::napTien(double soTien, const string& nguon) {
    double tien = lamTronDong(soTien);
    if (tien < hangso::NAP_TOI_THIEU || tien > hangso::NAP_TOI_DA) {
        throw ThanhToanThatBaiException("Số tiền nạp phải từ " + dinhDangTien(hangso::NAP_TOI_THIEU) +
                                        " đến " + dinhDangTien(hangso::NAP_TOI_DA));
    }
    m_soDu = lamTronDong(m_soDu + tien);
    string moTa = nguon.empty() ? "Nạp tiền vào ví" : ("Nạp tiền (" + nguon + ")");
    m_lichSu.push_back(GiaoDichVi{moTa, tien, thoiGianHienTai()});
}

void TaiKhoanVi::truTien(double soTien, const string& moTa) {
    double tien = lamTronDong(soTien);
    if (tien <= 0) {
        throw ThanhToanThatBaiException("Số tiền thanh toán phải lớn hơn 0");
    }
    if (m_soDu < tien) {
        throw ThanhToanThatBaiException("Ví không đủ số dư (" + dinhDangTien(tien) + " > " +
                                        dinhDangTien(m_soDu) + "), hãy nạp thêm tại mục Ví của tôi");
    }
    m_soDu = lamTronDong(m_soDu - tien);
    m_lichSu.push_back(GiaoDichVi{moTa.empty() ? "Thanh toán đơn hàng" : moTa, -tien, thoiGianHienTai()});
}

void TaiKhoanVi::nhanTien(double soTien, const string& moTa) {
    double tien = lamTronDong(soTien);
    if (tien <= 0) {
        throw ThanhToanThatBaiException("Số tiền nhận phải lớn hơn 0");
    }
    m_soDu = lamTronDong(m_soDu + tien);
    m_lichSu.push_back(GiaoDichVi{moTa, tien, thoiGianHienTai()});
}

void TaiKhoanVi::hoanTien(double soTien, const string& moTa) {
    nhanTien(soTien, moTa);
}
