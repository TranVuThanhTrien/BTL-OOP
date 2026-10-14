#pragma once
#include "Common.h"
// Người phụ trách: C | nhánh feature/payment-storage | Ví điện tử của khách hàng: nạp tiền, trừ tiền, lịch sử
struct GiaoDichVi { string moTa; double soTien; string thoiDiem; };   // soTien > 0: nạp, < 0: thanh toán
class TaiKhoanVi {
    double m_soDu = 0;
    vector<GiaoDichVi> m_lichSu;
public:
    // TODO(C-10): double soDu() const;  const vector<GiaoDichVi>& lichSu() const;
    // TODO(C-10): void napTien(double soTien, const string& nguon);   // 10.000 <= soTien <= 20.000.000, sai -> ThanhToanThatBaiException
    // TODO(C-10): void truTien(double soTien, const string& moTa);    // không đủ số dư -> ThanhToanThatBaiException
    // TODO(C-11): void hoanTien(double soTien, const string& moTa);   // cộng tiền hoàn do shop gửi, ghi lịch sử; ShipperVanChuyen dùng chung TaiKhoanVi để nhận phí ship khi đơn Hoàn tất
    // TODO(C-10): ghi GiaoDichVi (mô tả, số tiền, thời điểm) mỗi lần nạp hoặc trừ
};
