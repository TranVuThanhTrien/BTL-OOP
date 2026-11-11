#include "NguoiDung.h"

// Người phụ trách: A | nhánh feature/core-domain | LỚP 2

string KhachHang::vaiTro() const {
    return vaitro::KHACH_HANG;
}

bool KhachHang::coQuyen(const string& chucNang) const {
    return chucNang == quyen::DAT_HANG ||
           chucNang == quyen::XEM_DON_CUA_MINH ||
           chucNang == quyen::DUNG_VI;
}

unique_ptr<NguoiDung> KhachHang::clone() const {
    return make_unique<KhachHang>(*this);
}

string NguoiBan::vaiTro() const {
    return vaitro::NGUOI_BAN;
}

bool NguoiBan::coQuyen(const string& chucNang) const {
    return chucNang == quyen::XEM_DON_CUA_MINH ||
           chucNang == quyen::SUA_SAN_PHAM_SHOP ||
           chucNang == quyen::XEM_THONG_KE;
}

unique_ptr<NguoiDung> NguoiBan::clone() const {
    return make_unique<NguoiBan>(*this);
}

string ShipperVanChuyen::vaiTro() const {
    return vaitro::SHIPPER;
}

bool ShipperVanChuyen::coQuyen(const string& chucNang) const {
    return chucNang == quyen::XEM_DON_CUA_MINH ||
           chucNang == quyen::DUNG_VI ||
           chucNang == quyen::GIAO_HANG;
}

unique_ptr<NguoiDung> ShipperVanChuyen::clone() const {
    return make_unique<ShipperVanChuyen>(*this);
}

string QuanLy::vaiTro() const {
    return vaitro::QUAN_LY;
}

bool QuanLy::coQuyen(const string& chucNang) const {
    return chucNang == quyen::XEM_TAT_CA_DON ||
           chucNang == quyen::XEM_DON_CUA_MINH ||
           chucNang == quyen::SUA_SAN_PHAM_SHOP ||
           chucNang == quyen::QUAN_LY_SAN_PHAM ||
           chucNang == quyen::QUAN_LY_KHUYEN_MAI ||
           chucNang == quyen::QUAN_LY_VAN_CHUYEN ||
           chucNang == quyen::QUAN_LY_TAI_KHOAN ||
           chucNang == quyen::XEM_THONG_KE;
}

unique_ptr<NguoiDung> QuanLy::clone() const {
    return make_unique<QuanLy>(*this);
}
