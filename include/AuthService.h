#pragma once
#include "DataStore.h"
// Người phụ trách: D | nhánh feature/web-api | LỚP 6 | Đăng nhập và phân quyền. Sở hữu duy nhất danh sách NguoiDung (và ví của họ).
class AuthService {
    vector<unique_ptr<NguoiDung>> m_dsNguoiDung;     // địa chỉ các NguoiDung phải ổn định (ViDienTu giữ con trỏ tới ví) -> luôn nằm trên heap
    map<string,string> m_phien;                      // token -> tên đăng nhập (viết thường)
    string m_duongDanTaiKhoan;
public:
    explicit AuthService(string duongDanTaiKhoan);   // đọc DataStore::docTaiKhoan; chưa có file thì tạo mặc định theo TODO.md (quanly/admin, TT, DT, NS, GHN, GHTK, VTP, khach)
    string dangNhap(const string& user, const string& pass);   // TODO(D-9) trả token; sai/bị khóa -> DangNhapThatBaiException. Tên đăng nhập KHÔNG phân biệt hoa thường
    void dangXuat(const string& token);
    NguoiDung& xacThuc(const string& token);         // TODO(D-9) token sai/thiếu -> DangNhapThatBaiException (HTTP 401). Trả non-const vì service cần sửa ví
    void dangKyKhachHang(const string& hoTen, const string& user, const string& pass);
    void dangKyNguoiBan(const string& tenShop, const string& maShop, const string& pass);   // TODO(D-9,D-12) laMaShopHopLe, tự inHoa, duy nhất; maShop cũng là tên đăng nhập
    void yeuCauQuyen(const NguoiDung& nd, const string& chucNang) const;   // TODO(D-10) !nd.coQuyen(chucNang) -> KhongCoQuyenException (403)
    void khoaTaiKhoan(const NguoiDung& nguoiGoi, const string& user, bool khoa);   // TODO(D-10) chỉ QuanLy; ngắt mọi phiên của user bị khóa
    // Tra cứu cho DonHangService (không cần token):
    NguoiDung* tim(const string& tenDangNhap);       // nullptr nếu không có
    NguoiDung* timShipperCuaHang(const string& maHang);
    const vector<unique_ptr<NguoiDung>>& danhSach() const { return m_dsNguoiDung; }
    void luu() const;                                // DataStore::luuTaiKhoan; gọi sau mọi thay đổi (đăng ký, khóa, ví đổi)
};
