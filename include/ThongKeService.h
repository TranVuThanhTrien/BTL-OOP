#pragma once
#include "DonHang.h"
// Người phụ trách: D | nhánh feature/web-api | LỚP 6 | Hàm tĩnh thuần túy, không giữ trạng thái -> dễ test. Truyền vào DonHangService::tatCaDon() (QuanLy) hoặc danh sách đã lọc theo shop (NguoiBan).
class ThongKeService {
public:
    static double tyLeGiaoThanhCong(const vector<DonHang>& ds);
    static double tyLeHoanHang(const vector<DonHang>& ds);
    static map<string,double> doanhThuTheoNguoiBan(const vector<DonHang>& ds);
    static map<string,int> demTheoHang(const vector<DonHang>& ds);
};
