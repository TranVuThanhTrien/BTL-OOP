#include "ThongKeService.h"

// Người phụ trách: D | nhánh feature/web-api | LỚP 6
double ThongKeService::tyLeGiaoThanhCong(const vector<DonHang>& ds) {
    int tungDangGiao = 0;
    int tungHoanTat = 0;
    for (const auto& d : ds) {
        if (d.daTungQuaTrangThai(tt::DANG_GIAO)) ++tungDangGiao;
        if (d.daTungQuaTrangThai(tt::HOAN_TAT))  ++tungHoanTat;
    }
    if (tungDangGiao == 0) return 0;
    return static_cast<double>(tungHoanTat) * 100.0 / tungDangGiao;
}

double ThongKeService::tyLeHoanHang(const vector<DonHang>& ds) {
    int tungHoanTat = 0;
    int traHang = 0;
    for (const auto& d : ds) {
        if (d.daTungQuaTrangThai(tt::HOAN_TAT)) ++tungHoanTat;
        if (d.trangThai() == tt::TRA_HANG)      ++traHang;
    }
    if (tungHoanTat == 0) return 0;
    return static_cast<double>(traHang) * 100.0 / tungHoanTat;
}

map<string, double> ThongKeService::doanhThuTheoNguoiBan(const vector<DonHang>& ds) {
    map<string, double> kq;
    for (const auto& d : ds) {
        if (d.trangThai() != tt::HOAN_TAT) continue; // Đơn Trả hàng hoặc chưa Hoàn tất không tính
        map<string, double> theoShop = d.tomTat().theoNguoiBan;
        const auto& daHoan = d.daHoanTien();
        for (const auto& kv : theoShop) {
            double hoan = 0;
            auto it = daHoan.find(kv.first);
            if (it != daHoan.end()) hoan = it->second;
            double conLai = std::max(0.0, kv.second - hoan);
            kq[kv.first] = lamTronDong(kq[kv.first] + conLai);
        }
    }
    return kq;
}

map<string, int> ThongKeService::demTheoHang(const vector<DonHang>& ds) {
    map<string, int> kq = {
        {hang::GHN, 0},
        {hang::GHTK, 0},
        {hang::VIETTEL_POST, 0}
    };
    for (const auto& d : ds) {
        if (!d.hangVanChuyen().empty()) {
            kq[d.hangVanChuyen()]++;
        }
    }
    return kq;
}
