// Người phụ trách: D | nhánh feature/web-api
#include <iostream>
// #include "httplib.h"      // TODO(D-5): tải third_party/httplib.h (cpp-httplib)
// #include "json.hpp"       // TODO(D-5): tải third_party/json.hpp (nlohmann/json)
// QUYẾT ĐỊNH ĐA LUỒNG: cpp-httplib chạy mỗi request một thread. Dùng MỘT std::mutex toàn cục, mọi handler /api/* bắt đầu bằng std::lock_guard.
// Các biến tĩnh (bật/tắt khuyến mãi, bảng giá ship) và AuthService/DonHangService đều được bảo vệ bởi khóa này. Không tự khóa lẻ trong từng lớp.
// QUYẾT ĐỊNH JSON: toàn bộ chuyển đổi DonHang/SanPham/NguoiDung <-> JSON viết ở main.cpp (API) và DataStore.cpp (file); lớp domain/service không biết json.
// Trước khi đọc tên pttt từ web: web gửi 'The','Vi' -> đổi sang pttt::THE_TIN_DUNG, pttt::VI_DIEN_TU.
int main() {
    // TODO(D-5): httplib::Server svr; svr.set_mount_point("/", "./web");
    // TODO(D-6): các route (xem TODO.md): GET /api/san-pham, POST /api/gio-hang/them, GET /api/gio-hang,
    //            POST /api/gio-hang/khuyen-mai, POST /api/don-hang/xem-truoc, POST /api/don-hang, GET /api/don-hang,
    //            GET /api/don-hang/{id}, POST /api/don-hang/{id}/chuyen-trang-thai, GET /api/thong-ke
    // TODO(D-11): POST /api/dang-nhap, /api/dang-xuat, /api/dang-ky; GET /api/toi (header Authorization: Bearer token)
    // TODO(D-11): nhóm /api/quan-ly/*: san-pham (GET, POST, PUT, DELETE), khuyen-mai/{ma}/bat-tat, van-chuyen/{hang}, tai-khoan, tai-khoan/{user}/khoa
    // TODO(D-10): mỗi route gọi AuthService::xacThuc + yeuCauQuyen trước khi xử lý (khách chỉ thấy đơn của mình, người bán chỉ thấy shop mình); thiếu quyền -> HTTP 403
    // TODO(D-21): GET /api/san-pham?tuKhoa=&maShop=&sapXep=phoBien|moiNhat|banChay|giaTang|giaGiam, mỗi sản phẩm có thêm daBan và danhGia; daBan tăng khi đặt hàng thành công
    // TODO(D-19): POST /api/don-hang/{id}/nhan-don-tra và /xac-nhan-gui-ve-shop (chỉ shipper đúng hãng; sai bước -> 400, sai quyền -> 403)
    // TODO(D-20): chat theo đơn: GET /api/tin-nhan (danh sách hội thoại, kèm số tin chưa đọc), GET và POST /api/don-hang/{id}/tin-nhan { noiDung }; người ngoài đơn nhận 403
    // TODO(D-18): POST /api/don-hang/{id}/xac-nhan-giao { anh (base64) }: bước Đang giao -> Hoàn tất BẮT BUỘC kèm ảnh; route chuyen-trang-thai chung phải từ chối bước này nếu đơn chưa có ảnh giao hàng
    // TODO(D-17): POST /api/don-hang/{id}/tra-hang { lyDo } · /hoan-hang · /tu-choi-tra-hang · /hoan-tien · /anh-giao-hang · /anh-khieu-nai { anh (base64), moTa }; lỗi quyền -> 403
    // TODO(D-16): GET /api/giao-hang (chỉ Shipper): MỌI đơn của hãng mình từ trạng thái Mới (đơn Mới chỉ xem), kèm địa chỉ, tiền thu hộ COD và phí ship; gọi DonHangService::layDonGiaoHang
    // TODO(D-14): xem sản phẩm, giỏ hàng, xem-truoc KHÔNG cần đăng nhập; POST /api/don-hang, /api/vi/*, /api/quan-ly/* cần token, thiếu token trả HTTP 401 (DangNhapThatBaiException); giỏ hàng của khách vãng lai được giữ khi đăng nhập
    // TODO(D-13): POST /api/don-hang/{id}/ket-thuc (khách chủ đơn, người bán hoặc quản lý đều gọi được); JSON đơn có thêm "daKetThuc" và "thoiDiemKetThuc"
    // TODO(D-12): ví: GET /api/vi, POST /api/vi/nap-tien { soTien, nguon }, GET /api/vi/giao-dich
    // TODO(D-7): bắt OrderException -> res.status = e.maHttp() (400 mặc định, 401 DangNhapThatBai, 403 KhongCoQuyen, 500 FileData), JSON { "loi": e.loai(), "thongBao": e.what() }
    // TODO(D-5): svr.listen("0.0.0.0", 8080);
    std::cout << "ĐT08: bộ khung đã build được, hãy cài đặt các TODO\n";
    return 0;
}
