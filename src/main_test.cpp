// Người phụ trách: C | nhánh feature/payment-storage | build: cmake target dt08_test
#include <iostream>
int main() {
    // TODO(C-6): test Big 5 của DonHang (copy sâu: sửa bản sao không ảnh hưởng bản gốc; move: nguồn rỗng)
    // TODO(C-6): test 5 exception: HetHang, MaGiamGiaKhongHopLe, ChuyenTrangThaiSai, ThanhToanThatBai, FileData
    // TODO(C-6): test luồng trạng thái Mới -> Trả hàng và lỗi khi chuyển tiếp từ Trả hàng
    // TODO(C-6): test 3 hãng ship, 3 khuyến mãi, thống kê; chạy với -fsanitize=address
    std::cout << "TODO: chưa có test\n";
    return 0;
}
