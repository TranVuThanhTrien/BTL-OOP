// Test độc lập cho phần của C: Common, TaiKhoanVi, PhuongThucThanhToan. Không cần code của A, B, D.
// Build: cmake --build build --target dt08_c_test   |   Chạy: build\dt08_c_test.exe
#include "Common.h"
#include "OrderException.h"
#include "TaiKhoanVi.h"
#include "PhuongThucThanhToan.h"

static int dem = 0, loi = 0;
#define KT(dk) do { ++dem; if (dk) std::cout << "  [OK]   " << #dk << "\n"; else { ++loi; std::cout << "  [LOI]  " << #dk << "  (dòng " << __LINE__ << ")\n"; } } while (0)
template <class E, class F> static bool nem(F f) { try { f(); } catch (const E&) { return true; } catch (...) { return false; } return false; }

int main() {
    std::cout << "== Tiện ích chung ==\n";
    KT(dinhDangTien(1234567) == "1.234.567đ");
    KT(lamTronDong(10.5) == 11);
    KT(inHoa("tt") == "TT");
    KT(laMaShopHopLe("DT") && !laMaShopHopLe("D") && !laMaShopHopLe("D1"));
    KT(laSoDienThoaiHopLe("0912345678") && !laSoDienThoaiHopLe("12ab"));
    auto bam = bamMatKhau("123");
    KT(kiemTraBam("123", bam) && !kiemTraBam("sai", bam));
    KT(bamMatKhau("123") != bamMatKhau("123"));   // mỗi lần muối khác nhau

    std::cout << "== Ví (TaiKhoanVi) ==\n";
    TaiKhoanVi vi;
    vi.napTien(100000, "test");
    KT(vi.soDu() == 100000);
    KT(nem<ThanhToanThatBaiException>([&]{ vi.napTien(5000, "x"); }));        // dưới mức nạp tối thiểu
    KT(nem<ThanhToanThatBaiException>([&]{ vi.napTien(30000000, "x"); }));    // trên mức nạp tối đa
    vi.truTien(40000, "mua hàng");
    KT(vi.soDu() == 60000);
    KT(nem<ThanhToanThatBaiException>([&]{ vi.truTien(999999, "x"); }));      // không đủ số dư
    vi.nhanTien(15000, "phí ship");
    vi.hoanTien(5000, "hoàn tiền");
    KT(vi.soDu() == 80000);
    KT(vi.lichSu().size() == 4);

    std::cout << "== Phương thức thanh toán ==\n";
    KT(taoPhuongThuc(pttt::COD)->thanhToan(500000));
    KT(taoPhuongThuc(pttt::COD)->tinhPhiXuLy(500000) == 0);
    KT(taoPhuongThuc(pttt::THE_TIN_DUNG)->tinhPhiXuLy(100000) == 1500);       // 1,5%
    KT(nem<ThanhToanThatBaiException>([&]{ taoPhuongThuc(pttt::THE_TIN_DUNG)->thanhToan(60000000); }));
    TaiKhoanVi vi2; vi2.napTien(200000, "test");
    auto pv = taoPhuongThuc("Vi", &vi2);                                      // bí danh "Vi" từ web
    KT(pv->ten() == pttt::VI_DIEN_TU);
    pv->thanhToan(150000);
    KT(vi2.soDu() == 50000);
    KT(nem<ThanhToanThatBaiException>([&]{ pv->thanhToan(100000); }));        // thiếu tiền
    KT(nem<ThanhToanThatBaiException>([&]{ taoPhuongThuc(pttt::VI_DIEN_TU, nullptr)->thanhToan(1000); }));   // ví null
    KT(nem<ThanhToanThatBaiException>([&]{ taoPhuongThuc("Bitcoin"); }));     // tên lạ
    auto sao = pv->clone();
    KT(sao->ten() == pv->ten());

    std::cout << "== Exception ==\n";
    try { throw KhongCoQuyenException("x"); } catch (const OrderException& e) { KT(e.maHttp() == 403 && e.loai() == "KhongCoQuyenException"); }
    try { throw DangNhapThatBaiException("x"); } catch (const OrderException& e) { KT(e.maHttp() == 401); }
    try { throw FileDataException("x"); } catch (const OrderException& e) { KT(e.maHttp() == 500); }
    try { throw HetHangException("x"); } catch (const OrderException& e) { KT(e.maHttp() == 400); }

    std::cout << "\nTổng: " << dem << " kiểm tra, " << loi << " lỗi\n";
    return loi == 0 ? 0 : 1;
}
