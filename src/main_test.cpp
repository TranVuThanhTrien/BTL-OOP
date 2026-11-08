#include "DataStore.h"
#include "ThongKeService.h"
#include "OrderException.h"
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

namespace {
int soTest = 0;
void kiemTra(bool dung, const char* moTa) {
    ++soTest;
    if (!dung) {
        std::cerr << "[FAIL] " << moTa << '\n';
        std::exit(1);
    }
}

template<class Exception, class Fn>
void phaiNem(Fn fn, const char* moTa) {
    bool daNem = false;
    try { fn(); } catch (const Exception&) { daNem = true; }
    kiemTra(daNem, moTa);
}

DonHang taoDonMau(string ma, string khach = "khach") {
    DonHang don(std::move(ma), std::move(khach), 10, taoDonVi(hang::GHN), taoPhuongThuc(pttt::COD), nullptr);
    don.themChiTiet(make_unique<ChiTietDonHang>("AT01", "TT", 2, 120000, 0.3));
    don.themChiTiet(make_unique<ChiTietDonHang>("ST10", "NS", 1, 45000, 0.2));
    don.datDiaChiGiao(DiaChi{"Nguyễn Văn An", "0901234567", "25 Le Van Sy, Quan 3"});
    don.chotDon();
    return don;
}

void testBig5VaState() {
    DonHang goc = taoDonMau("DH001");
    DonHang banSao(goc);
    kiemTra(goc == banSao, "copy giữ mã đơn");
    banSao.themChiTiet(make_unique<ChiTietDonHang>("QJ02", "TT", 1, 280000, 0.7));
    kiemTra(goc.chiTiet().size() == 2 && banSao.chiTiet().size() == 3, "copy DonHang là deep copy");
    DonHang gan;
    gan = goc;
    kiemTra(gan == goc && gan.chiTiet().size() == goc.chiTiet().size(), "copy assignment");
    DonHang diChuyen(std::move(banSao));
    kiemTra(diChuyen.maDon() == "DH001" && banSao.chiTiet().empty(), "move constructor");
    DonHang ganDiChuyen;
    ganDiChuyen = std::move(gan);
    kiemTra(ganDiChuyen == goc && gan.chiTiet().empty(), "move assignment");

    goc.chuyenTrangThai();
    goc.chuyenTrangThai();
    kiemTra(goc.trangThai() == tt::DANG_GIAO && goc.lichSu().size() == 3, "lịch sử trạng thái Mới-Xác nhận-Đang giao");
    phaiNem<ChuyenTrangThaiSaiException>([&] { goc.chuyenTrangThai(); }, "không hoàn tất nếu thiếu ảnh giao hàng");
    goc.xacNhanGiaoThanhCong(AnhDinhKem{"proof.png", "ảnh thử", thoiGianHienTai()});
    kiemTra(goc.trangThai() == tt::HOAN_TAT && goc.coAnhGiaoHang(), "xác nhận giao cần ảnh");
    goc.yeuCauTraHang("Không vừa");
    goc.hoanHang();
    phaiNem<ChuyenTrangThaiSaiException>([&] { goc.ketThucDon(); }, "không kết thúc khi chờ shipper nhận hàng trả");
    goc.shipperNhanDonTra(hang::GHN);
    goc.shipperXacNhanGuiVeShop();
    kiemTra(goc.trangThai() == tt::TRA_HANG, "luồng trả hàng qua shipper");
    phaiNem<ChuyenTrangThaiSaiException>([&] { goc.chuyenTrangThai(); }, "Trả hàng là trạng thái cuối");
    goc.ketThucDon();
    kiemTra(goc.daKetThuc(), "kết thúc được đơn sau khi hàng trả đã về shop");
}

void testNghiepVu() {
    SanPham sp("AT01", "Áo thun", "TT", 120000, 0.3, 5, 10, 4.8);
    GioHang gio;
    gio.themSanPham(sp, 2);
    gio.themSanPham(sp, 1);
    kiemTra(gio.tinhTongTien() == 360000 && gio.demNguoiBan() == 1, "giỏ cộng số lượng và tính tổng");
    phaiNem<HetHangException>([&] { gio.themSanPham(sp, 3); }, "giỏ không vượt tồn kho");
    kiemTra(timHangReNhat(10, 1) == hang::GHTK, "tìm hãng ship rẻ nhất");
    phaiNem<OrderException>([] { (void)timHangReNhat(0, 1); }, "cự ly giao hàng phải dương");

    KMPhanTram km10;
    TomTatGio ttGio;
    ttGio.tongTien = 800000;
    kiemTra(km10.apDung(ttGio, 30000) == 50000, "GIAM10 được giới hạn 50.000đ");
    KMGiamTien km30;
    ttGio.soNguoiBan = 2;
    kiemTra(km30.apDung(ttGio, 10000) == 30000, "GIAM30K cần hai nhà bán");
    KMFreeShip freeShip;
    kiemTra(freeShip.apDung(ttGio, 24000) == 24000, "FREESHIP miễn đúng phí giao");
    phaiNem<MaGiamGiaKhongHopLeException>([] { (void)taoKhuyenMai("MA_KHONG_CO"); }, "mã khuyến mãi sai bị từ chối");

    TaiKhoanVi vi;
    vi.napTien(100000, "test");
    vi.truTien(25000, "test");
    vi.hoanTien(5000, "hoàn test");
    kiemTra(vi.soDu() == 80000 && vi.lichSu().size() == 3, "ví ghi nhận nạp, trừ và hoàn tiền");
    phaiNem<ThanhToanThatBaiException>([&] { vi.napTien(1, "test"); }, "ví chặn khoản nạp dưới hạn mức");
    phaiNem<ThanhToanThatBaiException>([&] { vi.truTien(90000, "test"); }, "ví chặn thanh toán vượt số dư");

    DonHang don = taoDonMau("DHSTAT");
    vector<DonHang> ds{don};
    kiemTra(ThongKeService::tyLeGiaoThanhCong(ds) == 0 && ThongKeService::demTheoHang(ds).at(hang::GHN) == 1,
            "thống kê đơn và hãng vận chuyển");
    phaiNem<FileDataException>([] { (void)DataStore::docSanPham("tep_khong_ton_tai.json"); }, "thiếu file dữ liệu trả FileDataException");
}

void testJsonStorage() {
    const auto thuMuc = std::filesystem::temp_directory_path() / "dt08_storage_test";
    std::filesystem::create_directories(thuMuc);
    const string sanPhamPath = (thuMuc / "san_pham.json").string();
    const string donPath = (thuMuc / "don_hang.json").string();
    const string taiKhoanPath = (thuMuc / "tai_khoan.json").string();
    try {
        vector<SanPham> dsSP{SanPham("AT01", "Áo thun", "TT", 120000, 0.3, 5, 2, 4.8)};
        DataStore::luuSanPham(dsSP, sanPhamPath);
        auto docSP = DataStore::docSanPham(sanPhamPath);
        kiemTra(docSP.size() == 1 && docSP[0].ma() == "AT01" && docSP[0].daBan() == 2, "JSON sản phẩm đọc ghi vòng lại");

        vector<DonHang> dsDon{taoDonMau("DHJSON")};
        DataStore::luuDonHang(dsDon, donPath);
        auto docDon = DataStore::docDonHang(donPath);
        kiemTra(docDon.size() == 1 && docDon[0].maDon() == "DHJSON" && docDon[0].chiTiet().size() == 2,
                "JSON đơn hàng đọc ghi vòng lại");

        vector<unique_ptr<NguoiDung>> dsNguoi;
        auto kh = make_unique<KhachHang>("KH01", "khach", bamMatKhau("123"), "Khách thử");
        kh->vi()->napTien(50000, "seed");
        dsNguoi.push_back(std::move(kh));
        DataStore::luuTaiKhoan(dsNguoi, taiKhoanPath);
        auto docNguoi = DataStore::docTaiKhoan(taiKhoanPath);
        kiemTra(docNguoi.size() == 1 && docNguoi[0]->kiemTraMatKhau("123") && docNguoi[0]->vi()->soDu() == 50000,
                "JSON tài khoản lưu băm mật khẩu và số dư ví");

        std::ifstream f(taiKhoanPath);
        string noiDung((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        kiemTra(noiDung.find("\"123\"") == string::npos, "JSON không lưu mật khẩu thuần");
    } catch (...) {
        std::error_code ec;
        std::filesystem::remove_all(thuMuc, ec);
        throw;
    }
    std::error_code ec;
    std::filesystem::remove_all(thuMuc, ec);
}
}

int main() {
    try {
        testBig5VaState();
        testNghiepVu();
        testJsonStorage();
        std::cout << "Đã qua " << soTest << " kiểm tra nghiệp vụ và lưu trữ.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[FAIL] Ngoại lệ không mong đợi: " << e.what() << '\n';
        return 1;
    }
}
