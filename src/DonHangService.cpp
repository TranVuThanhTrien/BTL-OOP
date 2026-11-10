#include "DonHangService.h"
#include "OrderException.h"
#include <filesystem>

// Người phụ trách: D | nhánh feature/web-api | LỚP 6
DonHangService::DonHangService(KhoSanPham& kho, AuthService& auth, string duongDanDon)
    : m_kho(kho), m_auth(auth), m_duongDanDon(std::move(duongDanDon)) {
    const bool tepDonDaTonTai = !m_duongDanDon.empty() && std::filesystem::exists(m_duongDanDon);
    try {
        m_dsDon = DataStore::docDonHang(m_duongDanDon);
    } catch (const FileDataException&) {
        if (tepDonDaTonTai) throw;
        m_dsDon.clear();
    }
    m_demDon = static_cast<int>(m_dsDon.size());
}

void DonHangService::luu() const {
    if (!m_duongDanDon.empty()) DataStore::luuDonHang(m_dsDon, m_duongDanDon);
    DataStore::luuSanPham(m_kho.tatCa(), "data/san_pham.json");
    m_auth.luu();
}

DonHang& DonHangService::tim(const string& maDon) {
    for (auto& d : m_dsDon) {
        if (d.maDon() == maDon) return d;
    }
    throw OrderException("Không tìm thấy đơn hàng: " + maDon);
}

string DonHangService::taoMaGio() {
    static int s_gioSeq = 0;
    ++s_gioSeq;
    string ma = "GIO" + std::to_string(s_gioSeq);
    m_gio[ma] = GioHang{};
    return ma;
}

GioHang& DonHangService::gio(const string& maGio) {
    return m_gio[maGio];
}

KetQuaTinhTien DonHangService::xemTruoc(const GioHang& gio, const string& hang, const string& pttt, const string& maKM, double km) const {
    if (gio.rong()) {
        throw HetHangException("Giỏ hàng trống");
    }
    unique_ptr<KhuyenMai> kmPtr = maKM.empty() ? nullptr : taoKhuyenMai(maKM);
    DonHang tam("PREVIEW", "khach", km, taoDonVi(hang), taoPhuongThuc(pttt, nullptr), std::move(kmPtr));
    for (const auto& d : gio.dongHang()) {
        tam.themChiTiet(make_unique<ChiTietDonHang>(
            d.first.ma(), d.first.maNguoiBan(), d.second, d.first.donGia(), d.first.khoiLuong()));
    }
    return tam.tinhKetQua();
}

const DonHang& DonHangService::taoDon(NguoiDung& khach, GioHang& gio, const DiaChi& diaChi,
                                      const string& hang, const string& pttt, const string& maKM, double km) {
    m_auth.yeuCauQuyen(khach, quyen::DAT_HANG);
    if (gio.rong()) {
        throw HetHangException("Giỏ hàng trống");
    }
    // 1. Kiểm tra tồn kho thực tế trong KhoSanPham
    for (const auto& d : gio.dongHang()) {
        const SanPham& spKho = m_kho.lay(d.first.ma());
        if (spKho.tonKho() < d.second) {
            throw HetHangException(spKho.ten() + " chỉ còn " + std::to_string(spKho.tonKho()) + " sản phẩm");
        }
    }
    // 2. Dựng đơn hàng
    char maBuf[16];
    std::snprintf(maBuf, sizeof maBuf, "DH%03d", m_demDon + 1);
    string maDon(maBuf);

    unique_ptr<KhuyenMai> kmPtr = maKM.empty() ? nullptr : taoKhuyenMai(maKM);
    unique_ptr<PhuongThucThanhToan> ptPtr = taoPhuongThuc(pttt, khach.vi());
    if (auto* viPtr = dynamic_cast<ViDienTu*>(ptPtr.get())) {
        viPtr->datMoTa("Thanh toán đơn " + maDon);
    }

    DonHang don(maDon, khach.tenDangNhap(), km, taoDonVi(hang), std::move(ptPtr), std::move(kmPtr));
    for (const auto& d : gio.dongHang()) {
        const SanPham& spKho = m_kho.lay(d.first.ma());
        don.themChiTiet(make_unique<ChiTietDonHang>(
            spKho.ma(), spKho.maNguoiBan(), d.second, spKho.donGia(), spKho.khoiLuong()));
    }
    don.datDiaChiGiao(diaChi);
    don.chotDon();

    // 3. Thanh toán trước khi trừ kho để đảm bảo tính nguyên tử (atomicity)
    double tong = don.tinhTongThanhToan();
    const_cast<PhuongThucThanhToan*>(don.thanhToan())->thanhToan(tong);

    // 4. Trừ tồn kho và cộng số lượng đã bán
    for (const auto& d : gio.dongHang()) {
        SanPham& spKho = m_kho.lay(d.first.ma());
        spKho.giamTon(d.second);
        spKho.congDaBan(d.second);
    }
    if (auto* kh = dynamic_cast<KhachHang*>(&khach)) {
        kh->datDiaChi(diaChi);
    }

    ++m_demDon;
    m_dsDon.push_back(std::move(don));
    gio.lamTrong();
    luu();
    return m_dsDon.back();
}

vector<const DonHang*> DonHangService::layDanhSach(const NguoiDung& nguoiGoi) const {
    vector<const DonHang*> kq;
    for (const auto& d : m_dsDon) {
        if (d.thuocDon(nguoiGoi)) {
            kq.push_back(&d);
        }
    }
    return kq;
}

const DonHang& DonHangService::layChiTiet(const string& maDon, const NguoiDung& nguoiGoi) const {
    for (const auto& d : m_dsDon) {
        if (d.maDon() == maDon) {
            if (!d.thuocDon(nguoiGoi)) {
                throw KhongCoQuyenException("Bạn không có quyền xem đơn hàng này");
            }
            return d;
        }
    }
    throw OrderException("Không tìm thấy đơn hàng: " + maDon);
}

vector<const DonHang*> DonHangService::layDonGiaoHang(const NguoiDung& shipper) const {
    m_auth.yeuCauQuyen(shipper, quyen::GIAO_HANG);
    vector<const DonHang*> kq;
    for (const auto& d : m_dsDon) {
        if (d.hangVanChuyen() == shipper.maHang()) {
            kq.push_back(&d);
        }
    }
    return kq;
}

void DonHangService::chuyenTrangThai(const string& maDon, const NguoiDung& nguoiGoi) {
    DonHang& d = tim(maDon);
    const string vt = nguoiGoi.vaiTro();
    const string st = d.trangThai();

    if (vt == vaitro::KHACH_HANG) {
        throw KhongCoQuyenException("Khách hàng không được chuyển trạng thái đơn hàng");
    } else if (vt == vaitro::NGUOI_BAN) {
        if (!d.coHangCuaShop(nguoiGoi.maShop()) || st != tt::MOI) {
            throw KhongCoQuyenException("Người bán chỉ được xác nhận đơn Mới của shop mình");
        }
    } else if (vt == vaitro::SHIPPER) {
        if (d.hangVanChuyen() != nguoiGoi.maHang()) {
            throw KhongCoQuyenException("Shipper chỉ được xử lý đơn thuộc hãng của mình");
        }
        if (st == tt::DANG_GIAO && !d.coAnhGiaoHang()) {
            throw ChuyenTrangThaiSaiException("Cần gửi ảnh xác nhận giao hàng để hoàn tất đơn");
        }
        if (st != tt::XAC_NHAN && st != tt::DANG_GIAO) {
            throw KhongCoQuyenException("Shipper không được thực hiện bước chuyển trạng thái này");
        }
    } else if (vt == vaitro::QUAN_LY) {
        if (st == tt::HOAN_TAT || st == tt::TRA_HANG) {
            throw KhongCoQuyenException("Quản lý chỉ chuyển các bước trước Hoàn tất");
        }
    }
    d.chuyenTrangThai();
    if (d.canTraPhiShip()) {
        if (NguoiDung* sp = m_auth.timShipperCuaHang(d.hangVanChuyen())) {
            if (sp->vi()) d.traPhiShip(*sp->vi());
        }
    }
    luu();
}

void DonHangService::xacNhanGiao(const string& maDon, NguoiDung& shipper, const string& anhBase64) {
    m_auth.yeuCauQuyen(shipper, quyen::GIAO_HANG);
    DonHang& d = tim(maDon);
    if (d.hangVanChuyen() != shipper.maHang()) {
        throw KhongCoQuyenException("Chỉ shipper đúng hãng mới được xác nhận giao");
    }
    if (d.trangThai() != tt::DANG_GIAO) {
        throw ChuyenTrangThaiSaiException("Chỉ xác nhận giao khi đơn đang ở trạng thái Đang giao");
    }
    string duongDan = DataStore::luuAnhBase64("data/anh", maDon + "_giao", anhBase64);
    d.xacNhanGiaoThanhCong(AnhDinhKem{duongDan, "Ảnh xác nhận giao hàng", thoiGianHienTai()});
    if (shipper.vi()) {
        d.traPhiShip(*shipper.vi());
    }
    luu();
}

// Nếu đơn đang ở trạng thái "Mới" khi gọi ketThuc:
// 1. Hoàn trả lại tồn kho (tangTon) cho từng sản phẩm trong KhoSanPham.
// 2. Nếu khách đã trả trước qua Ví điện tử hoặc Thẻ tín dụng, hoàn lại toàn bộ tổng tiền thanh toán vào ví khách.
void DonHangService::ketThuc(const string& maDon, const NguoiDung& nguoiGoi) {
    DonHang& d = tim(maDon);
    if (!d.thuocDon(nguoiGoi) || nguoiGoi.vaiTro() == vaitro::SHIPPER) {
        throw KhongCoQuyenException("Chỉ khách chủ đơn, người bán hoặc quản lý được kết thúc/hủy đơn");
    }
    bool laHuyDonMoi = (d.trangThai() == tt::MOI && !d.daKetThuc());
    d.ketThucDon();

    if (laHuyDonMoi) {
        // 1. Hoàn lại tồn kho cho từng mặt hàng
        for (const auto& ct : d.chiTiet()) {
            try {
                SanPham& sp = m_kho.lay(ct->maSP());
                sp.tangTon(ct->soLuong());
            } catch (const OrderException&) {
                // Sản phẩm có thể đã bị xóa khỏi kho, bỏ qua
            }
        }
        // 2. Hoàn lại tiền cho khách nếu đơn đã thanh toán trả trước (Ví / Thẻ)
        if (d.thanhToan() && d.thanhToan()->ten() != pttt::COD) {
            if (NguoiDung* kh = m_auth.tim(d.maKhach())) {
                if (kh->vi()) {
                    double tongHoan = d.tinhTongThanhToan();
                    if (tongHoan > 0) {
                        kh->vi()->hoanTien(tongHoan, "Hoàn tiền hủy đơn " + d.maDon());
                    }
                }
            }
        }
    }
    luu();
}

void DonHangService::traHang(const string& maDon, const NguoiDung& khach, const string& lyDo) {
    DonHang& d = tim(maDon);
    if (khach.vaiTro() != vaitro::KHACH_HANG || !d.thuocDon(khach)) {
        throw KhongCoQuyenException("Chỉ khách chủ đơn mới được yêu cầu trả hàng");
    }
    d.yeuCauTraHang(lyDo);
    luu();
}

void DonHangService::hoanHang(const string& maDon, const NguoiDung& nguoiGoi) {
    DonHang& d = tim(maDon);
    if ((nguoiGoi.vaiTro() != vaitro::NGUOI_BAN || !d.coHangCuaShop(nguoiGoi.maShop())) &&
        nguoiGoi.vaiTro() != vaitro::QUAN_LY) {
        throw KhongCoQuyenException("Chỉ người bán của đơn hoặc quản lý mới được duyệt hoàn hàng");
    }
    d.hoanHang();
    luu();
}

void DonHangService::tuChoiTraHang(const string& maDon, const NguoiDung& nguoiGoi) {
    DonHang& d = tim(maDon);
    if ((nguoiGoi.vaiTro() != vaitro::NGUOI_BAN || !d.coHangCuaShop(nguoiGoi.maShop())) &&
        nguoiGoi.vaiTro() != vaitro::QUAN_LY) {
        throw KhongCoQuyenException("Chỉ người bán của đơn hoặc quản lý mới được từ chối trả hàng");
    }
    d.tuChoiTraHang();
    luu();
}

void DonHangService::hoanTien(const string& maDon, const NguoiDung& nguoiGoi, const string& maShop) {
    DonHang& d = tim(maDon);
    string shop = maShop.empty() ? nguoiGoi.maShop() : inHoa(maShop);
    if (nguoiGoi.vaiTro() == vaitro::NGUOI_BAN) {
        if (nguoiGoi.maShop() != shop || !d.coHangCuaShop(shop)) {
            throw KhongCoQuyenException("Người bán chỉ được hoàn tiền phần hàng của shop mình");
        }
    } else if (nguoiGoi.vaiTro() != vaitro::QUAN_LY) {
        throw KhongCoQuyenException("Chỉ người bán hoặc quản lý được hoàn tiền");
    }
    NguoiDung* kh = m_auth.tim(d.maKhach());
    if (!kh || !kh->vi()) {
        throw ThanhToanThatBaiException("Không tìm thấy ví của khách hàng để hoàn tiền");
    }
    d.hoanTien(shop, *kh->vi());
    luu();
}

void DonHangService::nhanDonTra(const string& maDon, const NguoiDung& shipper) {
    m_auth.yeuCauQuyen(shipper, quyen::GIAO_HANG);
    DonHang& d = tim(maDon);
    d.shipperNhanDonTra(shipper.maHang());
    luu();
}

void DonHangService::xacNhanGuiVeShop(const string& maDon, const NguoiDung& shipper) {
    m_auth.yeuCauQuyen(shipper, quyen::GIAO_HANG);
    DonHang& d = tim(maDon);
    if (d.hangVanChuyen() != shipper.maHang()) {
        throw KhongCoQuyenException("Chỉ shipper đúng hãng mới được xác nhận gửi về shop");
    }
    d.shipperXacNhanGuiVeShop();
    luu();
}

void DonHangService::guiAnh(const string& maDon, const NguoiDung& nguoiGoi,
                            const string& loai, const string& anhBase64, const string& moTa) {
    DonHang& d = tim(maDon);
    if (loai == "giao") {
        if (nguoiGoi.vaiTro() != vaitro::SHIPPER || d.hangVanChuyen() != nguoiGoi.maHang()) {
            throw KhongCoQuyenException("Chỉ shipper của đơn mới gửi được ảnh tình trạng hàng");
        }
        if (d.trangThai() != tt::HOAN_TAT && d.trangThai() != tt::TRA_HANG)
            throw ChuyenTrangThaiSaiException("Chỉ gửi thêm ảnh tình trạng hàng sau khi đã giao");
        string duongDan = DataStore::luuAnhBase64("data/anh", maDon + "_" + loai, anhBase64);
        AnhDinhKem anh{duongDan, moTa, thoiGianHienTai()};
        d.themAnhGiaoHang(anh);
    } else if (loai == "khieuNai") {
        if (nguoiGoi.vaiTro() != vaitro::KHACH_HANG || !d.thuocDon(nguoiGoi)) {
            throw KhongCoQuyenException("Chỉ khách chủ đơn mới gửi được ảnh khiếu nại");
        }
        if (d.trangThai() != tt::HOAN_TAT && d.trangThai() != tt::TRA_HANG)
            throw ChuyenTrangThaiSaiException("Chỉ gửi ảnh khiếu nại khi đơn đã giao");
        string duongDan = DataStore::luuAnhBase64("data/anh", maDon + "_" + loai, anhBase64);
        AnhDinhKem anh{duongDan, moTa, thoiGianHienTai()};
        d.themAnhKhieuNai(anh);
    } else {
        throw OrderException("Loại ảnh không hợp lệ: " + loai);
    }
    luu();
}

void DonHangService::guiTinNhan(const string& maDon, const NguoiDung& nguoiGoi, const string& noiDung) {
    DonHang& d = tim(maDon);
    d.guiTinNhan(nguoiGoi, noiDung);
    luu();
}

vector<TinNhan> DonHangService::layTinNhan(const string& maDon, const NguoiDung& nguoiGoi) {
    DonHang& d = tim(maDon);
    if (!d.thuocDon(nguoiGoi)) {
        throw KhongCoQuyenException("Bạn không thuộc đơn hàng này");
    }
    d.danhDauDaDoc(nguoiGoi);
    luu();
    return d.tinNhan();
}

int DonHangService::tongTinChuaDoc(const NguoiDung& nguoi) const {
    int tong = 0;
    for (const auto& d : m_dsDon) {
        tong += d.soTinChuaDoc(nguoi);
    }
    return tong;
}
