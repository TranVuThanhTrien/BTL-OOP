# TODO ĐT08 – checklist theo từng người

Mỗi dòng `TODO(X-n)` trong code tương ứng một mục dưới đây. Làm xong thì tick `[x]` và commit theo dạng `feat: ...` / `fix: ...` trên nhánh của mình.
Chuẩn: C++17, tách .h/.cpp, `unique_ptr`, lớp abstract có `virtual ~X()` và `clone()`, không rò rỉ bộ nhớ.
Tài khoản demo: khach/123, TT/123 (Thời Trang), DT/123 (Điện Tử), NS/123 (Nhà Sách), GHN/123, GHTK/123, VTP/123 (shipper), quanly/admin. Giao diện mẫu (3 vai trò: khách hàng, người bán, quản lý) đã làm sẵn trong `web/index.html` (dữ liệu mô phỏng); E chỉ cần nối API thật.

## Kiến trúc đã chốt (v1) – ĐỌC `docs/KIEN_TRUC.md` TRƯỚC KHI CODE
Header đã được chốt: chữ ký hàm trong `include/*.h` là HỢP ĐỒNG giữa các thành viên. Muốn đổi chữ ký phải báo nhóm và sửa header trên develop, không tự đổi trong nhánh riêng.
Các mục dưới đây là thay đổi so với skeleton ban đầu (không đánh số lại các TODO cũ):
- Thêm `src/Common.cpp` (tiện ích dùng chung: thoiGianHienTai, dinhDangTien, bamMatKhau...): đã viết xong, cả nhóm dùng.
- Thêm `KhoSanPham` (A): nơi duy nhất giữ danh sách sản phẩm; tìm/lọc/sắp xếp cho D-21. Mã việc: A-23.
- `KhuyenMai` nhận `TomTatGio` thay vì `GioHang` (để DonHang tính lại giảm giá từ chi tiết). Bật/tắt khuyến mãi và bảng giá ship là trạng thái tĩnh trong KhuyenMai.cpp / DonViVanChuyen.cpp (không lưu file). Mã việc: A-7, B-3.
- `DonHang` có thêm `maKhach`, `chotDon()` (chụp phí ship), `traPhiShip(ví)`, `hoanTien(maShop, ví khách)`; Service gọi, DonHang không tự giữ ví người khác.
- `OrderException` đã xong: có `loai()` và `maHttp()` (C-1 chỉ còn phần test).

## Bản đồ file: ai sửa file nào, có bao nhiêu TODO
Mọi file code đều có TODO. Chỉ 4 file không có: 2 file cấu hình/giữ chỗ và 2 file dữ liệu JSON (JSON không ghi chú được nên TODO của chúng nằm trong `data/README.md`).

| File | Người phụ trách | Số TODO | Ghi chú |
|---|---|---|---|
| `.gitignore` | – | 0 | Cấu hình Git, làm sẵn, không cần viết thêm |
| `CMakeLists.txt` | D (server), C (test) | 2 | cần cài đặt |
| `data/README.md` | – | 4 | cần cài đặt |
| `data/don_hang.json` | – | 0 | Đang rỗng. JSON không ghi chú được nên TODO (C-5, E-9) nằm trong data/README.md |
| `data/san_pham.json` | – | 0 | Dữ liệu mẫu có sẵn. JSON không ghi chú được nên TODO nằm trong data/README.md |
| `docs/.gitkeep` | – | 0 | Giữ thư mục rỗng, chưa có gì để làm |
| `include/AuthService.h` | D | 8 | cần cài đặt |
| `include/Common.h` | cả nhóm | 1 | cần cài đặt |
| `include/DataStore.h` | C | 6 | cần cài đặt |
| `include/DonHang.h` | A | 25 | cần cài đặt |
| `include/DonHangService.h` | D | 12 | cần cài đặt |
| `include/DonViVanChuyen.h` | B | 6 | cần cài đặt |
| `include/GioHang.h` | A | 8 | cần cài đặt |
| `include/KhuyenMai.h` | A | 4 | cần cài đặt |
| `include/NguoiDung.h` | A | 6 | cần cài đặt |
| `include/OrderException.h` | C | 1 | cần cài đặt |
| `include/PhuongThucThanhToan.h` | C | 4 | cần cài đặt |
| `include/SanPham.h` | A | 4 | cần cài đặt |
| `include/TaiKhoanVi.h` | C | 5 | cần cài đặt |
| `include/ThongKeService.h` | D | 4 | cần cài đặt |
| `include/TrangThaiDonHang.h` | B | 5 | cần cài đặt |
| `src/AuthService.cpp` | D | 1 | cần cài đặt |
| `src/DataStore.cpp` | C | 1 | cần cài đặt |
| `src/DonHang.cpp` | A | 1 | cần cài đặt |
| `src/DonHangService.cpp` | D | 1 | cần cài đặt |
| `src/DonViVanChuyen.cpp` | B | 1 | cần cài đặt |
| `src/GioHang.cpp` | A | 1 | cần cài đặt |
| `src/KhuyenMai.cpp` | A | 1 | cần cài đặt |
| `src/NguoiDung.cpp` | A | 1 | cần cài đặt |
| `src/PhuongThucThanhToan.cpp` | C | 1 | cần cài đặt |
| `src/SanPham.cpp` | A | 1 | cần cài đặt |
| `src/TaiKhoanVi.cpp` | C | 1 | cần cài đặt |
| `src/ThongKeService.cpp` | D | 1 | cần cài đặt |
| `src/TrangThaiDonHang.cpp` | B | 1 | cần cài đặt |
| `src/main.cpp` | D | 18 | cần cài đặt |
| `src/main_test.cpp` | C | 4 | cần cài đặt |
| `third_party/README.txt` | D và C | 2 | cần cài đặt |
| `web/index.html` | E | 2 | cần cài đặt |

## A – feature/core-domain (Leader, thuyết trình 1)
- [x] A-1 SanPham: getter/setter, kiểm tra dữ liệu (giá > 0, kg > 0, tồn >= 0)
- [x] A-2 SanPham: giamTon / tangTon (ném HetHangException)
- [x] A-3 NguoiDung (abstract: vaiTro, coQuyen, clone) + KhachHang (có TaiKhoanVi), NguoiBan (mã shop TT, DT, NS = tên đăng nhập), QuanLy
- [x] A-4 GioHang: themSanPham, capNhatSoLuong, xoaSanPham, lamTrong
- [x] A-5 GioHang: tinhTongTien, tinhTongKhoiLuong, demNguoiBan, tongTheoNguoiBan
- [x] A-6 KMPhanTram (GIAM10), KMGiamTien (GIAM30K), KMFreeShip (FREESHIP)
- [x] A-7 taoKhuyenMai(ma): factory, mã sai ném MaGiamGiaKhongHopLeException
- [x] A-8 ChiTietDonHang: thành tiền, khối lượng, getter
- [x] A-9 DonHang: Big 5 (destructor, copy ctor, operator=, move ctor, move assign noexcept) dùng clone()
- [x] A-10 DonHang: operator==, operator<<
- [x] A-11 DonHang: tinhTienHang, tinhGiamGia, tinhTongThanhToan
- [x] A-12 DonHang: chuyenTrangThai, datTrangThai
- [x] A-14 DonHang: ketThucDon, daKetThuc (chỉ sau Hoàn tất hoặc Trả hàng); đơn đã kết thúc thì không đổi trạng thái được nữa
- [x] A-15 ShipperVanChuyen (kế thừa NguoiDung, giữ mã hãng GHN, GHTK hoặc ViettelPost; coQuyen chỉ cho đơn của hãng mình)
- [x] A-16 Địa chỉ: DiaChi giao của khách (nhập khi thanh toán, lưu lại cho lần sau), DiaChi lấy hàng của shop (người bán tự sửa)
- [x] A-17 Trả hàng: yeuCauTraHang (khách, đơn Hoàn tất), hoanHang (shop chấp nhận trả, chờ shipper lấy hàng trả; chưa sang Trả hàng), tuChoiTraHang, hoanTien(maShop) hoàn tiền hàng vào ví khách
- [x] A-18 Ảnh đính kèm: themAnhGiaoHang (shipper gửi khách), themAnhKhieuNai (khách gửi shop)
- [x] A-19 Xác nhận giao thành công bắt buộc có ảnh: coAnhGiaoHang, xacNhanGiaoThanhCong(ảnh); thiếu ảnh thì không sang Hoàn tất (quản lý cũng không bỏ qua); phí ship chỉ cộng cho shipper sau bước này
- [x] A-20 Luồng trả hàng qua shipper: shipperNhanDonTra (đúng hãng) rồi shipperXacNhanGuiVeShop mới chuyển sang Trả hàng; không kết thúc đơn khi trả hàng đang xử lý
- [x] A-21 Chat theo đơn: TinNhan, guiTinNhan (chỉ người thuộc đơn, tối đa 500 ký tự), soTinChuaDoc
- [x] A-22 SanPham: daBan, danhGia, congDaBan để hiển thị và sắp xếp Bán chạy, Phổ biến
- [x] A-23 KhoSanPham: tim (tuKhoa, maShop, sapXep), lay, them, xoa, nhapKho; GioHang::tomTat
- [ ] A-13 Quản lý repo, Jira; review và merge PR; 28/10 merge main, tag v1.0

## B – feature/design-patterns (thuyết trình 2)
- [ ] B-1 GHN, GHTK, ViettelPost: tinhPhiShip, uocTinhNgay, ten, clone
- [ ] B-2 Kiểm tra km <= 0 hoặc kg <= 0
- [ ] B-3 taoDonVi(ten) factory; timHangReNhat(km, kg)
- [ ] B-4 5 lớp trạng thái DonMoi, DaXacNhan, DangGiao, HoanTat, TraHang; TraHang là trạng thái cuối
- [ ] B-5 Lịch sử chuyển trạng thái (thời điểm, trạng thái) phục vụ tracking
- [ ] B-6 taoTrangThai(ten) khi đọc file
- [ ] B-8 DangGiao::chuyenTiep chỉ sang HoanTat khi đơn đã có ảnh giao hàng
- [ ] B-7 Kịch bản demo + bộ câu hỏi hội đồng cho phần Pattern

## C – feature/payment-storage (báo cáo 1)
- [ ] C-1 Exception: đẩy OrderException.h lên develop ngay tuần 1; hàm lấy tên lớp exception cho JSON
- [ ] C-2 COD, TheTinDung (phí 1,5%), ViDienTu (trừ số dư, napTien)
- [ ] C-3 taoPhuongThuc(ten) factory
- [ ] C-4 DataStore: docSanPham, luuSanPham (data/san_pham.json)
- [ ] C-5 DataStore: luuDonHang, docDonHang (data/don_hang.json), lỗi file ném FileDataException
- [ ] C-6 main_test.cpp: Big 5, 5 exception, luồng trạng thái, ship, khuyến mãi, thống kê
- [ ] C-7 Kiểm tra rò rỉ bộ nhớ (valgrind hoặc -fsanitize=address); viết Chương 1, Chương 4
- [ ] C-8 DataStore: docTaiKhoan, luuTaiKhoan (data/tai_khoan.json), băm mật khẩu, không lưu mật khẩu thuần
- [ ] C-9 Test DangNhapThatBaiException và KhongCoQuyenException (lớp đã có sẵn trong OrderException.h)
- [ ] C-10 TaiKhoanVi: soDu, lichSu, napTien (10.000 đến 20.000.000đ), truTien; ViDienTu gọi truTien khi thanh toán
- [ ] C-11 TaiKhoanVi::hoanTien cho tiền hoàn của shop; shipper dùng chung TaiKhoanVi để nhận phí ship khi đơn Hoàn tất (cộng đúng 1 lần)

## D – feature/web-api (báo cáo 2)
- [ ] D-1 DonHangService: taoDon, xemTruoc
- [ ] D-2 DonHangService: chuyenTrangThai, layDanhSach, layChiTiet
- [ ] D-3 ThongKeService: tyLeGiaoThanhCong, tyLeHoanHang
- [ ] D-4 ThongKeService: doanhThuTheoNguoiBan, demTheoHang
- [ ] D-5 Server cpp-httplib: mount thư mục web/, listen 8080
- [ ] D-6 10 route: GET /api/san-pham · POST /api/gio-hang/them · GET /api/gio-hang · POST /api/gio-hang/khuyen-mai · POST /api/don-hang/xem-truoc · POST /api/don-hang · GET /api/don-hang · GET /api/don-hang/{id} · POST /api/don-hang/{id}/chuyen-trang-thai · GET /api/thong-ke
- [ ] D-7 Bắt OrderException, trả HTTP 400 kèm { "loi", "thongBao" }
- [ ] D-8 Viết Chương 3, Lời nói đầu, Kết luận
- [ ] D-9 AuthService: dangNhap (trả token), dangXuat, xacThuc, dangKyKhachHang
- [ ] D-10 Phân quyền: yeuCauQuyen, khoaTaiKhoan (chỉ QuanLy); khách chỉ thấy đơn của mình, người bán chỉ thấy shop mình
- [ ] D-11 Route đăng nhập, đăng ký, GET /api/toi và nhóm /api/quan-ly/* (sản phẩm CRUD, bật/tắt khuyến mãi, sửa bảng giá ship, tài khoản)
- [ ] D-12 Route ví: GET /api/vi, POST /api/vi/nap-tien, GET /api/vi/giao-dich; đăng ký có vaiTro (KhachHang hoặc NguoiBan), dangKyNguoiBan kiểm tra mã shop 2-4 chữ cái
- [ ] D-13 Route POST /api/don-hang/{id}/ket-thuc; JSON đơn thêm daKetThuc và thoiDiemKetThuc
- [ ] D-14 Chế độ khách vãng lai: route xem sản phẩm, giỏ hàng, xem-truoc không cần token; đặt hàng, ví, quản lý cần token (thiếu token trả 401); giữ giỏ hàng khi khách đăng nhập
- [ ] D-15 Bảng quyền chuyển trạng thái: người bán (chỉ Mới sang Xác nhận; trả hàng đi qua luồng yêu cầu trả hàng), shipper đúng hãng (Xác nhận sang Đang giao, Đang giao sang Hoàn tất, chỉ khi kèm ảnh), quản lý mọi bước trước Hoàn tất, khách không được; sai thì ném KhongCoQuyenException
- [ ] D-16 Route GET /api/giao-hang cho shipper: MỌI đơn của hãng mình từ trạng thái Mới (đơn Mới chỉ xem, chưa có nút), kèm địa chỉ giao, địa chỉ lấy hàng, tiền thu hộ khi COD và phí ship được nhận
- [ ] D-17 Route trả hàng, hoàn hàng, từ chối, hoàn tiền, gửi ảnh giao hàng, gửi ảnh khiếu nại (ảnh gửi base64, server lưu file, trả đường dẫn)
- [ ] D-18 Route POST /api/don-hang/{id}/xac-nhan-giao kèm ảnh (base64): chỉ shipper đúng hãng, đơn đang giao; route chuyen-trang-thai chung từ chối bước Đang giao sang Hoàn tất khi chưa có ảnh
- [ ] D-21 Route GET /api/san-pham hỗ trợ tìm kiếm, lọc theo shop, sắp xếp; trả thêm daBan và danhGia
- [ ] D-19 Route nhan-don-tra và xac-nhan-gui-ve-shop cho shipper đúng hãng
- [ ] D-20 Route chat theo đơn: danh sách hội thoại kèm số tin chưa đọc, đọc và gửi tin nhắn; người ngoài đơn bị 403

## E – feature/frontend (slide + biểu đồ)
- [ ] E-1 Màn Sản phẩm: lưới card, tìm kiếm, lọc theo nhà bán, hiển thị tồn kho và hết hàng
- [ ] E-2 Màn Giỏ hàng: nhóm theo nhà bán, tăng/giảm/xoá, ô nhập mã giảm giá
- [ ] E-3 Màn Thanh toán: chọn khoảng cách, so sánh phí 3 hãng (nhãn "Rẻ nhất"), chọn thanh toán, hoá đơn
- [ ] E-4 Màn Đơn hàng: bảng có badge trạng thái, timeline tracking, nút chuyển trạng thái
- [ ] E-5 Màn Thống kê: 2 tỷ lệ, doanh thu theo nhà bán, số đơn theo hãng
- [ ] E-6 Nối API thật: thay các hàm trong khối "Api mô phỏng" của web/index.html bằng fetch()
- [ ] E-7 UML: Class Diagram (khớp code), Use Case, 2 Sequence
- [ ] E-8 Slide PPT, biểu đồ; Chương 2; khung LaTeX
- [ ] E-9 Dữ liệu mẫu 10–15 sản phẩm của 3 nhà bán, 5–8 đơn ở nhiều trạng thái (data/)
- [ ] E-10 Màn Đăng nhập / Đăng ký, menu theo vai trò, đăng xuất (web/index.html đã có mẫu)
- [ ] E-11 Khu vực Người bán và Quản lý: Tổng quan (KPI, việc cần làm), Đơn hàng (lọc, tìm kiếm, chuyển trạng thái), Sản phẩm (thêm, sửa, xóa, nhập kho)
- [ ] E-12 Chỉ Quản lý: Khuyến mãi (bật/tắt), Vận chuyển (sửa bảng giá), Tài khoản (khóa/mở khóa)
- [ ] E-13 Màn Ví của tôi (số dư, nạp tiền, lịch sử giao dịch) và đăng ký mở shop với mã shop riêng (web/index.html đã có mẫu)
- [ ] E-14 Nút "Kết thúc đơn hàng" (hiện khi đơn Hoàn tất hoặc Trả hàng, ở màn Đơn của tôi và Đơn hàng), nhãn "Đã kết thúc", mục cuối trong timeline
- [ ] E-15 Giỏ hàng nổi góc phải (số món, tạm tính, danh sách món có +/−, nút Thanh toán) chỉ hiện với khách hàng, ẩn ở màn Giỏ hàng và Thanh toán; bấm logo để về trang chủ (khách: Sản phẩm, người bán/quản lý: Tổng quan)
- [ ] E-16 Xem web trước, đăng nhập sau: mở web vào thẳng trang Sản phẩm với tư cách khách (header có nút Đăng nhập, Đăng ký); khách xem sản phẩm, thêm giỏ, xem thanh toán; bấm "Đăng nhập để đặt hàng" thì hiện màn đăng nhập, đăng nhập xong quay lại trang thanh toán với giỏ hàng còn nguyên; đăng xuất thì về chế độ khách
- [ ] E-17 Giao diện shipper: màn "Đơn giao hàng" gồm 3 thẻ số liệu (chờ nhận, đang giao, đã giao), bảng đơn của hãng mình có cột thu hộ COD, nút Nhận giao hàng / Giao thành công; màn "Đơn của tôi" của khách hiện dòng chờ (chờ người bán, chờ shipper, đang giao); nút chuyển trạng thái chỉ hiện đúng với từng vai trò
- [ ] E-18 Form địa chỉ giao hàng khi thanh toán (họ tên, số điện thoại, địa chỉ), giữ nguyên khi khách phải đăng nhập giữa chừng; người bán sửa địa chỉ shop ở màn Sản phẩm; chi tiết đơn hiện địa chỉ giao và địa chỉ lấy hàng từng shop
- [ ] E-19 Nút Yêu cầu trả hàng (khách, kèm lý do), Hoàn hàng và Từ chối (shop), Hoàn tiền cho khách khi hàng lỗi (shop, quản lý); nhãn "Chờ duyệt trả hàng"
- [ ] E-20 Gửi ảnh: shipper gửi ảnh tình trạng hàng cho khách sau khi giao, khách gửi ảnh khiếu nại kèm mô tả cho shop; hiển thị ảnh trong chi tiết đơn
- [ ] E-21 Màn shipper hiện mọi đơn của hãng từ lúc khách đặt, thẻ "Phí ship đã nhận", cột phí ship và thu hộ
- [ ] E-22 Ghi chú theo vai trò và trạng thái (khách, người bán, shipper, quản lý); mục "Việc cần làm" chỉ gồm đơn chờ xác nhận và yêu cầu trả hàng
- [ ] E-23 Lỗi đã gặp: không dùng id trùng giữa các màn hình (id="od" ở 3 màn làm chi tiết đơn ghi nhầm vào màn ẩn khi đổi vai trò); dùng class hoặc id riêng cho từng màn
- [ ] E-24 Đơn Đang giao: shipper không có nút "Giao thành công", thay bằng nút "Chụp ảnh đơn hàng để xác nhận giao thành công" (input file có capture="environment" để mở camera sau trên điện thoại); chọn ảnh xong mới chuyển Hoàn tất; quản lý không có nút bỏ qua bước này; nút gửi thêm ảnh vẫn còn sau khi giao
- [ ] E-25 Khách bấm "Yêu cầu trả hàng" thì hiện hộp xác nhận (lý do, mô tả, ảnh đính kèm tuỳ chọn, nút "Xác nhận gửi" và "Huỷ"); sau khi gửi hiện ngay dòng xanh "Bạn đã gửi yêu cầu trả hàng lúc ..." và "Bạn đã gửi khiếu nại kèm n ảnh"; nhãn trạng thái trả hàng trong danh sách đơn
- [ ] E-26 Shipper xử lý đơn trả: đơn hiện nhãn "Chờ shipper lấy hàng trả", địa chỉ đảo chiều (lấy ở khách, gửi về shop), nút "Nhận đơn trả hàng" rồi "Xác nhận đã gửi về shop"; thẻ "Đơn trả cần xử lý"
- [ ] E-27 Khung chat theo đơn cho mọi vai trò (trừ khách vãng lai): menu "Tin nhắn" có số tin chưa đọc, danh sách hội thoại bên trái, khung chat bên phải, Enter để gửi, nút "Nhắn tin về đơn này" trong chi tiết đơn; nội dung tin nhắn phải thoát ký tự HTML
- [ ] E-28 Giao diện khách hàng lấy ý tưởng từ sàn thương mại (không dùng logo, tên hay hình của hãng nào): header màu cam có ô tìm kiếm lọc ngay khi gõ, biểu tượng giỏ kèm số món; banner tự chuyển 3 slide có chấm điều hướng; dải voucher có nút "Lưu mã"; danh mục tròn theo shop; thanh sắp xếp (Phổ biến, Mới nhất, Bán chạy, Giá tăng/giảm); thẻ sản phẩm có sao đánh giá, "Đã bán", nhãn "Hết hàng"
- [ ] E-29 Trang chi tiết sản phẩm: ảnh lớn, giá, số lượng +/-, nút "Thêm vào giỏ hàng" và "Mua ngay", thông tin shop và địa chỉ, sản phẩm khác của shop; menu khách gồm Trang chủ, Đơn mua, Ví của tôi, Tin nhắn
- [ ] E-30 Đơn mua dạng thẻ theo shop với tab trạng thái (Tất cả, Chờ xác nhận, Chờ lấy hàng, Đang giao, Đã giao, Trả hàng); trên điện thoại có thanh điều hướng dưới đáy (Trang chủ, Giỏ hàng, Tin nhắn, Tôi) và khách vãng lai bấm mục cần tài khoản thì được mời đăng nhập
- [ ] E-31 Dữ liệu hiển thị mới: mỗi sản phẩm có trường daBan và danhGia (daBan tăng khi có đơn); API GET /api/san-pham trả thêm hai trường này, hỗ trợ tham số sapXep (phoBien, moiNhat, banChay, giaTang, giaGiam), tuKhoa và maShop
- [ ] E-32 Lỗi đã gặp: tên thao tác (data-a) không được trùng nhau giữa các màn hình, ví dụ "pd" vừa là mở sản phẩm vừa là xóa sản phẩm nên bấm thẻ sản phẩm không mở được chi tiết; mỗi thao tác một tên riêng

## Mở rộng tuỳ chọn (chỉ làm nếu còn thời gian, ngoài kế hoạch gốc)
- Huỷ đơn khi còn "Mới" và hoàn lại tồn kho; lọc đơn theo trạng thái; xuất báo cáo CSV; tìm hãng nhanh nhất.

## Lịch 3 tuần theo từng người

**A – Leader, thuyết trình 1 · nhánh feature/core-domain**
- Tuần 1: Tạo repo, Jira, nhánh; khung SanPham, GioHang, DonHang, KhuyenMai
- Tuần 2: Hoàn thiện Big 5, 3 loại khuyến mãi; review PR
- Tuần 3: Sửa lỗi tích hợp; merge main, gắn tag v1.0; tập thuyết trình

**B – Thuyết trình 2 · nhánh feature/design-patterns**
- Tuần 1: Khung TrangThaiDonHang và DonViVanChuyen
- Tuần 2: Hoàn thiện 5 trạng thái + lịch sử; 3 hãng ship
- Tuần 3: Test luồng trạng thái; kịch bản demo; tập thuyết trình

**C – Báo cáo 1 · nhánh feature/payment-storage**
- Tuần 1: Khung PhuongThucThanhToan, exception; viết Chương 1
- Tuần 2: Thanh toán 3 loại; DataStore JSON
- Tuần 3: Test, kiểm tra rò rỉ bộ nhớ; viết Chương 4

**D – Báo cáo 2 · nhánh feature/web-api**
- Tuần 1: Server "Hello"; khung route
- Tuần 2: Hoàn thiện API, DonHangService, ThongKeService; viết Chương 3
- Tuần 3: Sửa lỗi API; viết Lời nói đầu, Kết luận; rà báo cáo

**E – Slide + biểu đồ · nhánh feature/frontend**
- Tuần 1: UML nháp; trang HTML + Bootstrap; khung LaTeX
- Tuần 2: Nối 5 màn hình với API; UML hoàn chỉnh; Chương 2
- Tuần 3: Slide PPT, biểu đồ; hoàn thiện giao diện; tập cả nhóm
