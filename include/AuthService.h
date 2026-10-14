#pragma once
#include "NguoiDung.h"
// Người phụ trách: D | nhánh feature/web-api | Đăng nhập và phân quyền
class AuthService {
    vector<unique_ptr<NguoiDung>> m_dsNguoiDung;
    map<string,string> m_phien;                      // token -> tên đăng nhập
public:
    // TODO(D-9): string dangNhap(const string& user, const string& pass);   // trả token; sai/bị khóa -> DangNhapThatBaiException
    // TODO(D-9): void dangXuat(const string& token);
    // TODO(D-9): const NguoiDung& xacThuc(const string& token) const;       // token sai -> DangNhapThatBaiException
    // TODO(D-9): void dangKyKhachHang(const string& hoTen, const string& user, const string& pass);
    // TODO(D-9): void dangKyNguoiBan(const string& tenShop, const string& maShop, const string& pass);   // maShop 2-4 chữ cái, tự viết hoa, duy nhất, là tên đăng nhập
    // TODO(D-9): tên đăng nhập không phân biệt hoa thường (dt và DT là một)
    // TODO(D-10): void yeuCauQuyen(const NguoiDung& nd, const string& chucNang) const;   // thiếu quyền -> KhongCoQuyenException
    // TODO(D-10): void khoaTaiKhoan(const NguoiDung& nguoiGoi, const string& user, bool khoa);   // chỉ QuanLy
};
