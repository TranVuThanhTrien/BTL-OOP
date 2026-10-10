#include <iostream>
#include "ThanhToan.h"

// Chương trình chạy thử cho phần Thanh toán: COD, Thẻ tín dụng, Ví điện tử, lưu/đọc JSON, exception.
int main() {
    // 1. Ví điện tử: trả thành công rồi trả quá số dư
    ViDienTu vi(500000);
    try {
        vi.thanhToan(201400);
        std::cout << "[Vi] Thanh toan 201.400d OK, so du con: " << vi.getSoDu() << "\n";
        vi.thanhToan(500000);
    } catch (const ThanhToanException& e) {
        std::cout << "[Vi] Loi: " << e.what() << "\n";
    }

    // 2. Thẻ tín dụng: phí 1.5%, hạn mức 1.000.000
    try {
        TheTinDung the("4111 1111 1111 1111", 1000000);
        std::cout << "[The] Phi xu ly cho 200.000d: " << the.tinhPhiXuLy(200000) << "\n";
        the.thanhToan(200000);
        std::cout << "[The] " << the.getSoTheDaAn() << ", han muc con: " << the.getHanMucConLai() << "\n";
        the.thanhToan(900000);
    } catch (const ThanhToanException& e) {
        std::cout << "[The] Loi: " << e.what() << "\n";
    }

    // 3. Thẻ sai số
    try {
        TheTinDung sai("4111 1111 1111 1112", 1000000);
    } catch (const ThanhToanException& e) {
        std::cout << "[The] Loi: " << e.what() << "\n";
    }

    // 4. COD: ghi nhận, rồi xác nhận đã thu khi giao
    COD cod;
    cod.thanhToan(210000);
    std::cout << "[COD] Can thu: " << cod.getSoTienCanThu() << ", da thu: " << cod.daThuTien() << "\n";
    cod.xacNhanDaThu();
    std::cout << "[COD] Sau khi giao, da thu: " << cod.daThuTien() << "\n";

    // 5. Lưu ra file JSON rồi đọc lại
    try {
        ThanhToanJson::DanhSach ds;
        ds.push_back(cod.clone());
        ds.push_back(std::make_unique<TheTinDung>("4111 1111 1111 1111", 5000000));
        ds.push_back(vi.clone());
        ThanhToanJson::luu(ds, "data/payments.json");
        std::cout << "\nDa luu data/payments.json:\n" << ThanhToanJson::toJson(ds);

        auto doc = ThanhToanJson::doc("data/payments.json");
        std::cout << "Doc lai duoc " << doc.size() << " phuong thuc: ";
        for (const auto& p : doc) std::cout << p->getTen() << "; ";
        std::cout << "\n";
    } catch (const FileException& e) {
        std::cout << "Loi file: " << e.what() << "\n";
    }
    return 0;
}
