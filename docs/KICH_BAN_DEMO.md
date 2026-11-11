# Kịch bản demo và hỏi đáp: Design Pattern

## Mục tiêu (5–7 phút)

Minh họa Strategy, Factory, State, Prototype và Composition trên luồng đặt hàng/giao hàng. Demo dùng API/console của dự án; các trạng thái và tên class lấy từ `include/` và `docs/KIEN_TRUC.md`.

## Chuẩn bị

- Chạy server từ thư mục `dt08-project`: `build-api/dt08.exe` (hoặc executable vừa build); mở `http://127.0.0.1:8080`.
- Tài khoản mẫu: khách `khach/123`; shop `TT/123`; shipper `GHN/123`; quản lý `quanly/admin`.
- Chọn một sản phẩm shop TT còn hàng. Dùng một đơn demo trạng thái Mới nếu dữ liệu seed đã được nạp.

## Kịch bản trình diễn

| Thời gian | Thao tác | Điểm cần nói |
|---|---|---|
| 0:00–0:40 | Giới thiệu mô hình domain: `DonHang` là aggregate root; dịch vụ điều phối, web gọi HTTP API | Domain không phụ thuộc JSON/HTTP; `DataStore` là lớp biên lưu trữ. |
| 0:40–1:40 | Khách xem hàng, thêm `AT01` vào giỏ, xem trước phí và chọn GHN/COD | `DonViVanChuyen` và `PhuongThucThanhToan` là các chiến lược thay thế. |
| 1:40–2:20 | Tạo đơn, mở danh sách đơn | Factory `taoDonVi`, `taoPhuongThuc`, `taoKhuyenMai` chọn implementation từ mã cấu hình; strategy tính phí/thanh toán/giảm giá. |
| 2:20–3:20 | Shop TT xác nhận; GHN nhận và chuyển đơn sang Đang giao | State object kiểm soát bước tiếp theo hợp lệ; lịch sử được ghi tại `DonHang::datTrangThai`. |
| 3:20–4:00 | Thử đánh dấu giao thành công khi chưa có ảnh: hệ thống từ chối; gửi ảnh rồi xác nhận | Bất biến nghiệp vụ nằm ở aggregate và service, không thể bỏ qua bằng giao diện. |
| 4:00–4:40 | Khách nhắn tin theo đơn; quay lại đơn sau khi đọc tin | Chat gắn với đơn, giới hạn nội dung và theo dõi tin chưa đọc. |
| 4:40–5:30 | Quản lý xem thống kê; kết thúc đơn Hoàn tất | Chỉ kết thúc ở trạng thái được phép; quyền được lọc ở service. |
| 5:30–6:00 | Kết luận, chỉ ra các class trên sơ đồ UML | Các pattern có mục đích riêng và ghép thành một luồng nghiệp vụ. |

## Câu hỏi hội đồng và gợi ý trả lời

1. **Vì sao vận chuyển dùng Strategy?** Mỗi hãng có thuật toán tính phí và ngày dự kiến khác nhau nhưng cùng giao diện; có thể chọn hãng lúc chạy mà `DonHangService` không cần nhánh công thức riêng.
2. **Factory giải quyết vấn đề gì?** Tập trung ánh xạ mã như `GHN`/`COD`/`GIAM10` sang object cụ thể, đồng thời báo lỗi mã không hợp lệ ở một nơi.
3. **State khác enum trạng thái thế nào?** State đóng gói hành vi chuyển tiếp, nhờ đó trạng thái hiện tại quyết định bước hợp lệ; tránh gom nhiều điều kiện chuyển trạng thái vào một hàm dài.
4. **Prototype được dùng ở đâu?** Các chiến lược và trạng thái có `clone()` để `DonHang` hỗ trợ copy constructor/assignment (Big 5) mà vẫn sao chép đúng đối tượng đa hình.
5. **Vì sao `DonHang` sở hữu chi tiết bằng `unique_ptr`?** Chi tiết là thành phần vòng đời của đơn; quyền sở hữu rõ ràng, hủy cùng aggregate và không chia sẻ sở hữu mơ hồ.
6. **Làm sao tránh đổi bảng phí làm thay đổi đơn cũ?** `DonHang::chotDon()` chụp phí giao tại thời điểm đặt; đơn đã chốt dùng `m_phiShip`.
7. **Làm sao đảm bảo giao hàng phải có ảnh?** `DonHangService::xacNhanGiao` kiểm tra quyền/đơn; domain chỉ chuyển Đang giao sang Hoàn tất khi có ảnh.
8. **Nếu thêm hãng vận chuyển thì sửa gì?** Thêm implementation theo giao diện `DonViVanChuyen`, cập nhật factory và bảng giá; nghiệp vụ gọi interface tiếp tục dùng đa hình.
9. **Giới hạn hiện tại?** Bảng giá và bật/tắt khuyến mãi là trạng thái trong bộ nhớ, reset khi khởi động lại; token chưa hết hạn. Đây là giới hạn được nêu trong `KIEN_TRUC.md`.

## Phương án dự phòng

Nếu API không chạy, dùng console để trình bày phần domain: đăng nhập khách, chọn sản phẩm/đặt đơn; sau đó dùng kiểm thử tự động để chứng minh chuyển trạng thái, lỗi thiếu ảnh và các quy tắc nghiệp vụ. Không tuyên bố đã trình diễn thao tác nào chưa chạy trực tiếp.

