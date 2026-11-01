#include "KhuyenMai.h"
#include "OrderException.h"
// Người phụ trách: A | A-6, A-7
// Hằng số nghiệp vụ lấy từ hangso::* (Common.h). Trạng thái bật/tắt là biến tĩnh, không lưu file (docs/KIEN_TRUC.md mục 3, dòng 6).

namespace {
// Không giảm quá (tiền hàng + ship) và luôn là số đồng nguyên, không âm.
double gioiHan(double giam, const TomTatGio& gio, double phiShip) {
    double tran = gio.tongTien + phiShip;
    if (giam > tran) giam = tran;
    if (giam < 0) giam = 0;
    return lamTronDong(giam);
}
// Khởi tạo lần đầu khi gọi (tránh phụ thuộc thứ tự khởi tạo biến toàn cục giữa các file .cpp).
map<string,bool>& bangBat() {
    static map<string,bool> b = { { km::GIAM10, true }, { km::GIAM30K, true }, { km::FREESHIP, true } };
    return b;
}
} // namespace

// ---- GIAM10: 10%, tối đa 50.000đ, giỏ từ 200.000đ ----
bool KMPhanTram::dieuKienHopLe(const TomTatGio& gio) const { return gio.tongTien >= hangso::GIAM10_GIO_TOI_THIEU; }
double KMPhanTram::apDung(const TomTatGio& gio, double phiShip) const {
    if (!dieuKienHopLe(gio)) return 0;
    double giam = std::min(gio.tongTien * hangso::GIAM10_TI_LE, hangso::GIAM10_TOI_DA);
    return gioiHan(giam, gio, phiShip);
}

// ---- GIAM30K: giảm 30.000đ khi mua từ 2 nhà bán ----
bool KMGiamTien::dieuKienHopLe(const TomTatGio& gio) const { return gio.soNguoiBan >= hangso::GIAM30K_SO_NGUOI_BAN_TOI_THIEU; }
double KMGiamTien::apDung(const TomTatGio& gio, double phiShip) const {
    if (!dieuKienHopLe(gio)) return 0;
    return gioiHan(hangso::GIAM30K_SO_TIEN, gio, phiShip);
}

// ---- FREESHIP: miễn ship khi giỏ từ 300.000đ ----
bool KMFreeShip::dieuKienHopLe(const TomTatGio& gio) const { return gio.tongTien >= hangso::FREESHIP_GIO_TOI_THIEU; }
double KMFreeShip::apDung(const TomTatGio& gio, double phiShip) const {
    if (!dieuKienHopLe(gio)) return 0;
    return gioiHan(phiShip, gio, phiShip);
}

// ---- Factory + bật/tắt ----
unique_ptr<KhuyenMai> taoKhuyenMai(const string& ma) {
    const string m = inHoa(ma);
    if (!khuyenMaiDangBat(m))
        throw MaGiamGiaKhongHopLeException("Mã giảm giá không hợp lệ hoặc đã bị tắt: " + ma);
    if (m == km::GIAM10)    return make_unique<KMPhanTram>();
    if (m == km::GIAM30K)   return make_unique<KMGiamTien>();
    if (m == km::FREESHIP)  return make_unique<KMFreeShip>();
    throw MaGiamGiaKhongHopLeException("Mã giảm giá không hợp lệ: " + ma);
}

vector<string> danhSachMaKhuyenMai() { return { km::GIAM10, km::GIAM30K, km::FREESHIP }; }

bool khuyenMaiDangBat(const string& ma) {
    auto it = bangBat().find(inHoa(ma));
    return it != bangBat().end() && it->second;
}

void datKhuyenMaiBat(const string& ma, bool bat) {
    auto it = bangBat().find(inHoa(ma));
    if (it == bangBat().end()) throw MaGiamGiaKhongHopLeException("Mã giảm giá không tồn tại: " + ma);
    it->second = bat;
}
