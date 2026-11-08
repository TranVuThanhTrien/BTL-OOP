#pragma once
#include "Common.h"
// Người phụ trách: C | nhánh feature/payment-storage | LỚP 1 | Ví: khách (thanh toán, nhận hoàn tiền) và shipper (nhận phí ship) dùng chung
struct GiaoDichVi { string moTa; double soTien; string thoiDiem; };   // soTien > 0: tiền vào, < 0: tiền ra
class TaiKhoanVi {
    double m_soDu = 0;
    vector<GiaoDichVi> m_lichSu;
public:
    double soDu() const { return m_soDu; }
    const vector<GiaoDichVi>& lichSu() const { return m_lichSu; }
    void napTien(double soTien, const string& nguon);
    void truTien(double soTien, const string& moTa);
    void nhanTien(double soTien, const string& moTa);
    void hoanTien(double soTien, const string& moTa);
    // Mọi hàm trên ghi GiaoDichVi { moTa, soTien (có dấu), thoiGianHienTai() }
    void khoiPhuc(double soDu, vector<GiaoDichVi> lichSu) { m_soDu = soDu; m_lichSu = std::move(lichSu); }   // chỉ DataStore gọi khi đọc file
};
