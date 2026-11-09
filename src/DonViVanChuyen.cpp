#include "DonViVanChuyen.h"
#include "OrderException.h"

// Người phụ trách: B | nhánh feature/design-patterns | LỚP 1
BangGiaShip& bangGiaShip(const string& h) {
    static map<string, BangGiaShip> s_bangGia = {
        {hang::GHN,          BangGiaShip{15000, 2500, 3000, 1, 50}},
        {hang::GHTK,         BangGiaShip{12000, 2000, 2500, 2, 50}},
        {hang::VIETTEL_POST, BangGiaShip{14000, 2200, 2000, 2, 50}}
    };
    auto it = s_bangGia.find(h);
    if (it == s_bangGia.end()) {
        throw OrderException("Hãng vận chuyển không hợp lệ: " + h);
    }
    return it->second;
}

// Công thức: Phí ship = Phí mở cước cơ bản + (Số km × Đơn giá mỗi km) + (Số kg × Đơn giá mỗi kg)
double DonViTheoBangGia::tinhPhiShip(double km, double kg) const {
    if (km <= 0 || kg <= 0) {
        throw OrderException("Khoảng cách (km) và khối lượng (kg) phải lớn hơn 0");
    }
    const BangGiaShip bg = bangGiaShip(ten());
    return lamTronDong(bg.phiCoBan + km * bg.donGiaKm + kg * bg.donGiaKg);
}

int DonViTheoBangGia::uocTinhNgay(double km) const {
    if (km <= 0) {
        throw OrderException("Khoảng cách (km) phải lớn hơn 0");
    }
    const BangGiaShip bg = bangGiaShip(ten());
    int ngayThem = bg.kmMoiNgay > 0 ? static_cast<int>(km / bg.kmMoiNgay) : 0;
    return bg.ngayCoBan + ngayThem;
}

unique_ptr<DonViVanChuyen> taoDonVi(const string& ten) {
    if (ten == hang::GHN)          return make_unique<GHN>();
    if (ten == hang::GHTK)         return make_unique<GHTK>();
    if (ten == hang::VIETTEL_POST) return make_unique<ViettelPost>();
    throw OrderException("Đơn vị vận chuyển không tồn tại: " + ten);
}

vector<string> danhSachHang() {
    return {hang::GHN, hang::GHTK, hang::VIETTEL_POST};
}

string timHangReNhat(double km, double kg) {
    vector<string> ds = danhSachHang();
    string reNhat = ds.front();
    double phiMin = taoDonVi(reNhat)->tinhPhiShip(km, kg);
    for (std::size_t i = 1; i < ds.size(); ++i) {
        double phi = taoDonVi(ds[i])->tinhPhiShip(km, kg);
        if (phi < phiMin) {
            phiMin = phi;
            reNhat = ds[i];
        }
    }
    return reNhat;
}
