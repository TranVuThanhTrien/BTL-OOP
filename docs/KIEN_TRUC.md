# Kiến trúc C++ ĐT08 – bản chốt v1

Tài liệu này là nguồn sự thật cho cả nhóm. Chữ ký hàm trong `include/*.h` là hợp đồng giữa các người; trong header, dòng `TODO(X-n)` cho biết việc của ai.

## 1. Các lớp và quy tắc include

```
Lớp 0  Common.h/.cpp  OrderException.h            (cả nhóm; đã viết xong)
Lớp 1  SanPham   TaiKhoanVi   DonViVanChuyen   TrangThaiDonHang(*)      chỉ include Common.h
Lớp 2  NguoiDung(+TaiKhoanVi)   GioHang(+SanPham)   PhuongThucThanhToan(+TaiKhoanVi)
Lớp 3  KhuyenMai(+GioHang)      KhoSanPham(+SanPham)
Lớp 4  DonHang   (gom tất cả lớp 1-3; aggregate root)
Lớp 5  DataStore (đọc/ghi file, duy nhất dùng json.hpp)
Lớp 6  AuthService   DonHangService   ThongKeService                      (nghiệp vụ điều phối)
Lớp 7  main.cpp (httplib + json: route, chuyển đổi JSON)   main_test.cpp
(*) TrangThaiDonHang chỉ forward-declare DonHang; TrangThaiDonHang.cpp mới include DonHang.h, nên không vòng include.
```

Quy tắc cứng:
1. Lớp thấp không bao giờ include lớp cao. Mỗi header phải biên dịch được khi include một mình (đã kiểm tra).
2. Chỉ `Common.h` được include thư viện chuẩn. Cần thêm thì sửa `Common.h`, báo nhóm. Không `using namespace std`.
3. `json.hpp` chỉ xuất hiện trong `DataStore.cpp` và `main.cpp`. `httplib.h` chỉ trong `main.cpp`. Domain và service không biết JSON, HTTP, token.
4. Không viết chuỗi/số nghiệp vụ cứng trong `.cpp`: dùng `hang::`, `vaitro::`, `pttt::`, `tt::`, `km::`, `hangso::`, `quyen::`.
5. Tiền là `double` đơn vị đồng, mọi kết quả đi qua `lamTronDong()`. Thời điểm là chuỗi `"YYYY-MM-DD HH:MM:SS"` từ `thoiGianHienTai()` (giờ máy chủ).
6. Sở hữu: `unique_ptr` cho mọi thứ polymorphic; lớp abstract có `virtual ~X()` và `clone()`. Con trỏ thô chỉ là con trỏ không sở hữu (`ViDienTu::m_vi`, `NguoiDung::vi()`).

## 2. Header dùng chung (`Common.h`)

Gồm ba phần: include chuẩn kèm các `using`; các hằng tên chuẩn (`hang`, `vaitro`, `pttt`, `tt`, `km`) và hằng nghiệp vụ (`hangso`); khai báo tiện ích cài trong `Common.cpp`: `thoiGianHienTai`, `inHoa/inThuong`, `lamTronDong`, `dinhDangTien`, `laMaShopHopLe`, `laSoDienThoaiHopLe`, `bamMatKhau/kiemTraBam`.
Các tên chuẩn khớp với web: tên trạng thái là `Mới, Xác nhận, Đang giao, Hoàn tất, Trả hàng` (mảng `TTS`), hãng là `GHN, GHTK, ViettelPost`. Chỉ có tên thanh toán là web phải đổi: web gửi `The`/`Vi`, C++ dùng `TheTinDung`/`ViDienTu` (E đổi khi nối API, E-6).
`OrderException` có thêm `loai()` (tên lớp, cho trường `loi` của JSON) và `maHttp()`: 400 mặc định, 401 `DangNhapThatBai`, 403 `KhongCoQuyen`, 500 `FileData`.

## 3. Các chỗ skeleton cũ mâu thuẫn hoặc thiếu, và cách đã chốt

| # | Vấn đề | Quyết định |
|---|---|---|
| 1 | `KhuyenMai` nhận `GioHang` nhưng `DonHang` chỉ có chi tiết, không tính lại được giảm giá | Thêm `TomTatGio`; `GioHang::tomTat()` và `DonHang::tomTat()` cùng tạo ra nó; `KhuyenMai` nhận `TomTatGio` |
| 2 | `DonHang` không có mã chủ đơn, nên không lọc được "khách chỉ thấy đơn của mình" | Thêm `m_maKhach` |
| 3 | `DonHang::hoanTien` và phí ship cho shipper cần ví của người khác | `DonHang` không giữ ví. Service truyền `TaiKhoanVi&`: `hoanTien(maShop, viKhach)`, `traPhiShip(viShipper)` (tự chống cộng 2 lần) |
| 4 | Shipper cũng cần ví, `NguoiDung` thiếu vai trò Shipper | `NguoiDung::vi()` ảo (khách và shipper có, còn lại `nullptr`), thêm `maShop()`, `maHang()` để khỏi `dynamic_cast`; thêm `vaitro::SHIPPER` |
| 5 | Bảng giá ship sửa được (D-11) nhưng 3 hãng viết công thức cứng; sửa giá sẽ đổi luôn đơn cũ | `BangGiaShip` sửa được, `DonViTheoBangGia` giữ công thức chung; `DonHang::chotDon()` chụp phí ship |
| 6 | Bật/tắt khuyến mãi (D-11) chưa có chỗ chứa | Trạng thái tĩnh trong `KhuyenMai.cpp`, `taoKhuyenMai` ném lỗi khi mã bị tắt. Không lưu file |
| 7 | Không có chỗ giữ danh sách sản phẩm, trong khi service trừ kho và quản lý CRUD | Thêm `KhoSanPham` (A) |
| 8 | `SanPham` chưa có trường `daBan`, `danhGia` dù JSON và web đã có | Thêm vào ctor và getter |
| 9 | `DiaChi` nằm trong `DonHang.h` nhưng `KhachHang`/`NguoiBan` cần | Chuyển xuống `NguoiDung.h` |
| 10 | Hai hàm `chuyenTrangThai` trong Service (D-2 và D-15) | Chỉ còn bản có `nguoiGoi` |
| 11 | Bảng quyền: header ghi người bán được Hoàn tất→Trả hàng, D-15 ghi chỉ Mới→Xác nhận | Theo D-15. Hoàn tất→Trả hàng chỉ xảy ra khi shipper `xacNhanGuiVeShop`; `HoanTat::chuyenTiep` chỉ cho qua khi `hangDaVeShop()` |
| 12 | D-16: `main.cpp` ghi shipper thấy đơn từ Xác nhận, TODO.md ghi từ Mới | Theo TODO.md: từ Mới (đơn Mới chỉ xem). Đã sửa comment |
| 13 | Ghi lịch sử trạng thái có thể bị ghi trùng (lớp trạng thái và DonHang) | Chỉ `DonHang::datTrangThai` ghi `m_lichSu` |
| 14 | Giỏ khách vãng lai không có token | Giỏ lưu theo mã giỏ (header `X-Gio-Hang`) trong `DonHangService`; đăng nhập xong vẫn dùng mã đó nên không mất giỏ |
| 15 | Thanh toán ví dùng con trỏ tới ví; đơn đọc từ file không có ví | `taoPhuongThuc(ten, viKhach)`; đơn đọc từ file có `m_vi = nullptr` và không được thanh toán lại. `AuthService` giữ `NguoiDung` trên heap nên địa chỉ ổn định |

## 4. Luồng chính

**Đặt hàng** (`DonHangService::taoDon`): lấy giỏ → kiểm tra tồn → dựng `DonHang` (`themChiTiet`, `datDiaChiGiao`) → `chotDon()` (kiểm tra khuyến mãi, chụp phí ship) → `thanhToan(tinhTongThanhToan())` → trừ kho và `congDaBan` → lưu file. Thanh toán đặt sau cùng trước khi trừ kho để lỗi không làm hỏng kho. `xemTruoc` dựng `DonHang` tạm và gọi cùng `tinhKetQua()`, nên số trên màn thanh toán luôn bằng số của đơn thật.

**Giao hàng:** Mới →(người bán)→ Xác nhận →(shipper)→ Đang giao →(shipper, `xacNhanGiao` kèm ảnh)→ Hoàn tất; ngay sau đó Service gọi `traPhiShip`.

**Trả hàng:** khách `yeuCauTraHang` → shop `hoanHang` hoặc `tuChoiTraHang` → shipper `nhanDonTra` → `xacNhanGuiVeShop` (đơn sang Trả hàng) → `hoanTien(maShop)` → `ketThucDon`.

**Phân quyền hai tầng:** Service kiểm tra vai trò (bảng quyền D-15 và `NguoiDung::coQuyen`); `DonHang` chỉ giữ bất biến nghiệp vụ (đúng trạng thái, đúng hãng, đủ ảnh).

## 5. Design pattern (cho phần thuyết trình)

Strategy: `DonViVanChuyen`, `PhuongThucThanhToan`, `KhuyenMai`. State: `TrangThaiDonHang`. Factory: `taoDonVi`, `taoPhuongThuc`, `taoKhuyenMai`, `taoTrangThai`. Prototype (`clone()`) phục vụ Big 5 của `DonHang`. Composition: `DonHang` sở hữu chi tiết, chiến lược, trạng thái.

## 6. Đa luồng và lưu trữ

`cpp-httplib` xử lý mỗi request trên một thread. Dùng một `std::mutex` toàn cục trong `main.cpp`, mọi handler `/api/*` khóa ngay đầu hàm. Mutex này bảo vệ cả `AuthService`, `DonHangService`, `KhoSanPham` và các biến tĩnh (khuyến mãi, bảng giá ship). Không tự khóa riêng trong từng lớp.
File: `san_pham.json`, `don_hang.json`, `tai_khoan.json`; ảnh trong `data/anh/`, JSON chỉ giữ đường dẫn. Ghi ra file tạm rồi đổi tên. Bật/tắt khuyến mãi và bảng giá ship không lưu: khởi động lại server là về mặc định.

## 7. Giới hạn đã biết (chấp nhận cho đồ án)

`bamMatKhau` là băm có muối tự viết, chỉ để không lưu mật khẩu thuần; C có thể thay bằng SHA-256 mà không đổi chữ ký. Hoàn tiền tính trên tiền hàng của shop, chưa chia phần giảm giá. Thời điểm lấy theo giờ máy chủ. Token không hết hạn.
