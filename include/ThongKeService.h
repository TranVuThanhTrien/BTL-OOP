#pragma once
#include "DonHang.h"
// Người phụ trách: D | nhánh feature/web-api | LỚP 6 | Hàm tĩnh thuần túy, không giữ trạng thái -> dễ test. Truyền vào DonHangService::tatCaDon() (QuanLy) hoặc danh sách đã lọc theo shop (NguoiBan).
class ThongKeService {
public:
    static double tyLeGiaoThanhCong(const vector<DonHang>& ds);    // TODO(D-3) số đơn từng Hoàn tất / số đơn từng Đang giao (daTungQuaTrangThai); mẫu = 0 thì trả 0
    static double tyLeHoanHang(const vector<DonHang>& ds);         // TODO(D-3) số đơn Trả hàng / số đơn từng Hoàn tất; mẫu = 0 thì trả 0
    static map<string,double> doanhThuTheoNguoiBan(const vector<DonHang>& ds);   // TODO(D-4) tiền hàng từng shop của đơn Hoàn tất, trừ phần đã hoàn (daHoanTien); đơn Trả hàng không tính
    static map<string,int> demTheoHang(const vector<DonHang>& ds); // TODO(D-4) số đơn theo hãng vận chuyển
};
