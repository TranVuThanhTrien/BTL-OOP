#pragma once
#include "Common.h"
// Người phụ trách: C | nhánh feature/payment-storage | Strategy thanh toán
class PhuongThucThanhToan {                          // abstract
public:
    virtual ~PhuongThucThanhToan() = default;
    virtual bool   thanhToan(double soTien) = 0;     // throw ThanhToanThatBaiException
    virtual double tinhPhiXuLy(double soTien) const = 0;
    virtual string ten() const = 0;
    virtual unique_ptr<PhuongThucThanhToan> clone() const = 0;
};
// TODO(C-2): class COD        // thu hộ khi giao, phí 0
// TODO(C-2): class TheTinDung // phí 1,5%, kiểm tra số thẻ và hạn mức (giả lập)
// TODO(C-2): class ViDienTu   // giữ con trỏ KHÔNG sở hữu tới TaiKhoanVi của khách; thanh toán gọi truTien(); thiếu tiền -> ThanhToanThatBaiException
// TODO(C-3): unique_ptr<PhuongThucThanhToan> taoPhuongThuc(const string& ten);  // "COD" | "TheTinDung" | "ViDienTu"
