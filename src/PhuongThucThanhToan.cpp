#include "PhuongThucThanhToan.h"
#include "OrderException.h"

// Người phụ trách: C | nhánh feature/payment-storage | LỚP 2
// TODO(C-2): COD, TheTinDung (phí 1,5%, kiểm tra hạn mức thực tế), ViDienTu (trừ số dư)

bool COD::thanhToan(double) {
    return true;   // COD: thu tiền khi giao, luôn chấp nhận (theo hợp đồng trong PhuongThucThanhToan.h)
}

double COD::tinhPhiXuLy(double) const {
    return 0;
}

bool TheTinDung::thanhToan(double soTien) {
    if (soTien <= 0) {
        throw ThanhToanThatBaiException("Số tiền thanh toán qua thẻ tín dụng phải lớn hơn 0");
    }
    const double hanMucThe = hangso::HAN_MUC_THE_MOI_LAN;
    if (soTien > hanMucThe) {
        throw ThanhToanThatBaiException("Giao dịch vượt hạn mức thẻ tín dụng cho một lần thanh toán (" +
                                        dinhDangTien(hanMucThe) + ")");
    }
    return true;
}

double TheTinDung::tinhPhiXuLy(double soTien) const {
    if (soTien <= 0) return 0;
    return lamTronDong(soTien * hangso::PHI_THE_TIN_DUNG);
}

bool ViDienTu::thanhToan(double soTien) {
    if (!m_vi) {
        throw ThanhToanThatBaiException("Không tìm thấy ví điện tử liên kết để thanh toán");
    }
    m_vi->truTien(soTien, m_moTa);
    return true;
}

double ViDienTu::tinhPhiXuLy(double) const {
    return 0;
}

// TODO(C-3): factory; hỗ trợ cả tên chuẩn pttt::* và bí danh từ web cũ
unique_ptr<PhuongThucThanhToan> taoPhuongThuc(const string& ten, TaiKhoanVi* viKhach) {
    if (ten == pttt::COD) return make_unique<COD>();
    if (ten == pttt::THE_TIN_DUNG || ten == "The") return make_unique<TheTinDung>();
    if (ten == pttt::VI_DIEN_TU || ten == "Vi") return make_unique<ViDienTu>(viKhach);
    throw ThanhToanThatBaiException("Phương thức thanh toán không hợp lệ: " + ten);
}