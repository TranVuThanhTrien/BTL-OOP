# Thư mục dữ liệu (file JSON không ghi chú được nên TODO nằm ở đây)

- `san_pham.json`: đã có 12 sản phẩm mẫu của 3 shop TT, DT, NS. TODO(C-4): viết `DataStore::docSanPham` đọc đúng file này (khóa: maSP, ten, maNguoiBan, donGia, khoiLuong, tonKho, daBan, danhGia).
- `don_hang.json`: đang rỗng. TODO(C-5): `luuDonHang` ghi vào đây. TODO(E-9): tạo 5–8 đơn mẫu ở nhiều trạng thái để demo (mỗi đơn có mảng `chiTiet`, `lichSu`, `diaChiGiao`, `yeuCauTra`, `anhGiaoHang`, `anhKhieuNai`, `tinNhan` (mảng {maNguoiGui, vaiTro, noiDung, thoiDiem}), `daHoanTien` (ảnh lưu file trong `data/anh/`, JSON chỉ giữ đường dẫn) và trường `thoiDiemKetThuc`, rỗng nếu chưa kết thúc).
- `tai_khoan.json`: chưa có. TODO(C-8): tạo khi viết `luuTaiKhoan`, gồm quản lý, 3 shop (TT, DT, NS) và vài khách; mật khẩu phải băm, ví có `soDu` và `lichSu`.
