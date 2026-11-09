#pragma once
// ============================================================================
// Common.h – HEADER DÙNG CHUNG DUY NHẤT của cả nhóm (đã chốt, v1)
// Quy tắc:
//  1. Header riêng của mỗi lớp chỉ #include "Common.h" (hoặc header lớp bên dưới), KHÔNG tự #include thư viện chuẩn.
//     Cần thêm thư viện -> thêm vào ĐÂY, báo nhóm, không thêm trong header riêng.
//  2. Không "using namespace std". Chỉ các using khai báo ở cuối file này.
//  3. Không include json.hpp / httplib.h ở đây (nặng, và chỉ DataStore.cpp + main.cpp được dùng).
//  4. Mọi hằng số nghiệp vụ nằm trong namespace hangso / hang / vaitro / pttt (dưới), cấm viết số "cứng" trong .cpp.
// ============================================================================
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cstddef>
using std::string; using std::vector; using std::map; using std::pair;
using std::unique_ptr; using std::make_unique; using std::ostream; using std::move;

// ---- Tên chuẩn (chuỗi định danh dùng ở cả C++, JSON và web). Đổi ở đây là đổi cho cả hệ thống. ----
namespace hang {        // hãng vận chuyển: khớp HANG{} trong web/index.html và tên lớp GHN/GHTK/ViettelPost
    inline const string GHN = "GHN", GHTK = "GHTK", VIETTEL_POST = "ViettelPost";
}
namespace vaitro {      // NguoiDung::vaiTro()
    inline const string KHACH_HANG = "KhachHang", NGUOI_BAN = "NguoiBan", SHIPPER = "Shipper", QUAN_LY = "QuanLy";   // "Shipper" khớp web (skeleton cũ thiếu vai trò này)
}
namespace pttt {        // PhuongThucThanhToan::ten() / taoPhuongThuc(); web gửi "The","Vi" phải đổi sang tên này
    inline const string COD = "COD", THE_TIN_DUNG = "TheTinDung", VI_DIEN_TU = "ViDienTu";
}
namespace tt {          // TrangThaiDonHang::ten() / taoTrangThai(); đúng chuỗi web đang dùng (mảng TTS)
    inline const string MOI = "Mới", XAC_NHAN = "Xác nhận", DANG_GIAO = "Đang giao", HOAN_TAT = "Hoàn tất", TRA_HANG = "Trả hàng";
}
namespace km {          // KhuyenMai::ma()
    inline const string GIAM10 = "GIAM10", GIAM30K = "GIAM30K", FREESHIP = "FREESHIP";
}

// ---- Hằng số nghiệp vụ (lấy từ TODO.md + web/index.html; tiền tính bằng đồng, kiểu double, làm tròn bằng lamTronDong) ----
namespace hangso {
    constexpr double GIAM10_TI_LE = 0.10, GIAM10_TOI_DA = 50000, GIAM10_GIO_TOI_THIEU = 200000;
    constexpr double GIAM30K_SO_TIEN = 30000;  constexpr int GIAM30K_SO_NGUOI_BAN_TOI_THIEU = 2;
    constexpr double FREESHIP_GIO_TOI_THIEU = 300000;
    constexpr double PHI_THE_TIN_DUNG = 0.015;                       // tính trên (tiền hàng - giảm + ship), khớp web
    constexpr double NAP_TOI_THIEU = 10000, NAP_TOI_DA = 20000000;
    constexpr std::size_t TIN_NHAN_TOI_DA = 500;                    // ký tự
    constexpr std::size_t DIA_CHI_TOI_THIEU = 8;                    // ký tự
    constexpr std::size_t SDT_MIN = 9, SDT_MAX = 11;                // số chữ số
    constexpr std::size_t MA_SHOP_MIN = 2, MA_SHOP_MAX = 4;         // chữ cái
}

// ---- Hàm tiện ích dùng chung (cài trong src/Common.cpp) ----
string thoiGianHienTai();                    // "YYYY-MM-DD HH:MM:SS" giờ máy; MỌI trường thoiDiem đều dùng định dạng này
string inHoa(string s);                      // chỉ ASCII; tên đăng nhập so sánh bằng inHoa()
string inThuong(string s);
double lamTronDong(double soTien);           // làm tròn về số đồng nguyên, MỌI phép tính tiền kết thúc bằng hàm này
string dinhDangTien(double soTien);          // 1234567 -> "1.234.567đ"
bool   laMaShopHopLe(const string& ma);      // 2-4 chữ cái a-z/A-Z
bool   laSoDienThoaiHopLe(const string& s);  // 9-11 chữ số
string bamMatKhau(const string& matKhauThuan);                    // -> "muối$băm"; mỗi lần gọi ra muối khác nhau. CHỈ DÙNG cho bài tập, không dùng cho hệ thống thật
bool   kiemTraBam(const string& matKhauThuan, const string& bam);   // so khớp mật khẩu thuần với chuỗi đã băm
