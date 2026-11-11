#include "DonHang.h"
#include "OrderException.h"

// Người phụ trách: A | nhánh feature/core-domain | LỚP 4 (Aggregate Root)

DonHang::DonHang()
    : m_trangThai(make_unique<DonMoi>()) {}

DonHang::DonHang(string maDon, string maKhach, double khoangCachKm,
                 unique_ptr<DonViVanChuyen> vc,
                 unique_ptr<PhuongThucThanhToan> pt,
                 unique_ptr<KhuyenMai> km)
    : m_maDon(std::move(maDon)),
      m_maKhach(std::move(maKhach)),
      m_thanhToan(std::move(pt)),
      m_vanChuyen(std::move(vc)),
      m_khuyenMai(std::move(km)),
      m_trangThai(make_unique<DonMoi>()),
      m_khoangCachKm(khoangCachKm) {
    if (m_khoangCachKm <= 0) {
        throw OrderException("Khoảng cách giao hàng phải lớn hơn 0 km");
    }
    if (!m_vanChuyen) {
        throw OrderException("Đơn hàng thiếu đơn vị vận chuyển");
    }
    if (!m_thanhToan) {
        throw OrderException("Đơn hàng thiếu phương thức thanh toán");
    }
}

void DonHang::themChiTiet(unique_ptr<ChiTietDonHang> ct) {
    if (!ct || ct->soLuong() <= 0) {
        throw OrderException("Chi tiết đơn hàng không hợp lệ");
    }
    // Nghiệp vụ thực tế: nếu cùng mã sản phẩm được thêm nhiều lần vào đơn thì gộp dòng
    for (auto& hienCo : m_chiTiet) {
        if (hienCo->maSP() == ct->maSP()) {
            int tongSL = hienCo->soLuong() + ct->soLuong();
            hienCo = make_unique<ChiTietDonHang>(
                ct->maSP(), ct->maNguoiBan(), tongSL, ct->donGia(), ct->khoiLuong());
            return;
        }
    }
    m_chiTiet.push_back(std::move(ct));
}

void DonHang::datDiaChiGiao(const DiaChi& dc) {
    if (dc.hoTen.empty() || !laSoDienThoaiHopLe(dc.soDienThoai) || dc.diaChi.size() < hangso::DIA_CHI_TOI_THIEU) {
        throw OrderException("Nhập đủ họ tên, số điện thoại (9-11 số) và địa chỉ giao hàng (ít nhất 8 ký tự)");
    }
    m_diaChiGiao = dc;
}

void DonHang::chotDon() {
    if (m_chiTiet.empty()) {
        throw HetHangException("Giỏ hàng trống, không thể chốt đơn");
    }
    datDiaChiGiao(m_diaChiGiao);
    TomTatGio tt = tomTat();
    if (m_khuyenMai && !m_khuyenMai->dieuKienHopLe(tt)) {
        throw MaGiamGiaKhongHopLeException("Đơn hàng chưa đủ điều kiện áp dụng mã " + m_khuyenMai->ma());
    }
    m_phiShip = m_vanChuyen->tinhPhiShip(m_khoangCachKm, tt.tongKhoiLuong);
    m_daChot = true;
    m_thoiDiemDat = thoiGianHienTai();
    m_trangThai = make_unique<DonMoi>();
    m_lichSu.clear();
    m_lichSu.push_back(LichSuTrangThai{m_trangThai->ten(), m_thoiDiemDat});
}

DonHang::~DonHang() = default;

DonHang::DonHang(const DonHang& o)
    : m_maDon(o.m_maDon),
      m_maKhach(o.m_maKhach),
      m_thanhToan(o.m_thanhToan ? o.m_thanhToan->clone() : nullptr),
      m_vanChuyen(o.m_vanChuyen ? o.m_vanChuyen->clone() : nullptr),
      m_khuyenMai(o.m_khuyenMai ? o.m_khuyenMai->clone() : nullptr),
      m_trangThai(o.m_trangThai ? o.m_trangThai->clone() : nullptr),
      m_lichSu(o.m_lichSu),
      m_khoangCachKm(o.m_khoangCachKm),
      m_phiShip(o.m_phiShip),
      m_daChot(o.m_daChot),
      m_thoiDiemDat(o.m_thoiDiemDat),
      m_diaChiGiao(o.m_diaChiGiao),
      m_yeuCauTra(o.m_yeuCauTra ? make_unique<YeuCauTraHang>(*o.m_yeuCauTra) : nullptr),
      m_anhGiaoHang(o.m_anhGiaoHang),
      m_anhKhieuNai(o.m_anhKhieuNai),
      m_tinNhan(o.m_tinNhan),
      m_daDoc(o.m_daDoc),
      m_daHoanTien(o.m_daHoanTien),
      m_daTraPhiShip(o.m_daTraPhiShip),
      m_thoiDiemKetThuc(o.m_thoiDiemKetThuc) {
    m_chiTiet.reserve(o.m_chiTiet.size());
    for (const auto& ct : o.m_chiTiet) {
        if (ct) m_chiTiet.push_back(make_unique<ChiTietDonHang>(*ct));
    }
}

DonHang& DonHang::operator=(const DonHang& o) {
    if (this != &o) {
        DonHang tmp(o);
        *this = std::move(tmp);
    }
    return *this;
}

DonHang::DonHang(DonHang&& o) noexcept = default;
DonHang& DonHang::operator=(DonHang&& o) noexcept = default;

bool DonHang::operator==(const DonHang& o) const {
    return m_maDon == o.m_maDon;
}

ostream& operator<<(ostream& os, const DonHang& d) {
    os << "[" << d.m_maDon << " | Khách: " << d.m_maKhach
       << " | Trạng thái: " << d.trangThai()
       << " | Tổng: " << dinhDangTien(d.tinhTongThanhToan()) << "]";
    return os;
}

TomTatGio DonHang::tomTat() const {
    TomTatGio tt;
    for (const auto& ct : m_chiTiet) {
        double tien = ct->tinhThanhTien();
        tt.tongTien += tien;
        tt.tongKhoiLuong += ct->tinhKhoiLuong();
        tt.theoNguoiBan[ct->maNguoiBan()] = lamTronDong(tt.theoNguoiBan[ct->maNguoiBan()] + tien);
    }
    tt.tongTien = lamTronDong(tt.tongTien);
    tt.soNguoiBan = static_cast<int>(tt.theoNguoiBan.size());
    return tt;
}

double DonHang::tinhTienHang() const {
    return tomTat().tongTien;
}

double DonHang::tinhPhiShip() const {
    if (m_daChot) return lamTronDong(m_phiShip);
    if (!m_vanChuyen || m_chiTiet.empty()) return 0;
    return m_vanChuyen->tinhPhiShip(m_khoangCachKm, tomTat().tongKhoiLuong);
}

double DonHang::tinhGiamGia() const {
    if (!m_khuyenMai) return 0;
    TomTatGio tt = tomTat();
    if (!m_khuyenMai->dieuKienHopLe(tt)) {
        throw MaGiamGiaKhongHopLeException("Đơn hàng không đủ điều kiện áp dụng mã " + m_khuyenMai->ma());
    }
    return m_khuyenMai->apDung(tt, tinhPhiShip());
}

double DonHang::tinhPhiXuLy() const {
    if (!m_thanhToan) return 0;
    double coSo = tinhTienHang() - tinhGiamGia() + tinhPhiShip();
    return m_thanhToan->tinhPhiXuLy(std::max(0.0, coSo));
}

double DonHang::tinhTongThanhToan() const {
    double tong = tinhTienHang() - tinhGiamGia() + tinhPhiShip() + tinhPhiXuLy();
    return lamTronDong(std::max(0.0, tong));
}

KetQuaTinhTien DonHang::tinhKetQua() const {
    KetQuaTinhTien kq;
    TomTatGio tt = tomTat();
    kq.tienHang = tt.tongTien;
    kq.khoiLuong = tt.tongKhoiLuong;
    kq.phiShip = tinhPhiShip();
    kq.giamGia = tinhGiamGia();
    kq.phiXuLy = tinhPhiXuLy();
    kq.tongThanhToan = lamTronDong(std::max(0.0, kq.tienHang - kq.giamGia + kq.phiShip + kq.phiXuLy));
    kq.ngayDuKien = m_vanChuyen ? m_vanChuyen->uocTinhNgay(m_khoangCachKm) : 0;
    return kq;
}

double DonHang::tienThuHoCOD() const {
    if (m_thanhToan && m_thanhToan->ten() == pttt::COD) {
        return tinhTongThanhToan();
    }
    return 0;
}

void DonHang::chuyenTrangThai() {
    if (daKetThuc()) {
        throw ChuyenTrangThaiSaiException("Đơn đã kết thúc, không thể đổi trạng thái");
    }
    if (!m_trangThai) {
        throw ChuyenTrangThaiSaiException("Đơn hàng chưa có trạng thái");
    }
    m_trangThai->chuyenTiep(*this);
}

void DonHang::datTrangThai(unique_ptr<TrangThaiDonHang> ttMoi) {
    if (!ttMoi) return;
    m_trangThai = std::move(ttMoi);
    m_lichSu.push_back(LichSuTrangThai{m_trangThai->ten(), thoiGianHienTai()});
}

string DonHang::trangThai() const {
    return m_trangThai ? m_trangThai->ten() : tt::MOI;
}

bool DonHang::daTungQuaTrangThai(const string& tenTT) const {
    for (const auto& ls : m_lichSu) {
        if (ls.trangThai == tenTT) return true;
    }
    return trangThai() == tenTT;
}

void DonHang::xacNhanGiaoThanhCong(const AnhDinhKem& anh) {
    if (daKetThuc()) {
        throw ChuyenTrangThaiSaiException("Đơn đã kết thúc");
    }
    if (trangThai() != tt::DANG_GIAO) {
        throw ChuyenTrangThaiSaiException("Chỉ xác nhận giao thành công khi đơn đang ở trạng thái Đang giao");
    }
    if (anh.duongDan.empty()) {
        throw ChuyenTrangThaiSaiException("Bắt buộc phải có ảnh xác nhận giao hàng");
    }
    m_anhGiaoHang.push_back(anh);
    chuyenTrangThai();
}

void DonHang::themAnhGiaoHang(const AnhDinhKem& a) {
    if (daKetThuc()) throw ChuyenTrangThaiSaiException("Đơn đã kết thúc");
    if (trangThai() != tt::HOAN_TAT && trangThai() != tt::TRA_HANG) {
        throw ChuyenTrangThaiSaiException("Chỉ gửi thêm ảnh tình trạng hàng sau khi đã giao");
    }
    if (a.duongDan.empty()) throw OrderException("Đường dẫn ảnh không được rỗng");
    m_anhGiaoHang.push_back(a);
}

void DonHang::themAnhKhieuNai(const AnhDinhKem& a) {
    if (daKetThuc()) throw ChuyenTrangThaiSaiException("Đơn đã kết thúc");
    if (trangThai() != tt::HOAN_TAT && trangThai() != tt::TRA_HANG) {
        throw ChuyenTrangThaiSaiException("Chỉ gửi ảnh khiếu nại khi đơn đã giao");
    }
    if (a.duongDan.empty()) throw OrderException("Đường dẫn ảnh khiếu nại không được rỗng");
    m_anhKhieuNai.push_back(a);
}

double DonHang::traPhiShip(TaiKhoanVi& viShipper) {
    if (!canTraPhiShip()) return 0;
    double phi = tinhPhiShip();
    if (phi > 0) {
        viShipper.nhanTien(phi, "Phí ship đơn " + m_maDon);
    }
    m_daTraPhiShip = true;
    return phi;
}

void DonHang::yeuCauTraHang(const string& lyDo) {
    if (daKetThuc()) {
        throw ChuyenTrangThaiSaiException("Đơn đã kết thúc, không thể trả hàng");
    }
    if (trangThai() != tt::HOAN_TAT) {
        throw ChuyenTrangThaiSaiException("Chỉ yêu cầu trả hàng khi đơn đã Hoàn tất");
    }
    if (m_yeuCauTra) {
        throw OrderException("Đơn này đã có yêu cầu trả hàng");
    }
    if (lyDo.empty()) {
        throw OrderException("Vui lòng nhập lý do trả hàng");
    }
    m_yeuCauTra = make_unique<YeuCauTraHang>(YeuCauTraHang{lyDo, thoiGianHienTai(), TrangThaiTraHang::Cho});
}

void DonHang::hoanHang() {
    if (!m_yeuCauTra || m_yeuCauTra->trangThai != TrangThaiTraHang::Cho) {
        throw ChuyenTrangThaiSaiException("Không có yêu cầu trả hàng nào đang chờ duyệt");
    }
    m_yeuCauTra->trangThai = TrangThaiTraHang::ChoShipperLay;
}

void DonHang::tuChoiTraHang() {
    if (!m_yeuCauTra || m_yeuCauTra->trangThai != TrangThaiTraHang::Cho) {
        throw ChuyenTrangThaiSaiException("Không có yêu cầu trả hàng nào đang chờ duyệt");
    }
    m_yeuCauTra->trangThai = TrangThaiTraHang::TuChoi;
}

void DonHang::shipperNhanDonTra(const string& maHang) {
    if (maHang != hangVanChuyen()) {
        throw KhongCoQuyenException("Chỉ shipper thuộc hãng vận chuyển của đơn mới được nhận đơn trả");
    }
    if (!m_yeuCauTra || m_yeuCauTra->trangThai != TrangThaiTraHang::ChoShipperLay) {
        throw ChuyenTrangThaiSaiException("Đơn không ở bước chờ shipper lấy hàng trả");
    }
    m_yeuCauTra->trangThai = TrangThaiTraHang::DangGuiVeShop;
}

void DonHang::shipperXacNhanGuiVeShop() {
    if (!m_yeuCauTra || m_yeuCauTra->trangThai != TrangThaiTraHang::DangGuiVeShop) {
        throw ChuyenTrangThaiSaiException("Đơn không ở bước đang gửi hàng trả về shop");
    }
    m_yeuCauTra->trangThai = TrangThaiTraHang::DaVeShop;
    chuyenTrangThai(); // Hoàn tất -> Trả hàng
}

// NÂNG CẤP NGHIỆP VỤ THỰC TẾ:
// 1. Chặn hoàn tiền nếu yêu cầu trả hàng đã bị từ chối và không có ảnh khiếu nại hàng lỗi.
// 2. Khấu trừ phần giảm giá voucher (GIAM10, GIAM30K) theo tỷ lệ đóng góp tiền hàng của shop (Pro-rata),
//    đảm bảo khách không bao giờ nhận hoàn tiền nhiều hơn số thực trả cho phần hàng đó.
double DonHang::hoanTien(const string& maShop, TaiKhoanVi& viKhach) {
    if (daKetThuc()) {
        throw ChuyenTrangThaiSaiException("Đơn đã kết thúc, không thể hoàn tiền");
    }
    bool coYeuCauHopLe = m_yeuCauTra && (m_yeuCauTra->trangThai != TrangThaiTraHang::TuChoi);
    if (!coYeuCauHopLe && m_anhKhieuNai.empty()) {
        throw ThanhToanThatBaiException("Đơn hàng chưa có yêu cầu trả hàng hợp lệ hoặc ảnh bằng chứng khiếu nại");
    }
    string shop = inHoa(maShop);
    if (m_daHoanTien.count(shop)) {
        throw ThanhToanThatBaiException("Shop " + shop + " đã hoàn tiền cho đơn này rồi");
    }
    double tienShop = 0;
    for (const auto& ct : m_chiTiet) {
        if (ct->maNguoiBan() == shop) {
            tienShop += ct->tinhThanhTien();
        }
    }
    tienShop = lamTronDong(tienShop);
    if (tienShop <= 0) {
        throw ThanhToanThatBaiException("Không có sản phẩm nào của shop " + shop + " trong đơn");
    }

    // Phân bổ giảm giá hàng hóa theo tỷ lệ (Pro-rata Discount Allocation)
    double tongTienHang = tinhTienHang();
    double giamGiaDon = 0;
    try {
        giamGiaDon = tinhGiamGia();
    } catch (...) {
        giamGiaDon = 0;
    }
    // Mã FREESHIP giảm vào phí vận chuyển, không trừ vào tiền hàng của shop
    double giamGiaHang = (m_khuyenMai && m_khuyenMai->ma() == km::FREESHIP) ? 0.0 : giamGiaDon;
    double phanGiamCuaShop = (tongTienHang > 0 && giamGiaHang > 0)
                                 ? lamTronDong(giamGiaHang * (tienShop / tongTienHang))
                                 : 0.0;
    double soTienHoan = lamTronDong(std::max(0.0, tienShop - phanGiamCuaShop));

    string moTa = "Hoàn tiền đơn " + m_maDon + " (" + shop + ")";
    if (phanGiamCuaShop > 0) {
        moTa += " [đã khấu trừ KM " + dinhDangTien(phanGiamCuaShop) + "]";
    }
    viKhach.hoanTien(soTienHoan, moTa);
    m_daHoanTien[shop] = soTienHoan;
    return soTienHoan;
}

bool DonHang::dangXuLyTraHang() const {
    if (!m_yeuCauTra) return false;
    return m_yeuCauTra->trangThai == TrangThaiTraHang::Cho ||
           m_yeuCauTra->trangThai == TrangThaiTraHang::ChoShipperLay ||
           m_yeuCauTra->trangThai == TrangThaiTraHang::DangGuiVeShop;
}

bool DonHang::hangDaVeShop() const {
    return m_yeuCauTra && m_yeuCauTra->trangThai == TrangThaiTraHang::DaVeShop;
}

// Cho phép kết thúc đơn khi:
// - Đơn đã Hoàn tất hoặc Trả hàng (và không còn yêu cầu trả đang xử lý) -> Đóng đơn thành công.
// - Đơn đang ở trạng thái Mới (chưa shop nào xác nhận) -> Hủy đơn sớm trước khi giao.
void DonHang::ketThucDon() {
    if (daKetThuc()) {
        throw ChuyenTrangThaiSaiException("Đơn đã kết thúc rồi");
    }
    if (dangXuLyTraHang()) {
        throw ChuyenTrangThaiSaiException("Đơn đang có yêu cầu trả hàng chưa xử lý xong");
    }
    string ttHienTai = trangThai();
    if (ttHienTai != tt::HOAN_TAT && ttHienTai != tt::TRA_HANG && ttHienTai != tt::MOI) {
        throw ChuyenTrangThaiSaiException("Chỉ kết thúc/hủy đơn khi ở trạng thái Mới, Hoàn tất hoặc Trả hàng");
    }
    m_thoiDiemKetThuc = thoiGianHienTai();
}

bool DonHang::coHangCuaShop(const string& maShop) const {
    string s = inHoa(maShop);
    for (const auto& ct : m_chiTiet) {
        if (inHoa(ct->maNguoiBan()) == s) return true;
    }
    return false;
}

bool DonHang::thuocDon(const NguoiDung& nguoi) const {
    const string vt = nguoi.vaiTro();
    if (vt == vaitro::QUAN_LY) return true;
    if (vt == vaitro::KHACH_HANG) {
        return inThuong(nguoi.tenDangNhap()) == inThuong(m_maKhach) ||
               inThuong(nguoi.ma()) == inThuong(m_maKhach);
    }
    if (vt == vaitro::NGUOI_BAN) {
        return coHangCuaShop(nguoi.maShop());
    }
    if (vt == vaitro::SHIPPER) {
        return nguoi.maHang() == hangVanChuyen();
    }
    return false;
}

void DonHang::guiTinNhan(const NguoiDung& nguoiGui, const string& noiDung) {
    if (!thuocDon(nguoiGui)) {
        throw KhongCoQuyenException("Bạn không thuộc đơn hàng này");
    }
    if (noiDung.empty() || noiDung.size() > hangso::TIN_NHAN_TOI_DA) {
        throw OrderException("Tin nhắn không được rỗng và tối đa 500 ký tự");
    }
    m_tinNhan.push_back(TinNhan{nguoiGui.tenDangNhap(), nguoiGui.vaiTro(), noiDung, thoiGianHienTai()});
    m_daDoc[inThuong(nguoiGui.tenDangNhap())] = m_tinNhan.size();
}

int DonHang::soTinChuaDoc(const NguoiDung& nguoi) const {
    if (!thuocDon(nguoi)) return 0;
    string key = inThuong(nguoi.tenDangNhap());
    std::size_t moc = 0;
    auto it = m_daDoc.find(key);
    if (it != m_daDoc.end()) moc = it->second;
    int dem = 0;
    for (std::size_t i = moc; i < m_tinNhan.size(); ++i) {
        if (inThuong(m_tinNhan[i].maNguoiGui) != key) {
            ++dem;
        }
    }
    return dem;
}

void DonHang::danhDauDaDoc(const NguoiDung& nguoi) {
    if (!thuocDon(nguoi)) return;
    m_daDoc[inThuong(nguoi.tenDangNhap())] = m_tinNhan.size();
}
