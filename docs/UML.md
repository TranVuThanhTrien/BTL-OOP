# Sơ đồ UML

Các sơ đồ dùng cú pháp Mermaid có thể sửa trực tiếp và xem trong GitHub/VS Code với Mermaid preview. Quan hệ được đối chiếu với `include/*.h`, `src/DonHangService.cpp` và `docs/KIEN_TRUC.md`.

## Class diagram

```mermaid
classDiagram
  class NguoiDung {
    <<abstract>>
    +vaiTro() string
    +coQuyen(chucNang) bool
    +clone() unique_ptr
    +vi() TaiKhoanVi*
  }
  class KhachHang
  class NguoiBan
  class ShipperVanChuyen
  class QuanLy
  NguoiDung <|-- KhachHang
  NguoiDung <|-- NguoiBan
  NguoiDung <|-- ShipperVanChuyen
  NguoiDung <|-- QuanLy
  KhachHang *-- TaiKhoanVi
  ShipperVanChuyen *-- TaiKhoanVi

  class DonHang {
    <<aggregate root>>
    -vector~unique_ptr ChiTietDonHang~ m_chiTiet
    -unique_ptr TrangThaiDonHang m_trangThai
    -unique_ptr DonViVanChuyen m_vanChuyen
    -unique_ptr PhuongThucThanhToan m_thanhToan
    -unique_ptr KhuyenMai m_khuyenMai
    +chotDon()
    +tinhKetQua() KetQuaTinhTien
    +datTrangThai(tt)
    +xacNhanGiaoThanhCong(anh)
  }
  DonHang *-- "1..*" ChiTietDonHang
  DonHang *-- "1" TrangThaiDonHang
  DonHang o-- "0..1" DonViVanChuyen
  DonHang o-- "0..1" PhuongThucThanhToan
  DonHang o-- "0..1" KhuyenMai

  class TrangThaiDonHang {
    <<abstract>>
    +chuyenTiep(don)
    +ten() string
    +clone() unique_ptr
  }
  TrangThaiDonHang <|-- DonMoi
  TrangThaiDonHang <|-- DaXacNhan
  TrangThaiDonHang <|-- DangGiao
  TrangThaiDonHang <|-- HoanTat
  TrangThaiDonHang <|-- TraHang

  class DonViVanChuyen {
    <<abstract Strategy>>
    +tinhPhiShip(km, kg) double
    +uocTinhNgay(km) int
    +clone() unique_ptr
  }
  DonViVanChuyen <|-- GHN
  DonViVanChuyen <|-- GHTK
  DonViVanChuyen <|-- ViettelPost
  class PhuongThucThanhToan {
    <<abstract Strategy>>
    +thanhToan(soTien)
    +tinhPhiXuLy(soTien) double
  }
  PhuongThucThanhToan <|-- COD
  PhuongThucThanhToan <|-- TheTinDung
  PhuongThucThanhToan <|-- ViDienTu
  class KhuyenMai {
    <<abstract Strategy>>
    +apDung(tomTat, phiShip) double
  }
  KhuyenMai <|-- KMPhanTram
  KhuyenMai <|-- KMGiamTien
  KhuyenMai <|-- KMFreeShip

  class KhoSanPham
  KhoSanPham o-- SanPham
  class AuthService
  class DonHangService
  class ThongKeService
  class DataStore
  AuthService ..> NguoiDung
  DonHangService --> KhoSanPham
  DonHangService --> AuthService
  DonHangService --> DonHang
  ThongKeService ..> DonHang
  DataStore ..> DonHang
  DataStore ..> SanPham
```

## Use case

```mermaid
flowchart LR
  Khach[Khách hàng] --- U1((Duyệt và tìm sản phẩm))
  Khach --- U2((Quản lý giỏ và đặt đơn))
  Khach --- U3((Thanh toán và theo dõi đơn))
  Khach --- U4((Chat, yêu cầu trả hàng))
  Shop[Người bán] --- U5((Quản lý sản phẩm shop))
  Shop --- U6((Xác nhận và xử lý trả hàng))
  Shop --- U7((Xem thống kê shop))
  Shipper[Shipper] --- U8((Xem đơn của hãng))
  Shipper --- U9((Nhận, giao và gửi ảnh))
  Shipper --- U10((Vận chuyển hàng trả))
  QL[Quản lý] --- U11((Quản trị sản phẩm, tài khoản))
  QL --- U12((Quản lý khuyến mãi, vận chuyển))
  QL --- U13((Xem đơn và thống kê))
  QL --- U14((Xử lý hoàn tiền, kết thúc đơn))
  U2 -. cần đăng nhập .-> Khach
  U3 -. thanh toán .-> U2
```

## Sequence 1: đặt hàng

```mermaid
sequenceDiagram
  actor Khach
  participant Web
  participant API as HTTP API
  participant Auth as AuthService
  participant DH as DonHangService
  participant Kho as KhoSanPham
  participant Don as DonHang
  participant DS as DataStore
  Khach->>Web: Chọn sản phẩm, địa chỉ, vận chuyển, thanh toán
  Web->>API: POST /api/don-hang (JSON + token)
  API->>Auth: xacThuc(token)
  Auth-->>API: NguoiDung khách
  API->>DH: taoDon(khach, gio, diaChi, hang, pttt, maKM, km)
  DH->>Kho: Kiểm tra sản phẩm và tồn kho
  DH->>Don: Dựng đơn, thêm chi tiết, chốt phí và giảm giá
  Don->>Don: tinhKetQua() và thanhToan()
  DH->>Kho: Trừ tồn, cộng số đã bán
  DH->>DS: luuDonHang + luuSanPham + luu tài khoản
  API-->>Web: 201 + JSON đơn hàng
  Web-->>Khach: Hiển thị đơn đã tạo
```

## Sequence 2: giao hàng có ảnh

```mermaid
sequenceDiagram
  actor Shop as Người bán
  actor Ship as Shipper
  participant Web
  participant API as HTTP API
  participant Auth as AuthService
  participant DH as DonHangService
  participant Don as DonHang
  participant DS as DataStore
  Shop->>Web: Xác nhận đơn Mới
  Web->>API: PATCH chuyển trạng thái
  API->>Auth: Xác thực token
  API->>DH: chuyenTrangThai(maDon, shop)
  DH->>Don: Mới -> Xác nhận
  Ship->>Web: Nhận giao đơn thuộc hãng
  Web->>API: PATCH chuyển trạng thái
  API->>DH: chuyenTrangThai(maDon, shipper)
  DH->>Don: Xác nhận -> Đang giao
  Ship->>Web: Gửi ảnh giao hàng và xác nhận
  Web->>API: POST ảnh + xác nhận giao
  API->>DH: xacNhanGiao(maDon, shipper, anh)
  DH->>Don: Kiểm tra hãng, trạng thái, ảnh
  Don->>Don: Đang giao -> Hoàn tất
  DH->>DS: Lưu đơn và dữ liệu liên quan
  API-->>Web: JSON đơn Hoàn tất
```
