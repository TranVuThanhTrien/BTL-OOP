#include "NguoiDung.h"
// Người phụ trách: A | A-3, A-15, A-16
// Phân quyền bằng đa hình: mỗi vai trò tự trả lời coQuyen() theo bảng quyền ghi ở đầu NguoiDung.h.

namespace {
bool thuocDanhSach(const string& chucNang, std::initializer_list<const string*> ds) {
    for (const string* q : ds) if (chucNang == *q) return true;
    return false;
}
} // namespace

// ---- KhachHang ----
string KhachHang::vaiTro() const { return vaitro::KHACH_HANG; }
bool KhachHang::coQuyen(const string& chucNang) const {
    return thuocDanhSach(chucNang, { &quyen::DAT_HANG, &quyen::XEM_DON_CUA_MINH, &quyen::DUNG_VI });
}
unique_ptr<NguoiDung> KhachHang::clone() const { return make_unique<KhachHang>(*this); }

// ---- NguoiBan ----
string NguoiBan::vaiTro() const { return vaitro::NGUOI_BAN; }
bool NguoiBan::coQuyen(const string& chucNang) const {
    return thuocDanhSach(chucNang, { &quyen::XEM_DON_CUA_MINH, &quyen::SUA_SAN_PHAM_SHOP, &quyen::XEM_THONG_KE });
}
unique_ptr<NguoiDung> NguoiBan::clone() const { return make_unique<NguoiBan>(*this); }

// ---- ShipperVanChuyen ----
string ShipperVanChuyen::vaiTro() const { return vaitro::SHIPPER; }
bool ShipperVanChuyen::coQuyen(const string& chucNang) const {
    return thuocDanhSach(chucNang, { &quyen::XEM_DON_CUA_MINH, &quyen::DUNG_VI, &quyen::GIAO_HANG });
}
unique_ptr<NguoiDung> ShipperVanChuyen::clone() const { return make_unique<ShipperVanChuyen>(*this); }

// ---- QuanLy ----
string QuanLy::vaiTro() const { return vaitro::QUAN_LY; }
bool QuanLy::coQuyen(const string& chucNang) const {
    return thuocDanhSach(chucNang, { &quyen::XEM_TAT_CA_DON, &quyen::SUA_SAN_PHAM_SHOP, &quyen::QUAN_LY_SAN_PHAM,
                                     &quyen::QUAN_LY_KHUYEN_MAI, &quyen::QUAN_LY_VAN_CHUYEN, &quyen::QUAN_LY_TAI_KHOAN,
                                     &quyen::XEM_THONG_KE });
}
unique_ptr<NguoiDung> QuanLy::clone() const { return make_unique<QuanLy>(*this); }
