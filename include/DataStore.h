#pragma once
#include "DonHang.h"
#include "KhoSanPham.h"
// Người phụ trách: C | nhánh feature/payment-storage | LỚP 5 | CHỈ #include "json.hpp" trong DataStore.cpp (không đưa json vào header nào)
// Mọi hàm: file không mở/ghi được hoặc JSON hỏng -> FileDataException (kèm tên file). Ghi file theo kiểu ghi ra file tạm rồi rename, tránh hỏng file khi crash.
// Khóa JSON của từng đối tượng: xem data/README.md. Thời điểm: chuỗi "YYYY-MM-DD HH:MM:SS" (thoiGianHienTai()).
class DataStore {
public:
    static vector<SanPham> docSanPham(const string& duongDan);               // TODO(C-4) data/san_pham.json
    static void luuSanPham(const vector<SanPham>& ds, const string& duongDan);   // TODO(C-4)
    static vector<DonHang> docDonHang(const string& duongDan);               // TODO(C-5) data/don_hang.json; dựng lại đơn bằng friend (taoTrangThai, taoDonVi, taoPhuongThuc(ten, nullptr), taoKhuyenMai)
    static void luuDonHang(const vector<DonHang>& ds, const string& duongDan);   // TODO(C-5)
    static vector<unique_ptr<NguoiDung>> docTaiKhoan(const string& duongDan);    // TODO(C-8) data/tai_khoan.json
    static void luuTaiKhoan(const vector<unique_ptr<NguoiDung>>& ds, const string& duongDan);   // TODO(C-8): lưu m_matKhauBam, KHÔNG BAO GIỜ lưu mật khẩu thuần; kèm ví (soDu, lichSu)
    // Nhận ảnh base64 từ web (D-17/D-18), ghi file vào data/anh/ và trả đường dẫn tương đối để lưu vào AnhDinhKem::duongDan.
    static string luuAnhBase64(const string& thuMucAnh, const string& tienTo, const string& base64);   // TODO(C-5) giới hạn ~5MB, chỉ nhận png/jpg
};
