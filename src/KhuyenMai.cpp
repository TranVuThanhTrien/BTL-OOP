#include "KhuyenMai.h"
#include "OrderException.h"

// Người phụ trách: A | nhánh feature/core-domain | LỚP 3
namespace {
    map<string, bool>& trangThaiKM() {
        static map<string, bool> s_bat = {
            {km::GIAM10,   true},
            {km::GIAM30K,  true},
            {km::FREESHIP, true}
        };
        return s_bat;
    }
}

bool KMPhanTram::dieuKienHopLe(const TomTatGio& gio) const {
    return gio.tongTien >= hangso::GIAM10_GIO_TOI_THIEU;
}

double KMPhanTram::apDung(const TomTatGio& gio, double phiShip) const {
    if (!dieuKienHopLe(gio)) return 0;
    double giam = std::min(gio.tongTien * hangso::GIAM10_TI_LE, hangso::GIAM10_TOI_DA);
    giam = std::min(giam, gio.tongTien + phiShip);
    return lamTronDong(giam);
}

bool KMGiamTien::dieuKienHopLe(const TomTatGio& gio) const {
    return gio.soNguoiBan >= hangso::GIAM30K_SO_NGUOI_BAN_TOI_THIEU;
}

double KMGiamTien::apDung(const TomTatGio& gio, double phiShip) const {
    if (!dieuKienHopLe(gio)) return 0;
    double giam = std::min(hangso::GIAM30K_SO_TIEN, gio.tongTien + phiShip);
    return lamTronDong(giam);
}

bool KMFreeShip::dieuKienHopLe(const TomTatGio& gio) const {
    return gio.tongTien >= hangso::FREESHIP_GIO_TOI_THIEU;
}

double KMFreeShip::apDung(const TomTatGio& gio, double phiShip) const {
    if (!dieuKienHopLe(gio)) return 0;
    return lamTronDong(std::max(0.0, phiShip));
}

vector<string> danhSachMaKhuyenMai() {
    return {km::GIAM10, km::GIAM30K, km::FREESHIP};
}

bool khuyenMaiDangBat(const string& ma) {
    string m = inHoa(ma);
    auto& ds = trangThaiKM();
    auto it = ds.find(m);
    if (it == ds.end()) return false;
    return it->second;
}

void datKhuyenMaiBat(const string& ma, bool bat) {
    string m = inHoa(ma);
    auto& ds = trangThaiKM();
    auto it = ds.find(m);
    if (it == ds.end()) {
        throw MaGiamGiaKhongHopLeException("Mã khuyến mãi không tồn tại: " + ma);
    }
    it->second = bat;
}

unique_ptr<KhuyenMai> taoKhuyenMai(const string& ma) {
    string m = inHoa(ma);
    if (!khuyenMaiDangBat(m)) {
        throw MaGiamGiaKhongHopLeException("Mã không tồn tại hoặc đã tắt: " + ma);
    }
    if (m == km::GIAM10)   return make_unique<KMPhanTram>();
    if (m == km::GIAM30K)  return make_unique<KMGiamTien>();
    if (m == km::FREESHIP) return make_unique<KMFreeShip>();
    throw MaGiamGiaKhongHopLeException("Mã khuyến mãi không hợp lệ: " + ma);
}
