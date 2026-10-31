#pragma once
#include "TaiKhoanVi.h"
// Người phụ trách: C | nhánh feature/payment-storage | LỚP 2 | Strategy thanh toán
class PhuongThucThanhToan {                          // abstract
public:
    virtual ~PhuongThucThanhToan() = default;
    virtual bool   thanhToan(double soTien) = 0;     // soTien = tổng cần trả (đã gồm phí xử lý). Thất bại -> ThanhToanThatBaiException. COD luôn true (thu khi giao)
    virtual double tinhPhiXuLy(double soTien) const = 0;   // soTien = tiền hàng - giảm + ship; đã lamTronDong
    virtual string ten() const = 0;                  // một trong pttt::*
    virtual unique_ptr<PhuongThucThanhToan> clone() const = 0;
};
class COD : public PhuongThucThanhToan {             // TODO(C-2) phí 0
public:
    bool thanhToan(double) override;  double tinhPhiXuLy(double) const override;
    string ten() const override { return pttt::COD; }  unique_ptr<PhuongThucThanhToan> clone() const override { return make_unique<COD>(*this); }
};
class TheTinDung : public PhuongThucThanhToan {      // TODO(C-2) phí hangso::PHI_THE_TIN_DUNG, kiểm tra số thẻ + hạn mức (giả lập)
public:
    bool thanhToan(double) override;  double tinhPhiXuLy(double) const override;
    string ten() const override { return pttt::THE_TIN_DUNG; }  unique_ptr<PhuongThucThanhToan> clone() const override { return make_unique<TheTinDung>(*this); }
};
class ViDienTu : public PhuongThucThanhToan {        // TODO(C-2) giữ con trỏ KHÔNG sở hữu tới ví của khách (ví nằm trong KhachHang do AuthService sở hữu, địa chỉ ổn định)
    TaiKhoanVi* m_vi;                                // clone() chép con trỏ; sau khi đọc đơn từ file con trỏ = nullptr và KHÔNG được gọi thanhToan lại
    string m_moTa;                                   // ghi vào lịch sử ví, vd "Thanh toán đơn DH001"
public:
    explicit ViDienTu(TaiKhoanVi* vi = nullptr, string moTa = "Thanh toán đơn hàng") : m_vi(vi), m_moTa(std::move(moTa)) {}
    void datMoTa(string moTa) { m_moTa = std::move(moTa); }
    bool thanhToan(double) override;                 // m_vi null hoặc thiếu tiền -> ThanhToanThatBaiException; gọi m_vi->truTien
    double tinhPhiXuLy(double) const override;
    string ten() const override { return pttt::VI_DIEN_TU; }  unique_ptr<PhuongThucThanhToan> clone() const override { return make_unique<ViDienTu>(*this); }
};
unique_ptr<PhuongThucThanhToan> taoPhuongThuc(const string& ten, TaiKhoanVi* viKhach = nullptr);   // TODO(C-3) factory; tên lạ -> ThanhToanThatBaiException
