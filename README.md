# ĐT08 - hệ thống quản lý đơn hàng và giao vận (BTL OOP)

Backend C++17 + giao diện web HTML/CSS/JavaScript thuần. Phân quyền người dùng bằng **đa hình** (lớp trừu tượng `NguoiDung`), đơn hàng có đủ **Big 5** (copy sâu, move), lỗi nghiệp vụ báo bằng **exception riêng**. Dữ liệu lưu JSON trong `data/`.

## Chạy
Cần CMake 3.10+ và trình biên dịch C++17.
```
cmake -S . -B build
cmake --build build
./build/dt08_test             # kiểm thử
./build/dt08                  # rồi mở địa chỉ chương trình in ra trong trình duyệt
```
Chạy từ thư mục gốc dự án (chương trình đọc `./data` và `./web`). Windows MinGW: `cmake -S . -B build -G "MinGW Makefiles"`, chạy `.\build\dt08.exe`. Terminal tiếng Việt: `chcp 65001`.
Target `dt08_test` bật AddressSanitizer (`-fsanitize=address`) trên GCC/Clang ngoài Windows; MinGW không hỗ trợ nên cờ này tự tắt.

## Tài khoản
Mật khẩu của mọi tài khoản mẫu là `123456` (trong file chỉ lưu giá trị băm).
| Tên đăng nhập | Vai trò | Ghi chú |
|---|---|---|
| khach01 | Khách hàng | ví có sẵn 500.000 đ |
| TT, DT, NS | Người bán | Shop Thời Trang, Điện Tử, Nông Sản (tên đăng nhập = mã shop) |
| GHN, GHTK, VTP | Shipper | Giao Hàng Nhanh, Giao Hàng Tiết Kiệm, Viettel Post |
| admin | Quản lý | |

## Nghiệp vụ
- 4 vai trò, mỗi vai trò có quyền riêng: khách đặt hàng và dùng ví; người bán sửa sản phẩm shop mình và xem thống kê shop mình; shipper chỉ đụng tới đơn của hãng mình; quản lý xem mọi đơn, quản lý sản phẩm, khuyến mãi, vận chuyển, tài khoản.
- Đặt hàng kiểm tra tồn kho (hết hàng báo `HetHang`), áp mã khuyến mãi (mã sai báo `MaGiamGiaKhongHopLe`), tính phí ship theo hãng.
- 3 hãng vận chuyển (GHN, GHTK, ViettelPost), mỗi hãng có bảng giá và số ngày giao ước tính riêng.
- 3 loại khuyến mãi.
- Vòng đời đơn: từ *Mới* đến *Trả hàng*. Chuyển sai luồng báo `ChuyenTrangThaiSai`; đơn đã *Trả hàng* không chuyển tiếp được.
- Ví điện tử cho khách và shipper: nạp tiền trong hạn mức, trừ tiền khi thanh toán, hoàn tiền cho khách, cộng phí ship cho shipper (đúng một lần mỗi đơn). Mọi giao dịch ghi vào lịch sử ví. Thanh toán không đủ số dư hoặc nạp sai hạn mức báo `ThanhToanThatBai`.
- Thống kê theo shop và toàn hệ thống.
- Lưu và đọc JSON qua `DataStore`; ghi ra file tạm rồi đổi tên để không hỏng dữ liệu. Lỗi file báo `FileData`.
- Mật khẩu chỉ lưu dạng băm (muối + băm), không lưu mật khẩu thuần.

## Cấu trúc
```
include/      header: Common, NguoiDung, AuthService, SanPham, KhoSanPham, GioHang,
              DonHang, TrangThaiDonHang, DonHangService, DonViVanChuyen, KhuyenMai,
              TaiKhoanVi, PhuongThucThanhToan, DataStore, ThongKeService, OrderException
src/          cài đặt (.cpp), main.cpp (ứng dụng), main_test.cpp (kiểm thử)
data/         san_pham.json, don_hang.json, tai_khoan.json
docs/         KIEN_TRUC.md (thiết kế chi tiết)
web/          index.html (giao diện)
third_party/  json.hpp (nlohmann/json v3.11.3), httplib.h (cpp-httplib)
```

## Kiểm thử
`dt08_test` kiểm tra: Big 5 của `DonHang` (sửa bản sao không ảnh hưởng bản gốc, nguồn rỗng sau khi move); 5 exception (`HetHang`, `MaGiamGiaKhongHopLe`, `ChuyenTrangThaiSai`, `ThanhToanThatBai`, `FileData`); luồng trạng thái từ *Mới* đến *Trả hàng*; 3 hãng ship, 3 khuyến mãi, thống kê.

## API
Phần này điền sau khi hoàn thành server (`main.cpp`): liệt kê các đường dẫn GET/POST và cách gửi token đăng nhập.

## Giới hạn
Thanh toán là mô phỏng (không thu tiền thật). Hàm băm mật khẩu chỉ đủ cho bài tập, không dùng cho hệ thống thật. Thiếu `third_party/json.hpp` thì `DataStore` biên dịch được nhưng không đọc ghi được JSON.
