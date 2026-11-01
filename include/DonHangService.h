#pragma once
#include "AuthService.h"
// Người phụ trách: D | nhánh feature/web-api | LỚP 6 | Điều phối các bước nghiệp vụ; KHÔNG chứa công thức tính toán (nằm ở lớp OOP)
// Mỗi hàm public dưới đây = một route. Mọi hàm nhận `nguoiGoi` đã qua AuthService::xacThuc; hàm tự kiểm tra bảng quyền và ném KhongCoQuyenException.
// Service KHÔNG biết JSON/HTTP; main.cpp chuyển đổi.
class DonHangService {
    KhoSanPham& m_kho;  AuthService& m_auth;  string m_duongDanDon;
    vector<DonHang> m_dsDon;
    map<string,GioHang> m_gio;                       // mã giỏ -> giỏ. Khách vãng lai KHÔNG có token nên giỏ gắn với mã giỏ client giữ (header X-Gio-Hang); đăng nhập xong vẫn dùng mã đó nên giỏ không mất (D-14)
    int m_demDon = 0;                                // sinh mã DH001, DH002...
    void luu() const;                                // DataStore::luuDonHang + luuSanPham + AuthService::luu
    DonHang& tim(const string& maDon);
public:
    DonHangService(KhoSanPham& kho, AuthService& auth, string duongDanDon);   // docDonHang; m_demDon = số đơn đã có
    // ---- Giỏ hàng (không cần token) ----
    string taoMaGio();
    GioHang& gio(const string& maGio);               // chưa có thì tạo
    // ---- Tạo đơn ----
    // Đường tính tiền DUY NHẤT: dựng DonHang tạm từ giỏ -> DonHang::tinhKetQua(). Không trừ kho, không lưu.
    KetQuaTinhTien xemTruoc(const GioHang& gio, const string& hang, const string& pttt, const string& maKM, double km) const;   // TODO(D-1)
    // Giỏ -> kiểm tra tồn -> khuyến mãi -> chotDon (ship) -> payment->thanhToan -> trừ kho + congDaBan -> lưu file. Lỗi giữa chừng phải KHÔNG để lại kho/ví nửa vời (thanh toán sau cùng, trước khi trừ kho).
    const DonHang& taoDon(NguoiDung& khach, GioHang& gio, const DiaChi& diaChi, const string& hang, const string& pttt, const string& maKM, double km);   // TODO(D-1) rồi gio.lamTrong()
    // ---- Xem đơn: lọc theo vai trò (D-10) ----
    vector<const DonHang*> layDanhSach(const NguoiDung& nguoiGoi) const;     // Khách: của mình | NguoiBan: đơn có hàng shop mình | Shipper: đơn hãng mình | QuanLy: tất cả
    const DonHang& layChiTiet(const string& maDon, const NguoiDung& nguoiGoi) const;   // ngoài phạm vi -> KhongCoQuyenException
    vector<const DonHang*> layDonGiaoHang(const NguoiDung& shipper) const;   // D-16: MỌI đơn hãng mình kể từ trạng thái Mới (đơn Mới chỉ xem); thống nhất theo TODO.md, bỏ ghi chú cũ "từ Xác nhận"
    // ---- Chuyển trạng thái (D-15) ----
    // Bảng quyền (CHỐT): NguoiBan: chỉ Mới->Xác nhận | Shipper đúng hãng: Xác nhận->Đang giao; Đang giao->Hoàn tất CHỈ qua xacNhanGiao | QuanLy: mọi bước trước Hoàn tất, không bỏ qua ảnh | KhachHang: không được.
    // Hoàn tất->Trả hàng KHÔNG nằm trong bảng này: chỉ xảy ra khi shipper xacNhanGuiVeShop. (Bản cũ trong header ghi "NguoiBan: Hoàn tất->Trả hàng", mâu thuẫn với D-15; đã bỏ.)
    void chuyenTrangThai(const string& maDon, const NguoiDung& nguoiGoi);
    void xacNhanGiao(const string& maDon, NguoiDung& shipper, const string& anhBase64);   // TODO(D-18) lưu ảnh -> DonHang::xacNhanGiaoThanhCong -> traPhiShip(*shipper.vi()) đúng 1 lần
    void ketThuc(const string& maDon, const NguoiDung& nguoiGoi);            // D-13: khách chủ đơn, người bán có hàng, quản lý
    // ---- Trả hàng, ảnh ----
    void traHang(const string& maDon, const NguoiDung& khach, const string& lyDo);   // chỉ khách chủ đơn
    void hoanHang(const string& maDon, const NguoiDung& nguoiGoi);           // NguoiBan có hàng hoặc QuanLy
    void tuChoiTraHang(const string& maDon, const NguoiDung& nguoiGoi);
    void hoanTien(const string& maDon, const NguoiDung& nguoiGoi, const string& maShop);   // gọi DonHang::hoanTien(maShop, *khach.vi())
    void nhanDonTra(const string& maDon, const NguoiDung& shipper);          // D-19 shipper đúng hãng
    void xacNhanGuiVeShop(const string& maDon, const NguoiDung& shipper);
    void guiAnh(const string& maDon, const NguoiDung& nguoiGoi, const string& loai, const string& anhBase64, const string& moTa);   // loai: "giao" (shipper) | "khieuNai" (khách)
    // ---- Chat (D-20) ----
    void guiTinNhan(const string& maDon, const NguoiDung& nguoiGoi, const string& noiDung);
    vector<TinNhan> layTinNhan(const string& maDon, const NguoiDung& nguoiGoi);   // đồng thời danhDauDaDoc
    int tongTinChuaDoc(const NguoiDung& nguoi) const;
    const vector<DonHang>& tatCaDon() const { return m_dsDon; }              // cho ThongKeService
};
