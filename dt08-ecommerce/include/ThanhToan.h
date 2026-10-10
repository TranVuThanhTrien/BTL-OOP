#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Exceptions.h"

// ===== Strategy thanh toán (abstract) - đúng chữ ký trong tài liệu =====
// Quy ước: soTien = tiền hàng - giảm giá + phí ship (CHƯA gồm phí xử lý).
// Phí xử lý do tinhPhiXuLy() tính và được cộng thêm khi trừ tiền thật.
class PhuongThucThanhToan {
public:
    virtual ~PhuongThucThanhToan() = default;
    // Thực hiện thanh toán, trả true nếu thành công; ném ThanhToanException nếu sai thông tin / thiếu tiền.
    virtual bool   thanhToan(double soTien) = 0;
    // Phí xử lý cho số tiền này (VNĐ): COD/Ví = 0, Thẻ = 1.5%.
    virtual double tinhPhiXuLy(double soTien) const = 0;
    virtual std::string getTen() const = 0;
    virtual std::unique_ptr<PhuongThucThanhToan> clone() const = 0;
};

// COD: không phí, thu tiền khi giao.
class COD : public PhuongThucThanhToan {
    double m_soTienCanThu;
    bool   m_daThu;
public:
    explicit COD(double soTienCanThu = 0, bool daThu = false);   // throw nếu số tiền < 0
    bool   thanhToan(double soTien) override;    // ghi nhận số tiền cần thu (chưa thu thật); throw nếu đã thu
    double tinhPhiXuLy(double soTien) const override;
    std::string getTen() const override;
    std::unique_ptr<PhuongThucThanhToan> clone() const override;
    void   xacNhanDaThu();                       // gọi khi giao xong; throw nếu chưa đặt hoặc đã thu
    bool   daThuTien() const;
    double getSoTienCanThu() const;
};

// Thẻ tín dụng: phí 1.5%, kiểm tra số thẻ (độ dài, chữ số, Luhn) và hạn mức.
class TheTinDung : public PhuongThucThanhToan {
    static constexpr double PHI_THE_TIN_DUNG = 0.015;
    struct KhoiPhucTag {};
    std::string m_soThe;      // chỉ chữ số, hoặc "************1234" khi khôi phục từ file
    double      m_hanMuc;     // hạn mức còn lại
    TheTinDung(KhoiPhucTag, std::string soTheDaAn, double hanMuc);
public:
    TheTinDung(const std::string& soThe, double hanMuc);         // throw nếu số thẻ sai hoặc hạn mức < 0
    static TheTinDung khoiPhuc(const std::string& soTheCuoi4, double hanMucConLai);   // dùng khi đọc file
    bool   thanhToan(double soTien) override;    // throw nếu vượt hạn mức
    double tinhPhiXuLy(double soTien) const override;
    std::string getTen() const override;
    std::unique_ptr<PhuongThucThanhToan> clone() const override;
    std::string getSoTheDaAn() const;            // "**** **** **** 1234"
    double getHanMucConLai() const;
};

// Ví điện tử: không phí, trừ trực tiếp vào số dư.
class ViDienTu : public PhuongThucThanhToan {
    double m_soDu;
public:
    explicit ViDienTu(double soDuBanDau = 0);    // throw nếu < 0
    bool   thanhToan(double soTien) override;    // throw nếu không đủ số dư
    double tinhPhiXuLy(double soTien) const override;
    std::string getTen() const override;
    std::unique_ptr<PhuongThucThanhToan> clone() const override;
    void   napTien(double soTien);               // throw nếu soTien <= 0
    double getSoDu() const;
};

// ===== Lưu / đọc JSON (không cần thư viện ngoài, không đổi interface ở trên) =====
// File mẫu:
// [
//   {"loai":"COD","soTienCanThu":210000,"daThu":false},
//   {"loai":"TheTinDung","soTheCuoi":"1111","hanMuc":5000000},
//   {"loai":"ViDienTu","soDu":500000}
// ]
// Số thẻ đầy đủ không bao giờ được ghi ra.
namespace ThanhToanJson {
    using DanhSach = std::vector<std::unique_ptr<PhuongThucThanhToan>>;

    // Một phương thức -> chuỗi JSON một dòng. Ném FileException nếu là loại không hỗ trợ.
    std::string toJson(const PhuongThucThanhToan& pt);
    // Chuỗi JSON một object -> phương thức. Ném FileException nếu sai định dạng / dữ liệu không hợp lệ.
    std::unique_ptr<PhuongThucThanhToan> tuJson(const std::string& json);

    std::string toJson(const DanhSach& ds);                 // cả danh sách -> mảng JSON
    DanhSach    tuJsonDanhSach(const std::string& json);    // mảng JSON -> danh sách

    // Ghi file (ghi ra file tạm rồi đổi tên, tự tạo thư mục cha). Ném FileException nếu không ghi được.
    void luu(const DanhSach& ds, const std::string& duongDan);
    // Đọc file. Ném FileException nếu không mở được hoặc JSON sai.
    DanhSach doc(const std::string& duongDan);
}
