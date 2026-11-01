#include "DonHang.h"
#include "OrderException.h"
// Người phụ trách: A | A-8..A-12, A-14, A-16..A-21
// DonHang giữ bất biến nghiệp vụ của một đơn. "Ai được gọi hàm nào" (bảng quyền D-15) do DonHangService kiểm tra.

namespace {
// Nhân bản một thành viên unique_ptr đa hình (null thì giữ null).
template <class T>
unique_ptr<T> nhanBan(const unique_ptr<T>& p) { return p ? p->clone() : nullptr; }

string catKhoangTrang(const string& s) {
    size_t dau = s.find_first_not_of(" \t\r\n");
    if (dau == string::npos) return "";
    size_t cuoi = s.find_last_not_of(" \t\r\n");
    return s.substr(dau, cuoi - dau + 1);
}
// Đếm ký tự UTF-8 (bỏ qua byte tiếp nối 10xxxxxx) để giới hạn 500 ký tự đúng với tiếng Việt có dấu.
size_t demKyTu(const string& s) {
    size_t n = 0;
    for (unsigned char c : s) if ((c & 0xC0) != 0x80) ++n;
    return n;
}
bool cungMaShop(const string& a, const string& b) { return inHoa(a) == inHoa(b); }
}

// ============================================================================
// Dựng đơn
// ============================================================================
DonHang::DonHang() : m_trangThai(make_unique<DonMoi>()) {}

DonHang::DonHang(string maDon, string maKhach, double khoangCachKm,
                 unique_ptr<DonViVanChuyen> vc, unique_ptr<PhuongThucThanhToan> pt, unique_ptr<KhuyenMai> km)
    : m_maDon(std::move(maDon)), m_maKhach(std::move(maKhach)),
      m_thanhToan(std::move(pt)), m_vanChuyen(std::move(vc)), m_khuyenMai(std::move(km)),
      m_trangThai(make_unique<DonMoi>()), m_khoangCachKm(khoangCachKm) {
    if (m_maDon.empty()) throw OrderException("Mã đơn hàng không được để trống");
    if (!(m_khoangCachKm > 0)) throw OrderException("Khoảng cách giao hàng phải lớn hơn 0");
    if (!m_vanChuyen) throw OrderException("Đơn hàng phải có đơn vị vận chuyển");
    if (!m_thanhToan) throw OrderException("Đơn hàng phải có phương thức thanh toán");
}

void DonHang::themChiTiet(unique_ptr<ChiTietDonHang> ct) {
    if (!ct) throw OrderException("Chi tiết đơn hàng không hợp lệ");
    if (m_daChot) throw OrderException("Đơn đã chốt, không thể thêm sản phẩm");
    if (ct->soLuong() <= 0) throw OrderException("Số lượng sản phẩm trong đơn phải lớn hơn 0");
    m_chiTiet.push_back(std::move(ct));
}

void DonHang::datDiaChiGiao(const DiaChi& dc) {
    if (catKhoangTrang(dc.hoTen).empty()) throw OrderException("Vui lòng nhập họ tên người nhận");
    if (!laSoDienThoaiHopLe(dc.soDienThoai)) throw OrderException("Số điện thoại không hợp lệ (cần 9-11 chữ số)");
    if (demKyTu(catKhoangTrang(dc.diaChi)) < hangso::DIA_CHI_TOI_THIEU)
        throw OrderException("Địa chỉ giao hàng quá ngắn (tối thiểu " + std::to_string(hangso::DIA_CHI_TOI_THIEU) + " ký tự)");
    m_diaChiGiao = dc;
}

void DonHang::chotDon() {
    if (m_daChot) throw OrderException("Đơn hàng đã được chốt");
    if (m_chiTiet.empty()) throw OrderException("Đơn hàng chưa có sản phẩm");
    if (m_diaChiGiao.hoTen.empty() && m_diaChiGiao.soDienThoai.empty() && m_diaChiGiao.diaChi.empty())
        throw OrderException("Đơn hàng chưa có địa chỉ giao hàng");
    if (m_khuyenMai && !m_khuyenMai->dieuKienHopLe(tomTat()))
        throw MaGiamGiaKhongHopLeException("Đơn hàng chưa đủ điều kiện áp dụng mã " + m_khuyenMai->ma());
    m_phiShip = tinhPhiShip();       // chưa chốt: tính từ bảng giá hiện tại
    m_daChot = true;
    m_thoiDiemDat = thoiGianHienTai();
    datTrangThai(make_unique<DonMoi>());   // ghi mục "Mới" đầu tiên vào lịch sử
}

// ============================================================================
// Big 5 (A-9): mọi thành viên unique_ptr đều được clone() => copy sâu
// ============================================================================
DonHang::~DonHang() = default;

DonHang::DonHang(const DonHang& o)
    : m_maDon(o.m_maDon), m_maKhach(o.m_maKhach),
      m_thanhToan(nhanBan(o.m_thanhToan)), m_vanChuyen(nhanBan(o.m_vanChuyen)),
      m_khuyenMai(nhanBan(o.m_khuyenMai)), m_trangThai(nhanBan(o.m_trangThai)),
      m_lichSu(o.m_lichSu), m_khoangCachKm(o.m_khoangCachKm), m_phiShip(o.m_phiShip), m_daChot(o.m_daChot),
      m_thoiDiemDat(o.m_thoiDiemDat), m_diaChiGiao(o.m_diaChiGiao),
      m_yeuCauTra(o.m_yeuCauTra ? make_unique<YeuCauTraHang>(*o.m_yeuCauTra) : nullptr),
      m_anhGiaoHang(o.m_anhGiaoHang), m_anhKhieuNai(o.m_anhKhieuNai), m_tinNhan(o.m_tinNhan),
      m_daDoc(o.m_daDoc), m_daHoanTien(o.m_daHoanTien), m_daTraPhiShip(o.m_daTraPhiShip),
      m_thoiDiemKetThuc(o.m_thoiDiemKetThuc) {
    m_chiTiet.reserve(o.m_chiTiet.size());
    for (const auto& ct : o.m_chiTiet) m_chiTiet.push_back(ct ? make_unique<ChiTietDonHang>(*ct) : nullptr);
}

// Copy-and-swap dạng "dựng bản sao trước, rồi chuyển vào": nếu việc sao chép ném ngoại lệ thì *this còn nguyên.
DonHang& DonHang::operator=(const DonHang& o) {
    if (this != &o) {
        DonHang tam(o);
        *this = std::move(tam);
    }
    return *this;
}

DonHang::DonHang(DonHang&& o) noexcept
    : m_maDon(std::move(o.m_maDon)), m_maKhach(std::move(o.m_maKhach)), m_chiTiet(std::move(o.m_chiTiet)),
      m_thanhToan(std::move(o.m_thanhToan)), m_vanChuyen(std::move(o.m_vanChuyen)),
      m_khuyenMai(std::move(o.m_khuyenMai)), m_trangThai(std::move(o.m_trangThai)),
      m_lichSu(std::move(o.m_lichSu)), m_khoangCachKm(o.m_khoangCachKm), m_phiShip(o.m_phiShip), m_daChot(o.m_daChot),
      m_thoiDiemDat(std::move(o.m_thoiDiemDat)), m_diaChiGiao(std::move(o.m_diaChiGiao)),
      m_yeuCauTra(std::move(o.m_yeuCauTra)), m_anhGiaoHang(std::move(o.m_anhGiaoHang)),
      m_anhKhieuNai(std::move(o.m_anhKhieuNai)), m_tinNhan(std::move(o.m_tinNhan)),
      m_daDoc(std::move(o.m_daDoc)), m_daHoanTien(std::move(o.m_daHoanTien)), m_daTraPhiShip(o.m_daTraPhiShip),
      m_thoiDiemKetThuc(std::move(o.m_thoiDiemKetThuc)) {
    // Đưa nguồn về trạng thái "rỗng" xác định (chuỗi nhỏ không tự rỗng sau khi move).
    o.m_maDon.clear(); o.m_maKhach.clear(); o.m_thoiDiemDat.clear(); o.m_thoiDiemKetThuc.clear();
    o.m_chiTiet.clear(); o.m_lichSu.clear(); o.m_anhGiaoHang.clear(); o.m_anhKhieuNai.clear();
    o.m_tinNhan.clear(); o.m_daDoc.clear(); o.m_daHoanTien.clear();
    o.m_diaChiGiao = DiaChi{};
    o.m_khoangCachKm = 0; o.m_phiShip = 0; o.m_daChot = false; o.m_daTraPhiShip = false;
}

DonHang& DonHang::operator=(DonHang&& o) noexcept {
    if (this == &o) return *this;
    m_maDon = std::move(o.m_maDon);               m_maKhach = std::move(o.m_maKhach);
    m_chiTiet = std::move(o.m_chiTiet);           m_thanhToan = std::move(o.m_thanhToan);
    m_vanChuyen = std::move(o.m_vanChuyen);       m_khuyenMai = std::move(o.m_khuyenMai);
    m_trangThai = std::move(o.m_trangThai);       m_lichSu = std::move(o.m_lichSu);
    m_khoangCachKm = o.m_khoangCachKm;            m_phiShip = o.m_phiShip;       m_daChot = o.m_daChot;
    m_thoiDiemDat = std::move(o.m_thoiDiemDat);   m_diaChiGiao = std::move(o.m_diaChiGiao);
    m_yeuCauTra = std::move(o.m_yeuCauTra);       m_anhGiaoHang = std::move(o.m_anhGiaoHang);
    m_anhKhieuNai = std::move(o.m_anhKhieuNai);   m_tinNhan = std::move(o.m_tinNhan);
    m_daDoc = std::move(o.m_daDoc);               m_daHoanTien = std::move(o.m_daHoanTien);
    m_daTraPhiShip = o.m_daTraPhiShip;            m_thoiDiemKetThuc = std::move(o.m_thoiDiemKetThuc);
    o.m_maDon.clear(); o.m_maKhach.clear(); o.m_thoiDiemDat.clear(); o.m_thoiDiemKetThuc.clear();
    o.m_chiTiet.clear(); o.m_lichSu.clear(); o.m_anhGiaoHang.clear(); o.m_anhKhieuNai.clear();
    o.m_tinNhan.clear(); o.m_daDoc.clear(); o.m_daHoanTien.clear();
    o.m_diaChiGiao = DiaChi{};
    o.m_khoangCachKm = 0; o.m_phiShip = 0; o.m_daChot = false; o.m_daTraPhiShip = false;
    return *this;
}

// ---- A-10 ----
bool DonHang::operator==(const DonHang& o) const { return m_maDon == o.m_maDon; }

ostream& operator<<(ostream& os, const DonHang& d) {
    os << "Đơn " << d.m_maDon << " | khách " << d.m_maKhach
       << " | " << (d.m_trangThai ? d.m_trangThai->ten() : string("(chưa có trạng thái)"))
       << " | " << d.m_chiTiet.size() << " dòng hàng"
       << " | tổng " << dinhDangTien(d.tinhTongThanhToan());
    return os;
}

// ============================================================================
// Tiền (A-11): mọi kết quả kết thúc bằng lamTronDong
// ============================================================================
TomTatGio DonHang::tomTat() const {
    TomTatGio t;
    for (const auto& ct : m_chiTiet) {
        t.tongTien += ct->tinhThanhTien();
        t.tongKhoiLuong += ct->tinhKhoiLuong();
        t.theoNguoiBan[ct->maNguoiBan()] += ct->tinhThanhTien();
    }
    t.tongTien = lamTronDong(t.tongTien);
    for (auto& cap : t.theoNguoiBan) cap.second = lamTronDong(cap.second);
    t.soNguoiBan = (int)t.theoNguoiBan.size();
    return t;
}

double DonHang::tinhTienHang() const { return tomTat().tongTien; }

double DonHang::tinhPhiShip() const {
    if (m_daChot) return m_phiShip;
    if (m_chiTiet.empty() || !m_vanChuyen) return 0;   // chưa có hàng: không có phí ship (xemTruoc giỏ rỗng)
    return lamTronDong(m_vanChuyen->tinhPhiShip(m_khoangCachKm, tomTat().tongKhoiLuong));
}

double DonHang::tinhGiamGia() const {
    if (!m_khuyenMai) return 0;
    return lamTronDong(m_khuyenMai->apDung(tomTat(), tinhPhiShip()));
}

double DonHang::tinhPhiXuLy() const {
    if (!m_thanhToan) return 0;
    return lamTronDong(m_thanhToan->tinhPhiXuLy(tinhTienHang() - tinhGiamGia() + tinhPhiShip()));
}

double DonHang::tinhTongThanhToan() const {
    return lamTronDong(tinhTienHang() - tinhGiamGia() + tinhPhiShip() + tinhPhiXuLy());
}

KetQuaTinhTien DonHang::tinhKetQua() const {
    KetQuaTinhTien kq;
    const TomTatGio t = tomTat();
    kq.tienHang = t.tongTien;
    kq.khoiLuong = t.tongKhoiLuong;
    kq.phiShip = tinhPhiShip();
    kq.giamGia = m_khuyenMai ? lamTronDong(m_khuyenMai->apDung(t, kq.phiShip)) : 0;
    kq.phiXuLy = m_thanhToan ? lamTronDong(m_thanhToan->tinhPhiXuLy(kq.tienHang - kq.giamGia + kq.phiShip)) : 0;
    kq.tongThanhToan = lamTronDong(kq.tienHang - kq.giamGia + kq.phiShip + kq.phiXuLy);
    kq.ngayDuKien = m_vanChuyen ? m_vanChuyen->uocTinhNgay(m_khoangCachKm) : 0;
    return kq;
}

double DonHang::tienThuHoCOD() const {
    if (m_thanhToan && m_thanhToan->ten() == pttt::COD) return tinhTongThanhToan();
    return 0;
}

// ============================================================================
// Trạng thái (A-12, A-14)
// ============================================================================
void DonHang::chuyenTrangThai() {
    if (daKetThuc()) throw ChuyenTrangThaiSaiException("Đơn " + m_maDon + " đã kết thúc, không thể đổi trạng thái");
    m_trangThai->chuyenTiep(*this);
}

void DonHang::datTrangThai(unique_ptr<TrangThaiDonHang> tt) {
    if (!tt) throw OrderException("Trạng thái mới không hợp lệ");
    if (daKetThuc()) throw ChuyenTrangThaiSaiException("Đơn " + m_maDon + " đã kết thúc, không thể đổi trạng thái");
    m_lichSu.push_back({ tt->ten(), thoiGianHienTai() });   // NƠI DUY NHẤT ghi lịch sử (B-5)
    m_trangThai = std::move(tt);
}

string DonHang::trangThai() const { return m_trangThai ? m_trangThai->ten() : string(); }

bool DonHang::daTungQuaTrangThai(const string& tenTT) const {
    return std::any_of(m_lichSu.begin(), m_lichSu.end(), [&](const LichSuTrangThai& l) { return l.trangThai == tenTT; });
}

// ============================================================================
// Giao hàng + ảnh (A-18, A-19)
// ============================================================================
void DonHang::xacNhanGiaoThanhCong(const AnhDinhKem& anh) {
    if (trangThai() != tt::DANG_GIAO)
        throw ChuyenTrangThaiSaiException("Chỉ xác nhận giao thành công khi đơn đang giao");
    if (catKhoangTrang(anh.duongDan).empty())
        throw OrderException("Cần có ảnh đơn hàng để xác nhận giao thành công");
    AnhDinhKem a = anh;
    if (a.thoiDiem.empty()) a.thoiDiem = thoiGianHienTai();
    m_anhGiaoHang.push_back(std::move(a));
    try {
        chuyenTrangThai();
    } catch (...) {
        m_anhGiaoHang.pop_back();     // không để lại ảnh nếu chuyển trạng thái thất bại
        throw;
    }
}

void DonHang::themAnhGiaoHang(const AnhDinhKem& a) {
    if (!daTungQuaTrangThai(tt::HOAN_TAT)) throw OrderException("Chỉ gửi ảnh giao hàng sau khi đơn đã giao");
    if (daKetThuc()) throw OrderException("Đơn đã kết thúc");
    if (catKhoangTrang(a.duongDan).empty()) throw OrderException("Ảnh không hợp lệ");
    AnhDinhKem x = a;
    if (x.thoiDiem.empty()) x.thoiDiem = thoiGianHienTai();
    m_anhGiaoHang.push_back(std::move(x));
}

void DonHang::themAnhKhieuNai(const AnhDinhKem& a) {
    if (!daTungQuaTrangThai(tt::HOAN_TAT)) throw OrderException("Chỉ gửi khiếu nại sau khi đơn đã giao");
    if (daKetThuc()) throw OrderException("Đơn đã kết thúc");
    if (catKhoangTrang(a.duongDan).empty()) throw OrderException("Ảnh không hợp lệ");
    AnhDinhKem x = a;
    if (x.thoiDiem.empty()) x.thoiDiem = thoiGianHienTai();
    m_anhKhieuNai.push_back(std::move(x));
}

double DonHang::traPhiShip(TaiKhoanVi& viShipper) {
    if (!canTraPhiShip()) return 0;
    viShipper.nhanTien(m_phiShip, "Phí giao đơn " + m_maDon);
    m_daTraPhiShip = true;            // đặt cờ SAU khi cộng tiền thành công => không cộng 2 lần, không mất phí nếu lỗi
    return m_phiShip;
}

// ============================================================================
// Trả hàng (A-17, A-20)
// ============================================================================
void DonHang::yeuCauTraHang(const string& lyDo) {
    if (trangThai() != tt::HOAN_TAT) throw ChuyenTrangThaiSaiException("Chỉ yêu cầu trả hàng khi đơn đã hoàn tất");
    if (daKetThuc()) throw ChuyenTrangThaiSaiException("Đơn đã kết thúc");
    if (m_yeuCauTra) throw OrderException("Đơn này đã có yêu cầu trả hàng");
    const string ly = catKhoangTrang(lyDo);
    if (ly.empty()) throw OrderException("Vui lòng nhập lý do trả hàng");
    m_yeuCauTra = make_unique<YeuCauTraHang>(YeuCauTraHang{ ly, thoiGianHienTai(), TrangThaiTraHang::Cho });
}

void DonHang::hoanHang() {
    if (!m_yeuCauTra || m_yeuCauTra->trangThai != TrangThaiTraHang::Cho)
        throw ChuyenTrangThaiSaiException("Không có yêu cầu trả hàng đang chờ duyệt");
    m_yeuCauTra->trangThai = TrangThaiTraHang::ChoShipperLay;   // đơn vẫn ở Hoàn tất cho đến khi shipper gửi hàng về shop
}

void DonHang::tuChoiTraHang() {
    if (!m_yeuCauTra || m_yeuCauTra->trangThai != TrangThaiTraHang::Cho)
        throw ChuyenTrangThaiSaiException("Không có yêu cầu trả hàng đang chờ duyệt");
    m_yeuCauTra->trangThai = TrangThaiTraHang::TuChoi;
}

void DonHang::shipperNhanDonTra(const string& maHang) {
    if (!m_yeuCauTra || m_yeuCauTra->trangThai != TrangThaiTraHang::ChoShipperLay)
        throw ChuyenTrangThaiSaiException("Đơn chưa ở trạng thái chờ shipper lấy hàng trả");
    if (maHang != hangVanChuyen())
        throw KhongCoQuyenException("Đơn trả thuộc hãng " + hangVanChuyen() + ", không thuộc hãng " + maHang);
    m_yeuCauTra->trangThai = TrangThaiTraHang::DangGuiVeShop;
}

void DonHang::shipperXacNhanGuiVeShop() {
    if (!m_yeuCauTra || m_yeuCauTra->trangThai != TrangThaiTraHang::DangGuiVeShop)
        throw ChuyenTrangThaiSaiException("Shipper chưa nhận đơn trả này");
    m_yeuCauTra->trangThai = TrangThaiTraHang::DaVeShop;
    try {
        chuyenTrangThai();            // HoanTat::chuyenTiep chỉ cho qua khi hangDaVeShop() => sang Trả hàng
    } catch (...) {
        m_yeuCauTra->trangThai = TrangThaiTraHang::DangGuiVeShop;
        throw;
    }
}

double DonHang::hoanTien(const string& maShop, TaiKhoanVi& viKhach) {
    if (trangThai() != tt::HOAN_TAT && trangThai() != tt::TRA_HANG)
        throw ChuyenTrangThaiSaiException("Chỉ hoàn tiền cho đơn đã giao hoặc đã trả hàng");
    if (daKetThuc()) throw ChuyenTrangThaiSaiException("Đơn đã kết thúc");
    if (dangXuLyTraHang()) throw ChuyenTrangThaiSaiException("Đơn đang xử lý trả hàng, chưa thể hoàn tiền");
    if (!coHangCuaShop(maShop)) throw OrderException("Đơn không có hàng của shop " + maShop);
    for (const auto& cap : m_daHoanTien)
        if (cungMaShop(cap.first, maShop)) throw OrderException("Đã hoàn tiền cho shop " + maShop + " rồi");
    double soTien = 0;
    for (const auto& ct : m_chiTiet)
        if (cungMaShop(ct->maNguoiBan(), maShop)) soTien += ct->tinhThanhTien();
    soTien = lamTronDong(soTien);
    viKhach.hoanTien(soTien, "Hoàn tiền đơn " + m_maDon + " từ shop " + inHoa(maShop));
    m_daHoanTien[inHoa(maShop)] = soTien;      // ghi sau khi ví đã nhận thành công
    return soTien;
}

bool DonHang::dangXuLyTraHang() const {
    if (!m_yeuCauTra) return false;
    const TrangThaiTraHang s = m_yeuCauTra->trangThai;
    return s == TrangThaiTraHang::Cho || s == TrangThaiTraHang::ChoShipperLay || s == TrangThaiTraHang::DangGuiVeShop;
}

bool DonHang::hangDaVeShop() const { return m_yeuCauTra && m_yeuCauTra->trangThai == TrangThaiTraHang::DaVeShop; }

// ============================================================================
// Kết thúc đơn (A-14)
// ============================================================================
void DonHang::ketThucDon() {
    if (daKetThuc()) throw ChuyenTrangThaiSaiException("Đơn đã kết thúc rồi");
    if (trangThai() != tt::HOAN_TAT && trangThai() != tt::TRA_HANG)
        throw ChuyenTrangThaiSaiException("Chỉ kết thúc đơn khi đã Hoàn tất hoặc Trả hàng");
    if (dangXuLyTraHang()) throw ChuyenTrangThaiSaiException("Đơn đang xử lý trả hàng, chưa thể kết thúc");
    m_thoiDiemKetThuc = thoiGianHienTai();
}

// ============================================================================
// Chat theo đơn (A-21). Khóa m_daDoc = tên đăng nhập viết thường (đúng quy ước trong NguoiDung.h)
// ============================================================================
bool DonHang::coHangCuaShop(const string& maShop) const {
    if (maShop.empty()) return false;
    return std::any_of(m_chiTiet.begin(), m_chiTiet.end(),
                       [&](const unique_ptr<ChiTietDonHang>& ct) { return cungMaShop(ct->maNguoiBan(), maShop); });
}

bool DonHang::thuocDon(const NguoiDung& nguoi) const {
    const string vt = nguoi.vaiTro();
    if (vt == vaitro::QUAN_LY)   return true;
    if (vt == vaitro::KHACH_HANG) return nguoi.ma() == m_maKhach;
    if (vt == vaitro::NGUOI_BAN)  return coHangCuaShop(nguoi.maShop());
    if (vt == vaitro::SHIPPER)    return !nguoi.maHang().empty() && nguoi.maHang() == hangVanChuyen();
    return false;
}

void DonHang::guiTinNhan(const NguoiDung& nguoiGui, const string& noiDung) {
    if (!thuocDon(nguoiGui)) throw KhongCoQuyenException("Bạn không thuộc đơn " + m_maDon + " nên không thể nhắn tin");
    const string nd = catKhoangTrang(noiDung);
    if (nd.empty()) throw OrderException("Tin nhắn không được để trống");
    if (demKyTu(nd) > hangso::TIN_NHAN_TOI_DA)
        throw OrderException("Tin nhắn tối đa " + std::to_string(hangso::TIN_NHAN_TOI_DA) + " ký tự");
    m_tinNhan.push_back({ nguoiGui.ma(), nguoiGui.vaiTro(), nd, thoiGianHienTai() });
    m_daDoc[inThuong(nguoiGui.tenDangNhap())] = m_tinNhan.size();   // tin của chính mình coi như đã đọc
}

int DonHang::soTinChuaDoc(const NguoiDung& nguoi) const {
    if (!thuocDon(nguoi)) return 0;
    size_t daDoc = 0;
    auto it = m_daDoc.find(inThuong(nguoi.tenDangNhap()));
    if (it != m_daDoc.end()) daDoc = it->second;
    int dem = 0;
    for (size_t i = daDoc; i < m_tinNhan.size(); ++i)
        if (m_tinNhan[i].maNguoiGui != nguoi.ma()) ++dem;
    return dem;
}

void DonHang::danhDauDaDoc(const NguoiDung& nguoi) {
    if (!thuocDon(nguoi)) return;
    m_daDoc[inThuong(nguoi.tenDangNhap())] = m_tinNhan.size();
}
