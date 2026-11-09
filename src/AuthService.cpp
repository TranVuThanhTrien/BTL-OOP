#include "AuthService.h"
#include "OrderException.h"
#include <random>
#include <filesystem>

// Người phụ trách: D | nhánh feature/web-api | LỚP 6
AuthService::AuthService(string duongDanTaiKhoan)
    : m_duongDanTaiKhoan(std::move(duongDanTaiKhoan)) {
    const bool taiKhoanDaTonTai = !m_duongDanTaiKhoan.empty() && std::filesystem::exists(m_duongDanTaiKhoan);
    try {
        m_dsNguoiDung = DataStore::docTaiKhoan(m_duongDanTaiKhoan);
    } catch (const FileDataException&) {
        if (taiKhoanDaTonTai) throw;
        m_dsNguoiDung.clear();
    }
    if (m_dsNguoiDung.empty()) {
        // Tạo tài khoản demo mặc định theo TODO.md
        auto kh1 = make_unique<KhachHang>("KH01", "khach", bamMatKhau("123"), "Nguyễn Văn An");
        kh1->vi()->napTien(800000, "Chuyển khoản ngân hàng");
        kh1->datDiaChi(DiaChi{"Nguyễn Văn An", "0901234567", "25 Lê Văn Sỹ, Quận 3, TP.HCM"});
        m_dsNguoiDung.push_back(std::move(kh1));

        auto kh2 = make_unique<KhachHang>("KH02", "lan", bamMatKhau("123"), "Trần Thị Lan");
        kh2->vi()->napTien(300000, "Chuyển khoản ngân hàng");
        kh2->datDiaChi(DiaChi{"Trần Thị Lan", "0912345678", "88 Cách Mạng Tháng 8, Quận 10, TP.HCM"});
        m_dsNguoiDung.push_back(std::move(kh2));

        auto kh3 = make_unique<KhachHang>("KH03", "minh", bamMatKhau("123"), "Lê Quốc Minh");
        kh3->vi()->napTien(150000, "Chuyển khoản ngân hàng");
        kh3->datDiaChi(DiaChi{"Lê Quốc Minh", "0987654321", "15 Phan Xích Long, Phú Nhuận, TP.HCM"});
        m_dsNguoiDung.push_back(std::move(kh3));

        auto sTT = make_unique<NguoiBan>("NB01", "TT", bamMatKhau("123"), "Shop Thời Trang");
        sTT->datDiaChiLayHang(DiaChi{"Shop Thời Trang", "0909000111", "12 Nguyễn Trãi, Quận 1, TP.HCM"});
        m_dsNguoiDung.push_back(std::move(sTT));

        auto sDT = make_unique<NguoiBan>("NB02", "DT", bamMatKhau("123"), "Shop Điện Tử");
        sDT->datDiaChiLayHang(DiaChi{"Shop Điện Tử", "0909000222", "45 Lê Lợi, Quận 1, TP.HCM"});
        m_dsNguoiDung.push_back(std::move(sDT));

        auto sNS = make_unique<NguoiBan>("NB03", "NS", bamMatKhau("123"), "Nhà Sách");
        sNS->datDiaChiLayHang(DiaChi{"Nhà Sách", "0909000333", "7 Pasteur, Quận 3, TP.HCM"});
        m_dsNguoiDung.push_back(std::move(sNS));

        m_dsNguoiDung.push_back(make_unique<ShipperVanChuyen>("SP01", "GHN",  bamMatKhau("123"), "Shipper GHN",         hang::GHN));
        m_dsNguoiDung.push_back(make_unique<ShipperVanChuyen>("SP02", "GHTK", bamMatKhau("123"), "Shipper GHTK",        hang::GHTK));
        m_dsNguoiDung.push_back(make_unique<ShipperVanChuyen>("SP03", "VTP",  bamMatKhau("123"), "Shipper ViettelPost", hang::VIETTEL_POST));
        m_dsNguoiDung.push_back(make_unique<QuanLy>("QL01", "quanly", bamMatKhau("admin"), "Trần Minh Quân"));
        luu();
    }
}

string AuthService::dangNhap(const string& user, const string& pass) {
    NguoiDung* nd = tim(user);
    if (!nd || !nd->kiemTraMatKhau(pass)) {
        throw DangNhapThatBaiException("Sai tên đăng nhập hoặc mật khẩu");
    }
    if (nd->biKhoa()) {
        throw DangNhapThatBaiException("Tài khoản đã bị khóa, vui lòng liên hệ quản lý");
    }
    std::random_device rd;
    std::mt19937_64 gen(rd());
    char buf[33];
    std::snprintf(buf, sizeof buf, "%016llx%016llx",
                  static_cast<unsigned long long>(gen()),
                  static_cast<unsigned long long>(gen()));
    string token(buf);
    m_phien[token] = inThuong(nd->tenDangNhap());
    return token;
}

void AuthService::dangXuat(const string& token) {
    m_phien.erase(token);
}

NguoiDung& AuthService::xacThuc(const string& token) {
    if (token.empty()) {
        throw DangNhapThatBaiException("Vui lòng đăng nhập để thực hiện thao tác này");
    }
    auto it = m_phien.find(token);
    if (it == m_phien.end()) {
        throw DangNhapThatBaiException("Phiên đăng nhập không hợp lệ hoặc đã hết hạn");
    }
    NguoiDung* nd = tim(it->second);
    if (!nd || nd->biKhoa()) {
        throw DangNhapThatBaiException("Tài khoản không tồn tại hoặc đã bị khóa");
    }
    return *nd;
}

void AuthService::dangKyKhachHang(const string& hoTen, const string& user, const string& pass) {
    if (hoTen.empty() || user.size() < 3 || pass.size() < 3) {
        throw OrderException("Nhập đủ họ tên, tên đăng nhập và mật khẩu (từ 3 ký tự)");
    }
    if (tim(user)) {
        throw OrderException("Tên đăng nhập đã tồn tại");
    }
    string ma = "KH" + std::to_string(m_dsNguoiDung.size() + 1);
    m_dsNguoiDung.push_back(make_unique<KhachHang>(ma, user, bamMatKhau(pass), hoTen));
    luu();
}

void AuthService::dangKyNguoiBan(const string& tenShop, const string& maShop, const string& pass) {
    if (tenShop.empty() || pass.size() < 3) {
        throw OrderException("Nhập đủ tên shop và mật khẩu (từ 3 ký tự)");
    }
    if (!laMaShopHopLe(maShop)) {
        throw OrderException("Mã shop gồm 2-4 chữ cái, ví dụ DT, TT, NS");
    }
    string maHoa = inHoa(maShop);
    if (tim(maHoa)) {
        throw OrderException("Mã shop (tên đăng nhập) đã tồn tại");
    }
    string ma = "NB" + std::to_string(m_dsNguoiDung.size() + 1);
    m_dsNguoiDung.push_back(make_unique<NguoiBan>(ma, maHoa, bamMatKhau(pass), tenShop));
    luu();
}

void AuthService::yeuCauQuyen(const NguoiDung& nd, const string& chucNang) const {
    if (!nd.coQuyen(chucNang)) {
        throw KhongCoQuyenException("Tài khoản không có quyền thực hiện chức năng: " + chucNang);
    }
}

void AuthService::khoaTaiKhoan(const NguoiDung& nguoiGoi, const string& user, bool khoa) {
    yeuCauQuyen(nguoiGoi, quyen::QUAN_LY_TAI_KHOAN);
    NguoiDung* nd = tim(user);
    if (!nd) {
        throw OrderException("Không tìm thấy tài khoản: " + user);
    }
    if (inThuong(nd->tenDangNhap()) == inThuong(nguoiGoi.tenDangNhap())) {
        throw OrderException("Không thể tự khóa tài khoản của chính mình");
    }
    if (khoa) {
        nd->khoa();
        string uLow = inThuong(nd->tenDangNhap());
        for (auto it = m_phien.begin(); it != m_phien.end();) {
            if (it->second == uLow) it = m_phien.erase(it);
            else ++it;
        }
    } else {
        nd->moKhoa();
    }
    luu();
}

NguoiDung* AuthService::tim(const string& tenDangNhap) {
    string u = inThuong(tenDangNhap);
    for (auto& nd : m_dsNguoiDung) {
        if (nd && (inThuong(nd->tenDangNhap()) == u || inThuong(nd->ma()) == u)) {
            return nd.get();
        }
    }
    return nullptr;
}

NguoiDung* AuthService::timShipperCuaHang(const string& maHang) {
    for (auto& nd : m_dsNguoiDung) {
        if (nd && nd->vaiTro() == vaitro::SHIPPER && nd->maHang() == maHang) {
            return nd.get();
        }
    }
    return nullptr;
}

void AuthService::luu() const {
    if (m_duongDanTaiKhoan.empty()) return;
    DataStore::luuTaiKhoan(m_dsNguoiDung, m_duongDanTaiKhoan);
}
