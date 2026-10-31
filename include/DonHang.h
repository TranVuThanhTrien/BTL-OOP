#pragma once
#include "NguoiDung.h"
#include "GioHang.h"
#include "KhuyenMai.h"
#include "PhuongThucThanhToan.h"
#include "DonViVanChuyen.h"
#include "TrangThaiDonHang.h"
// Người phụ trách: A | nhánh feature/core-domain | LỚP 4 (aggregate root)
// NGUYÊN TẮC: DonHang giữ MỌI quy tắc nghiệp vụ của một đơn (tiền, trạng thái, trả hàng, ảnh, chat, kết thúc).
// DonHang KHÔNG biết server, JSON, file, token. Việc "ai được gọi hàm nào" (bảng quyền D-15) do DonHangService kiểm tra;
// DonHang chỉ kiểm tra bất biến nghiệp vụ (đúng trạng thái, đúng hãng, đủ ảnh...).
enum class TrangThaiTraHang { Cho, ChoShipperLay, DangGuiVeShop, DaVeShop, TuChoi };   // Cho = chờ shop duyệt
struct AnhDinhKem { string duongDan; string moTa; string thoiDiem; };   // ảnh shipper gửi khách, ảnh khiếu nại của khách; chỉ giữ đường dẫn (file nằm ở data/anh/)
struct TinNhan { string maNguoiGui; string vaiTro; string noiDung; string thoiDiem; };   // chat gắn với từng đơn
struct YeuCauTraHang { string lyDo; string thoiDiem; TrangThaiTraHang trangThai; };
// Kết quả tính tiền: xemTruoc() và đơn thật dùng CHUNG một đường tính (DonHang::tinhKetQua), web không tự tính lại.
struct KetQuaTinhTien { double tienHang = 0, giamGia = 0, phiShip = 0, phiXuLy = 0, tongThanhToan = 0, khoiLuong = 0; int ngayDuKien = 0; };

class ChiTietDonHang {
    string m_maSP, m_maNguoiBan; int m_soLuong; double m_donGia, m_khoiLuong;
public:
    ChiTietDonHang(string maSP, string maNguoiBan, int soLuong, double donGia, double khoiLuong)
        : m_maSP(std::move(maSP)), m_maNguoiBan(std::move(maNguoiBan)), m_soLuong(soLuong), m_donGia(donGia), m_khoiLuong(khoiLuong) {}
    double tinhThanhTien() const { return m_donGia * m_soLuong; }      // TODO(A-8) đã viết sẵn
    double tinhKhoiLuong() const { return m_khoiLuong * m_soLuong; }   // khối lượng một sản phẩm x số lượng
    const string& maSP() const { return m_maSP; }
    const string& maNguoiBan() const { return m_maNguoiBan; }
    int soLuong() const { return m_soLuong; }
    double donGia() const { return m_donGia; }
    double khoiLuong() const { return m_khoiLuong; }                   // của MỘT sản phẩm
};

class DonHang {
    string m_maDon, m_maKhach;                       // m_maKhach = NguoiDung::ma() của chủ đơn (skeleton cũ thiếu -> không lọc được "khách chỉ thấy đơn của mình")
    vector<unique_ptr<ChiTietDonHang>> m_chiTiet;    // composition
    unique_ptr<PhuongThucThanhToan> m_thanhToan;
    unique_ptr<DonViVanChuyen>      m_vanChuyen;
    unique_ptr<KhuyenMai>           m_khuyenMai;     // có thể null
    unique_ptr<TrangThaiDonHang>    m_trangThai;
    vector<LichSuTrangThai>         m_lichSu;
    double m_khoangCachKm = 0;
    double m_phiShip = 0;                            // CHỐT lúc chotDon() để đổi bảng giá ship không làm đổi đơn cũ
    bool   m_daChot = false;
    string m_thoiDiemDat;
    DiaChi m_diaChiGiao;                             // khách nhập khi thanh toán
    unique_ptr<YeuCauTraHang> m_yeuCauTra;           // null = chưa trả hàng
    vector<AnhDinhKem> m_anhGiaoHang, m_anhKhieuNai;
    vector<TinNhan> m_tinNhan;                       // khách, shop, shipper, quản lý cùng thấy
    map<string,size_t> m_daDoc;                      // tên đăng nhập -> số tin đã xem (cho soTinChuaDoc)
    map<string,double> m_daHoanTien;                 // mã shop -> số tiền đã hoàn cho khách
    bool m_daTraPhiShip = false;                     // đã cộng phí ship cho shipper chưa
    string m_thoiDiemKetThuc;                        // rỗng = chưa kết thúc
    friend class DataStore;                          // DataStore dựng lại đơn từ file; không ai khác được chạm vào trường private
public:
    // ---- Dựng đơn: DonHangService chỉ gọi đúng thứ tự: ctor -> themChiTiet... -> datDiaChiGiao -> chotDon ----
    DonHang();                                       // đơn rỗng, trạng thái Mới (DataStore dùng)
    DonHang(string maDon, string maKhach, double khoangCachKm, unique_ptr<DonViVanChuyen> vc, unique_ptr<PhuongThucThanhToan> pt, unique_ptr<KhuyenMai> km);
    void themChiTiet(unique_ptr<ChiTietDonHang> ct);
    void datDiaChiGiao(const DiaChi& dc);            // TODO(A-16): bắt buộc họ tên, laSoDienThoaiHopLe, địa chỉ >= hangso::DIA_CHI_TOI_THIEU -> OrderException
    void chotDon();                                  // kiểm tra đủ chi tiết + địa chỉ + khuyến mãi hợp lệ (MaGiamGiaKhongHopLeException), chụp m_phiShip, ghi m_thoiDiemDat, lịch sử "Mới"
    // ---- Big 5 (TODO(A-9)); clone() từng thành viên unique_ptr (null thì giữ null) ----
    ~DonHang();
    DonHang(const DonHang& o);
    DonHang& operator=(const DonHang& o);            // copy-and-swap
    DonHang(DonHang&& o) noexcept;
    DonHang& operator=(DonHang&& o) noexcept;
    bool operator==(const DonHang& o) const;         // TODO(A-10) so mã đơn
    friend ostream& operator<<(ostream& os, const DonHang& d);   // TODO(A-10)
    // ---- Tiền (TODO(A-11)); mọi kết quả qua lamTronDong ----
    TomTatGio tomTat() const;                        // tạo từ m_chiTiet, đưa cho KhuyenMai
    double tinhTienHang() const;
    double tinhGiamGia() const;                      // m_khuyenMai ? apDung(tomTat(), tinhPhiShip()) : 0
    double tinhPhiShip() const;                      // đã chốt thì trả m_phiShip, chưa chốt thì tính tạm từ m_vanChuyen (dùng cho xemTruoc)
    double tinhPhiXuLy() const;                      // m_thanhToan->tinhPhiXuLy(tienHang - giamGia + phiShip)
    double tinhTongThanhToan() const;                // tienHang - giamGia + phiShip + phiXuLy
    KetQuaTinhTien tinhKetQua() const;
    double tienThuHoCOD() const;                     // thanh toán COD: tổng cần thu khi giao; còn lại 0 (D-16)
    // ---- Trạng thái ----
    void chuyenTrangThai();                          // TODO(A-12): kiểm tra daKetThuc() (A-14) -> m_trangThai->chuyenTiep(*this)
    void datTrangThai(unique_ptr<TrangThaiDonHang> tt);   // TODO(A-12): B gọi từ lớp trạng thái; ĐÂY là nơi ghi m_lichSu (B-5)
    string trangThai() const;                        // ten() của trạng thái hiện tại (tt::*)
    bool daTungQuaTrangThai(const string& tenTT) const;   // tra m_lichSu, dùng cho thống kê tỷ lệ (D-3)
    // ---- Giao hàng: ảnh bắt buộc (TODO(A-19)) ----
    bool coAnhGiaoHang() const { return !m_anhGiaoHang.empty(); }
    void xacNhanGiaoThanhCong(const AnhDinhKem& anh);     // chỉ khi đang DangGiao: lưu ảnh rồi chuyenTrangThai() sang Hoàn tất
    void themAnhGiaoHang(const AnhDinhKem& a);            // TODO(A-18) shipper, đơn đã giao
    void themAnhKhieuNai(const AnhDinhKem& a);            // TODO(A-18) khách, đơn đã giao, chưa kết thúc
    // Phí ship cho shipper: QUYẾT ĐỊNH – DonHang không giữ ví shipper, nên Service gọi sau khi đơn sang Hoàn tất; hàm tự chống cộng 2 lần.
    bool   canTraPhiShip() const { return !m_daTraPhiShip && trangThai() == tt::HOAN_TAT; }
    double traPhiShip(TaiKhoanVi& viShipper);             // TODO(A-19,C-11) cộng phiShip đúng 1 lần, ghi lịch sử ví; lần sau trả 0
    // ---- Trả hàng (TODO(A-17), TODO(A-20)) ----
    void yeuCauTraHang(const string& lyDo);               // khách; chỉ khi Hoàn tất, chưa kết thúc, chưa có yêu cầu
    void hoanHang();                                      // shop chấp nhận: Cho -> ChoShipperLay (đơn vẫn ở Hoàn tất)
    void tuChoiTraHang();                                 // Cho -> TuChoi
    void shipperNhanDonTra(const string& maHang);         // đúng hãng của đơn: ChoShipperLay -> DangGuiVeShop
    void shipperXacNhanGuiVeShop();                       // DangGuiVeShop -> DaVeShop, rồi chuyển đơn sang Trả hàng
    double hoanTien(const string& maShop, TaiKhoanVi& viKhach);   // hoàn tiền HÀNG của shop (không gồm ship), 1 lần/shop; trả số tiền; ghi m_daHoanTien
    bool dangXuLyTraHang() const;                         // Cho / ChoShipperLay / DangGuiVeShop
    bool hangDaVeShop() const;                            // yêu cầu trả = DaVeShop: điều kiện để HoanTat::chuyenTiep sang Trả hàng
    // ---- Kết thúc đơn (TODO(A-14)) ----
    void ketThucDon();                                    // chỉ khi Hoàn tất hoặc Trả hàng và !dangXuLyTraHang(); ghi m_thoiDiemKetThuc
    bool daKetThuc() const { return !m_thoiDiemKetThuc.empty(); }
    // ---- Chat (TODO(A-21)) ----
    bool thuocDon(const NguoiDung& nguoi) const;          // khách chủ đơn | NguoiBan có hàng | Shipper đúng hãng | QuanLy
    void guiTinNhan(const NguoiDung& nguoiGui, const string& noiDung);   // không thuộc đơn -> KhongCoQuyenException; rỗng hoặc > hangso::TIN_NHAN_TOI_DA -> OrderException
    int  soTinChuaDoc(const NguoiDung& nguoi) const;
    void danhDauDaDoc(const NguoiDung& nguoi);
    // ---- Truy cập chỉ đọc (D map sang JSON qua các hàm này) ----
    const string& maDon() const { return m_maDon; }
    const string& maKhach() const { return m_maKhach; }
    double khoangCachKm() const { return m_khoangCachKm; }
    const string& thoiDiemDat() const { return m_thoiDiemDat; }
    const vector<unique_ptr<ChiTietDonHang>>& chiTiet() const { return m_chiTiet; }
    const DonViVanChuyen* vanChuyen() const { return m_vanChuyen.get(); }
    const PhuongThucThanhToan* thanhToan() const { return m_thanhToan.get(); }
    const KhuyenMai* khuyenMai() const { return m_khuyenMai.get(); }
    const vector<LichSuTrangThai>& lichSu() const { return m_lichSu; }
    const DiaChi& diaChiGiao() const { return m_diaChiGiao; }
    const YeuCauTraHang* yeuCauTra() const { return m_yeuCauTra.get(); }
    const vector<AnhDinhKem>& anhGiaoHang() const { return m_anhGiaoHang; }
    const vector<AnhDinhKem>& anhKhieuNai() const { return m_anhKhieuNai; }
    const vector<TinNhan>& tinNhan() const { return m_tinNhan; }
    const map<string,double>& daHoanTien() const { return m_daHoanTien; }
    const string& thoiDiemKetThuc() const { return m_thoiDiemKetThuc; }
    bool daTraPhiShip() const { return m_daTraPhiShip; }
    bool coHangCuaShop(const string& maShop) const;       // có ChiTietDonHang của shop này không
    string hangVanChuyen() const { return m_vanChuyen ? m_vanChuyen->ten() : string(); }
};
