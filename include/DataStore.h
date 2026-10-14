#pragma once
#include "DonHang.h"
// Người phụ trách: C | nhánh feature/payment-storage | cần third_party/json.hpp (nlohmann)
class DataStore {
public:
    // TODO(C-4): static vector<SanPham> docSanPham(const string& duongDan);          // data/san_pham.json
    // TODO(C-4): static void luuSanPham(const vector<SanPham>& ds, const string& duongDan);
    // TODO(C-5): static void luuDonHang(const vector<DonHang>& ds, const string& duongDan);   // data/don_hang.json
    // TODO(C-5): static vector<DonHang> docDonHang(const string& duongDan);          // file lỗi -> FileDataException
    // TODO(C-8): static vector<unique_ptr<NguoiDung>> docTaiKhoan(const string& duongDan);   // data/tai_khoan.json
    // TODO(C-8): static void luuTaiKhoan(...);   // lưu mật khẩu đã băm, KHÔNG lưu mật khẩu thuần
};
