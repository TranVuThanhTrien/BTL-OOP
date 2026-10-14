#pragma once
#include "DonHang.h"
#include "DataStore.h"
// Người phụ trách: D | nhánh feature/web-api | gom các bước nghiệp vụ, KHÔNG chứa logic tính toán (logic nằm ở lớp OOP)
class DonHangService {
    vector<DonHang> m_dsDon;
public:
    // TODO(D-1): DonHang taoDon(const GioHang& gio, const string& hang, const string& pttt, const string& maKM, double km);
    //            // giỏ -> áp khuyến mãi -> tính ship -> thanh toán -> trừ tồn kho -> lưu file
    // TODO(D-1): double xemTruoc(...) const;                 // tính thử, không lưu
    // TODO(D-2): void chuyenTrangThai(const string& maDon);
    // TODO(D-2): const vector<DonHang>& layDanhSach() const;
    // TODO(D-19): void nhanDonTra(const string& maDon, const NguoiDung& shipper);  void xacNhanGuiVeShop(const string& maDon, const NguoiDung& shipper);   // chỉ shipper đúng hãng
    // TODO(D-20): void guiTinNhan(const string& maDon, const NguoiDung& nguoiGoi, const string& noiDung);  vector<TinNhan> layTinNhan(...);  int tongTinChuaDoc(const NguoiDung& nguoi) const;
    // TODO(D-18): void xacNhanGiao(const string& maDon, const NguoiDung& shipper, const string& anhBase64);   // chỉ shipper đúng hãng, đơn đang ở DangGiao; lưu ảnh rồi mới chuyển HoanTat
    // TODO(D-17): void traHang(const string& maDon, const NguoiDung& nguoiGoi, const string& lyDo);   // chỉ khách chủ đơn
    // TODO(D-17): void hoanHang(...);  void tuChoiTraHang(...);  void hoanTien(...);               // chỉ NguoiBan có hàng trong đơn hoặc QuanLy
    // TODO(D-17): void guiAnh(const string& maDon, const NguoiDung& nguoiGoi, const string& loai, const string& anhBase64, const string& moTa);   // loai: "giao" (shipper) hoặc "khieuNai" (khách)
    // TODO(D-15): void chuyenTrangThai(const string& maDon, const NguoiDung& nguoiGoi);   // kiểm tra theo bảng quyền bên dưới, sai -> KhongCoQuyenException
    //   Bảng quyền: NguoiBan: Mới->Xác nhận và Hoàn tất->Trả hàng | Shipper (đúng hãng): Xác nhận->Đang giao và Đang giao->Hoàn tất | QuanLy: mọi bước | KhachHang: không được
    // TODO(D-2): const DonHang& layChiTiet(const string& maDon) const;
};
