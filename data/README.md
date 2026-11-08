# Thư mục dữ liệu

- `san_pham.json`: 12 sản phẩm mẫu thuộc TT, DT, NS. `DataStore::docSanPham` và `luuSanPham` đọc/ghi file JSON (C-4 đã hoàn thành).
- `don_hang.json`: đơn hàng do ứng dụng tạo được lưu đầy đủ, gồm chi tiết, trạng thái, địa chỉ, ảnh, trả hàng, chat và thời điểm kết thúc (C-5 đã hoàn thành). E-9 đã hoàn thành: hiện có 8 đơn (đơn gốc và 7 đơn demo) trải trên Mới, Xác nhận, Đang giao, Hoàn tất và Trả hàng. Các đơn demo có mã DH-DEMO-002 đến DH-DEMO-008; giữ chúng tách biệt khi dùng dữ liệu cho demo.
- `tai_khoan.json`: được tạo khi khởi động nếu chưa có; tài khoản demo có mật khẩu băm, ví và lịch sử giao dịch (C-8 đã hoàn thành).
