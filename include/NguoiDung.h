#pragma once
#include "TaiKhoanVi.h"
// Người phụ trách: A | nhánh feature/core-domain | LỚP 2 (Common + TaiKhoanVi) | Phân quyền bằng đa hình
struct DiaChi { string hoTen, soDienThoai, diaChi; };   // CHUYỂN từ DonHang.h xuống đây vì KhachHang/NguoiBan cần (tránh vòng include)

// Tên chức năng cho coQuyen()/AuthService::yeuCauQuyen(). Dùng hằng, KHÔNG gõ chuỗi rời.
// Bảng quyền (X = có):             KhachHang NguoiBan Shipper QuanLy
//   DAT_HANG                         X
//   XEM_DON_CUA_MINH (khách/shop/hãng)  X       X        X
//   XEM_TAT_CA_DON                                                X
//   SUA_SAN_PHAM_SHOP (shop mình)             X                   X
//   QUAN_LY_SAN_PHAM (mọi shop)                                    X
//   QUAN_LY_KHUYEN_MAI / QUAN_LY_VAN_CHUYEN / QUAN_LY_TAI_KHOAN    X
//   DUNG_VI (xem ví, nạp tiền)        X                  X(xem)
//   XEM_THONG_KE                              X(shop mình)          X
//   GIAO_HANG                                          X
namespace quyen {
    inline const string DAT_HANG = "datHang", XEM_DON_CUA_MINH = "xemDonCuaMinh", XEM_TAT_CA_DON = "xemTatCaDon",
        SUA_SAN_PHAM_SHOP = "suaSanPhamShop", QUAN_LY_SAN_PHAM = "quanLySanPham", QUAN_LY_KHUYEN_MAI = "quanLyKhuyenMai",
        QUAN_LY_VAN_CHUYEN = "quanLyVanChuyen", QUAN_LY_TAI_KHOAN = "quanLyTaiKhoan", DUNG_VI = "dungVi",
        XEM_THONG_KE = "xemThongKe", GIAO_HANG = "giaoHang";
}

class NguoiDung {                                    // abstract
protected:
    string m_ma, m_tenDangNhap, m_matKhauBam, m_hoTen;   // m_tenDangNhap lưu dạng đã inHoa() với NguoiBan/Shipper; so sánh luôn dùng inThuong()
    bool m_biKhoa = false;
    NguoiDung(string ma, string tenDangNhap, string matKhauBam, string hoTen)
        : m_ma(std::move(ma)), m_tenDangNhap(std::move(tenDangNhap)), m_matKhauBam(std::move(matKhauBam)), m_hoTen(std::move(hoTen)) {}
    NguoiDung(const NguoiDung&) = default;
public:
    virtual ~NguoiDung() = default;
    virtual string vaiTro() const = 0;                          // một trong vaitro::*
    virtual bool   coQuyen(const string& chucNang) const = 0;   // chucNang là quyen::*, theo bảng trên
    virtual unique_ptr<NguoiDung> clone() const = 0;
    // Truy cập chung, để DonHang/Service KHÔNG phải dynamic_cast:
    virtual TaiKhoanVi*       vi()       { return nullptr; }    // KhachHang và ShipperVanChuyen có ví, còn lại nullptr
    virtual const TaiKhoanVi* vi() const { return nullptr; }
    virtual string maShop() const { return ""; }                // chỉ NguoiBan: TT, DT, NS...
    virtual string maHang() const { return ""; }                // chỉ Shipper: hang::GHN / GHTK / VIETTEL_POST

    const string& ma() const { return m_ma; }
    const string& tenDangNhap() const { return m_tenDangNhap; }
    const string& hoTen() const { return m_hoTen; }
    const string& matKhauBam() const { return m_matKhauBam; }
    bool biKhoa() const { return m_biKhoa; }
    void datHoTen(const string& t) { m_hoTen = t; }
    void datMatKhau(const string& matKhauThuan) { m_matKhauBam = bamMatKhau(matKhauThuan); }
    void khoa() { m_biKhoa = true; }
    void moKhoa() { m_biKhoa = false; }
    bool kiemTraMatKhau(const string& matKhauThuan) const { return kiemTraBam(matKhauThuan, m_matKhauBam); }
};

class KhachHang : public NguoiDung {                 // TODO(A-3) cài vaiTro/coQuyen/clone trong NguoiDung.cpp
    TaiKhoanVi m_vi; DiaChi m_diaChi;                // TODO(A-16): địa chỉ mặc định, lưu sau lần đặt hàng đầu
public:
    KhachHang(string ma, string tenDangNhap, string matKhauBam, string hoTen)
        : NguoiDung(std::move(ma), std::move(tenDangNhap), std::move(matKhauBam), std::move(hoTen)) {}
    string vaiTro() const override;  bool coQuyen(const string&) const override;  unique_ptr<NguoiDung> clone() const override;
    TaiKhoanVi*       vi() override { return &m_vi; }
    const TaiKhoanVi* vi() const override { return &m_vi; }
    const DiaChi& diaChi() const { return m_diaChi; }
    void datDiaChi(const DiaChi& d) { m_diaChi = d; }
};
class NguoiBan : public NguoiDung {                  // tên đăng nhập == mã shop (TT, DT, NS), 2-4 chữ cái in hoa
    string m_maShop, m_tenShop; DiaChi m_diaChiLayHang;   // TODO(A-16): người bán tự sửa địa chỉ lấy hàng
public:
    NguoiBan(string ma, string maShop, string matKhauBam, string tenShop)
        : NguoiDung(std::move(ma), maShop, std::move(matKhauBam), tenShop), m_maShop(std::move(maShop)), m_tenShop(std::move(tenShop)) {}
    string vaiTro() const override;  bool coQuyen(const string&) const override;  unique_ptr<NguoiDung> clone() const override;
    string maShop() const override { return m_maShop; }
    const string& tenShop() const { return m_tenShop; }
    const DiaChi& diaChiLayHang() const { return m_diaChiLayHang; }
    void datDiaChiLayHang(const DiaChi& d) { m_diaChiLayHang = d; }
};
class ShipperVanChuyen : public NguoiDung {          // TODO(A-15) tên đăng nhập GHN, GHTK, VTP; chỉ đụng tới đơn của hãng mình
    string m_maHang; TaiKhoanVi m_vi;                // ví nhận phí ship (cộng đúng 1 lần/đơn, xem DonHang::traPhiShip)
public:
    ShipperVanChuyen(string ma, string tenDangNhap, string matKhauBam, string hoTen, string maHang)
        : NguoiDung(std::move(ma), std::move(tenDangNhap), std::move(matKhauBam), std::move(hoTen)), m_maHang(std::move(maHang)) {}
    string vaiTro() const override;  bool coQuyen(const string&) const override;  unique_ptr<NguoiDung> clone() const override;
    TaiKhoanVi*       vi() override { return &m_vi; }
    const TaiKhoanVi* vi() const override { return &m_vi; }
    string maHang() const override { return m_maHang; }
};
class QuanLy : public NguoiDung {
public:
    QuanLy(string ma, string tenDangNhap, string matKhauBam, string hoTen)
        : NguoiDung(std::move(ma), std::move(tenDangNhap), std::move(matKhauBam), std::move(hoTen)) {}
    string vaiTro() const override;  bool coQuyen(const string&) const override;  unique_ptr<NguoiDung> clone() const override;
};
