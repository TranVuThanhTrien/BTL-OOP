#pragma once
#include "GioHang.h"
#include "KhuyenMai.h"
#include "PhuongThucThanhToan.h"
#include "DonViVanChuyen.h"
#include "TrangThaiDonHang.h"
// Người phụ trách: A | nhánh feature/core-domain
struct DiaChi { string hoTen, soDienThoai, diaChi; };                 // địa chỉ giao hàng của khách và địa chỉ lấy hàng của shop
struct AnhDinhKem { string duongDan; string moTa; string thoiDiem; };  // ảnh shipper gửi khách, ảnh khiếu nại của khách
enum class TrangThaiTraHang { Cho, ChoShipperLay, DangGuiVeShop, DaVeShop, TuChoi };   // Cho = chờ shop duyệt
struct TinNhan { string maNguoiGui; string vaiTro; string noiDung; string thoiDiem; };       // chat gắn với từng đơn hàng
struct YeuCauTraHang { string lyDo; string thoiDiem; TrangThaiTraHang trangThai; };
class ChiTietDonHang {
    string m_maSP, m_maNguoiBan; int m_soLuong; double m_donGia, m_khoiLuong;
public:
    ChiTietDonHang(string maSP, string maNguoiBan, int soLuong, double donGia, double khoiLuong);
    // TODO(A-8): double tinhThanhTien() const;  double tinhKhoiLuong() const;  getter
};
class DonHang {
    string m_maDon;
    vector<unique_ptr<ChiTietDonHang>> m_chiTiet;    // composition
    unique_ptr<PhuongThucThanhToan> m_thanhToan;
    unique_ptr<DonViVanChuyen>      m_vanChuyen;
    unique_ptr<KhuyenMai>           m_khuyenMai;     // có thể null
    unique_ptr<TrangThaiDonHang>    m_trangThai;
    vector<LichSuTrangThai>         m_lichSu;
    double m_khoangCachKm = 0;
    DiaChi m_diaChiGiao;                             // khách nhập khi thanh toán
    unique_ptr<YeuCauTraHang> m_yeuCauTra;           // null = chưa trả hàng
    vector<AnhDinhKem> m_anhGiaoHang, m_anhKhieuNai;
    vector<TinNhan> m_tinNhan;                       // khách, shop, shipper, quản lý cùng thấy
    map<string,double> m_daHoanTien;                 // mã shop -> số tiền đã hoàn cho khách
    bool m_daTraPhiShip = false;                     // đã cộng phí ship cho shipper chưa
    string m_thoiDiemKetThuc;                        // rỗng = chưa kết thúc
public:
    DonHang();
    // TODO(A-9): ~DonHang();                                   // 1. destructor
    // TODO(A-9): DonHang(const DonHang& o);                    // 2. copy sâu (dùng clone())
    // TODO(A-9): DonHang& operator=(const DonHang& o);         // 3. copy assign (copy-and-swap)
    // TODO(A-9): DonHang(DonHang&& o) noexcept;                // 4. move ctor
    // TODO(A-9): DonHang& operator=(DonHang&& o) noexcept;     // 5. move assign
    // TODO(A-10): bool operator==(const DonHang& o) const;  friend ostream& operator<<(ostream&, const DonHang&);
    // TODO(A-11): double tinhTienHang() const;  double tinhGiamGia() const;  double tinhTongThanhToan() const;
    // TODO(A-12): void chuyenTrangThai();   // gọi m_trangThai->chuyenTiep(*this)
    // TODO(A-16): void datDiaChiGiao(const DiaChi& dc);  const DiaChi& diaChiGiao() const;   // bắt buộc có họ tên, số điện thoại 9-11 số, địa chỉ >= 8 ký tự
    // TODO(A-17): void yeuCauTraHang(const string& lyDo);   // khách, chỉ khi HoanTat và chưa kết thúc, chỉ 1 lần
    // TODO(A-17): void hoanHang();   // shop chấp nhận: Cho -> ChoShipperLay (đơn vẫn ở HoanTat)
    // TODO(A-20): void shipperNhanDonTra(const string& maHang);   // đúng hãng của đơn: ChoShipperLay -> DangGuiVeShop
    // TODO(A-20): void shipperXacNhanGuiVeShop();                 // DangGuiVeShop -> DaVeShop, đồng thời chuyển trạng thái đơn sang TraHang
    // TODO(A-20): ketThucDon() bị chặn khi yêu cầu trả hàng đang xử lý (Cho, ChoShipperLay, DangGuiVeShop)
    // TODO(A-21): void guiTinNhan(const NguoiDung& nguoiGui, const string& noiDung);   // chỉ người thuộc đơn: khách chủ đơn, người bán có hàng, shipper đúng hãng, quản lý; tối đa 500 ký tự
    // TODO(A-21): int soTinChuaDoc(const NguoiDung& nguoi) const;   // tin của người khác gửi sau lần xem gần nhất
    // TODO(A-17): void tuChoiTraHang();
    // TODO(A-17): double hoanTien(const string& maShop);   // hoàn tiền hàng của shop đó (không gồm phí ship), 1 lần mỗi shop, cộng vào TaiKhoanVi của khách
    // TODO(A-18): void themAnhGiaoHang(const AnhDinhKem& a);  void themAnhKhieuNai(const AnhDinhKem& a);   // shipper (đơn đã giao), khách (đơn đã giao, chưa kết thúc)
    // TODO(A-19): bool coAnhGiaoHang() const;  void xacNhanGiaoThanhCong(const AnhDinhKem& anh);   // shipper gửi ảnh -> lưu ảnh rồi mới chuyển DangGiao -> HoanTat
    // TODO(A-19): chuyenTrangThai() từ DangGiao phải ném ChuyenTrangThaiSaiException nếu chưa có ảnh giao hàng (kể cả quản lý cũng không bỏ qua được); cộng phí ship cho shipper đúng 1 lần khi sang HoanTat
    // TODO(A-14): void ketThucDon();  bool daKetThuc() const;   // chỉ khi HoanTat hoặc TraHang, sai -> ChuyenTrangThaiSaiException; ghi m_thoiDiemKetThuc
    // TODO(A-14): chuyenTrangThai() phải ném ChuyenTrangThaiSaiException nếu đơn đã kết thúc (không thêm trạng thái thứ 6, dùng cờ riêng)
    // TODO(A-12): void datTrangThai(unique_ptr<TrangThaiDonHang> tt);  // B gọi từ các lớp trạng thái
};
