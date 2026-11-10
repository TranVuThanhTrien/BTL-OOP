#include "DonHangService.h"
#include "ThongKeService.h"
#include "OrderException.h"
#include <iomanip>
#include <limits>
#ifdef _WIN32
#include <windows.h>
#endif

namespace {
string docDong(const string& nhac) {
    std::cout << nhac;
    string s;
    std::getline(std::cin, s);
    return s;
}

int docSo(const string& nhac, int macDinh = 0) {
    string s = docDong(nhac);
    if (s.empty()) return macDinh;
    try { return std::stoi(s); } catch (...) { return macDinh; }
}

double docSoThuc(const string& nhac, double macDinh = 0) {
    string s = docDong(nhac);
    if (s.empty()) return macDinh;
    try { return std::stod(s); } catch (...) { return macDinh; }
}

void hienThiSanPham(const KhoSanPham& kho) {
    std::cout << "\n=== DANH SÁCH SẢN PHẨM ===\n";
    for (const auto& sp : kho.tatCa()) {
        std::cout << std::left << std::setw(7) << sp.ma() << std::setw(26) << sp.ten()
                  << "Shop " << sp.maNguoiBan() << " | " << dinhDangTien(sp.donGia())
                  << " | tồn " << sp.tonKho() << " | ★ " << sp.danhGia() << "\n";
    }
}

void hienThiGio(const GioHang& gio) {
    if (gio.rong()) { std::cout << "Giỏ hàng đang trống.\n"; return; }
    for (const auto& d : gio.dongHang()) {
        std::cout << d.first.ma() << " - " << d.first.ten() << " x" << d.second
                  << " = " << dinhDangTien(d.first.donGia() * d.second) << "\n";
    }
    std::cout << "Tạm tính: " << dinhDangTien(gio.tinhTongTien())
              << " | khối lượng: " << gio.tinhTongKhoiLuong() << " kg\n";
}

void hienThiDon(const DonHang& d) {
    std::cout << d << " | Hãng: " << d.hangVanChuyen()
              << " | Đặt lúc: " << d.thoiDiemDat() << "\n";
    for (const auto& ct : d.chiTiet())
        std::cout << "  - " << ct->maSP() << " x" << ct->soLuong()
                  << " = " << dinhDangTien(ct->tinhThanhTien()) << "\n";
}

bool dangNhap(AuthService& auth, NguoiDung*& nguoi, string& token) {
    const string user = docDong("Tên đăng nhập: ");
    const string pass = docDong("Mật khẩu: ");
    token = auth.dangNhap(user, pass);
    nguoi = &auth.xacThuc(token);
    std::cout << "Xin chào " << nguoi->hoTen() << " (" << nguoi->vaiTro() << ").\n";
    return true;
}

void hienThiDonCuaNguoi(DonHangService& donHang, const NguoiDung& nguoi) {
    auto ds = donHang.layDanhSach(nguoi);
    if (ds.empty()) { std::cout << "Chưa có đơn hàng.\n"; return; }
    for (const DonHang* d : ds) hienThiDon(*d);
}

void menuKhach(KhoSanPham& kho, DonHangService& donHang, NguoiDung& nguoi, GioHang& gio) {
    while (true) {
        std::cout << "\n=== KHÁCH HÀNG ===\n1. Xem sản phẩm\n2. Thêm vào giỏ\n3. Xem giỏ\n"
                     "4. Thanh toán\n5. Đơn của tôi\n6. Ví của tôi\n7. Đăng xuất\n0. Thoát\n";
        int chon = docSo("Chọn: ");
        try {
            if (chon == 0) return;
            if (chon == 1) hienThiSanPham(kho);
            else if (chon == 2) {
                string ma = docDong("Mã sản phẩm: ");
                int sl = docSo("Số lượng: ", 1);
                gio.themSanPham(kho.lay(ma), sl);
                std::cout << "Đã cập nhật giỏ hàng.\n";
            } else if (chon == 3) hienThiGio(gio);
            else if (chon == 4) {
                if (gio.rong()) { std::cout << "Giỏ hàng trống.\n"; continue; }
                double km = docSoThuc("Khoảng cách giao hàng (km): ", 10);
                string hangVanChuyen = docDong("Hãng (GHN/GHTK/ViettelPost, Enter = rẻ nhất): ");
                if (hangVanChuyen.empty()) hangVanChuyen = timHangReNhat(km, gio.tinhTongKhoiLuong());
                string pt = docDong("Thanh toán COD/TheTinDung/ViDienTu [COD]: ");
                if (pt.empty()) pt = pttt::COD;
                string maKM = docDong("Mã khuyến mãi (Enter nếu không có): ");
                auto kq = donHang.xemTruoc(gio, hangVanChuyen, pt, maKM, km);
                std::cout << "Tiền hàng " << dinhDangTien(kq.tienHang) << " | giảm " << dinhDangTien(kq.giamGia)
                          << " | ship " << dinhDangTien(kq.phiShip) << " | phí xử lý " << dinhDangTien(kq.phiXuLy)
                          << " | tổng " << dinhDangTien(kq.tongThanhToan) << "\n";
                if (docDong("Xác nhận đặt hàng? (y/N): ") != "y") continue;
                DiaChi dc;
                if (auto* kh = dynamic_cast<KhachHang*>(&nguoi)) dc = kh->diaChi();
                if (dc.hoTen.empty()) dc.hoTen = nguoi.hoTen();
                if (dc.soDienThoai.empty()) dc.soDienThoai = docDong("Số điện thoại: ");
                if (dc.diaChi.empty()) dc.diaChi = docDong("Địa chỉ giao hàng: ");
                const DonHang& d = donHang.taoDon(nguoi, gio, dc, hangVanChuyen, pt, maKM, km);
                hienThiDon(d);
            } else if (chon == 5) hienThiDonCuaNguoi(donHang, nguoi);
            else if (chon == 6) {
                const auto* vi = nguoi.vi();
                std::cout << "Số dư: " << (vi ? dinhDangTien(vi->soDu()) : "Không có ví") << "\n";
                if (vi) for (const auto& gd : vi->lichSu())
                    std::cout << "  " << gd.thoiDiem << " | " << gd.moTa << " | " << dinhDangTien(gd.soTien) << "\n";
            } else if (chon == 7) return;
            else std::cout << "Lựa chọn không hợp lệ.\n";
        } catch (const OrderException& e) { std::cout << "Lỗi: " << e.what() << "\n"; }
    }
}

void menuNhanVien(DonHangService& donHang, NguoiDung& nguoi) {
    while (true) {
        std::cout << "\n=== " << nguoi.vaiTro() << " ===\n1. Xem đơn\n2. Chuyển trạng thái đơn\n"
                     "3. Thống kê (quản lý)\n4. Đăng xuất\n0. Thoát\n";
        int chon = docSo("Chọn: ");
        try {
            if (chon == 0) return;
            if (chon == 1) hienThiDonCuaNguoi(donHang, nguoi);
            else if (chon == 2) {
                string ma = docDong("Mã đơn: ");
                donHang.chuyenTrangThai(ma, nguoi);
                std::cout << "Đã chuyển trạng thái thành công.\n";
            } else if (chon == 3 && nguoi.vaiTro() == vaitro::QUAN_LY) {
                const auto& ds = donHang.tatCaDon();
                std::cout << "Tỷ lệ giao thành công: " << ThongKeService::tyLeGiaoThanhCong(ds)
                          << "% | tỷ lệ trả hàng: " << ThongKeService::tyLeHoanHang(ds) << "%\n";
                for (const auto& kv : ThongKeService::doanhThuTheoNguoiBan(ds))
                    std::cout << kv.first << ": " << dinhDangTien(kv.second) << "\n";
            } else if (chon == 4) return;
            else std::cout << "Lựa chọn không hợp lệ.\n";
        } catch (const OrderException& e) { std::cout << "Lỗi: " << e.what() << "\n"; }
    }
}
}

int runConsole() {
#ifdef _WIN32
    // Nội dung giao diện và dữ liệu nguồn đều dùng UTF-8.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try {
        KhoSanPham kho(DataStore::docSanPham("data/san_pham.json"));
        AuthService auth("data/tai_khoan.json");
        DonHangService donHang(kho, auth, "data/don_hang.json");
        string maGio = donHang.taoMaGio();
        GioHang& gio = donHang.gio(maGio);
        std::cout << "ĐT08 - Cửa hàng trực tuyến (console)\n"
                     "Tài khoản demo: khach/123, TT/123, DT/123, NS/123, GHN/123, GHTK/123, VTP/123, quanly/admin\n";
        while (true) {
            std::cout << "\n1. Xem sản phẩm\n2. Đăng nhập\n0. Thoát\n";
            int chon = docSo("Chọn: ");
            if (!std::cin || chon == 0) break;
            try {
                if (chon == 1) hienThiSanPham(kho);
                else if (chon == 2) {
                    NguoiDung* nguoi = nullptr;
                    string token;
                    dangNhap(auth, nguoi, token);
                    if (nguoi->vaiTro() == vaitro::KHACH_HANG) menuKhach(kho, donHang, *nguoi, gio);
                    else menuNhanVien(donHang, *nguoi);
                    auth.dangXuat(token);
                } else std::cout << "Lựa chọn không hợp lệ.\n";
            } catch (const OrderException& e) { std::cout << "Lỗi: " << e.what() << "\n"; }
        }
        auth.luu();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Không khởi tạo được ứng dụng: " << e.what() << "\n";
        return 1;
    }
}

int runHttpServer();

int main(int argc, char** argv) {
    if (argc > 1 && string(argv[1]) == "--console") return runConsole();
    return runHttpServer();
}
