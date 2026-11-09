#include "TrangThaiDonHang.h"
#include "DonHang.h"
#include "OrderException.h"

// Người phụ trách: B | nhánh feature/design-patterns | LỚP 1
void DonMoi::chuyenTiep(DonHang& don) {
    don.datTrangThai(make_unique<DaXacNhan>());
}

void DaXacNhan::chuyenTiep(DonHang& don) {
    don.datTrangThai(make_unique<DangGiao>());
}

void DangGiao::chuyenTiep(DonHang& don) {
    if (!don.coAnhGiaoHang()) {
        throw ChuyenTrangThaiSaiException("Cần ảnh đơn hàng do shipper chụp trước khi xác nhận giao thành công");
    }
    don.datTrangThai(make_unique<HoanTat>());
}

void HoanTat::chuyenTiep(DonHang& don) {
    if (!don.hangDaVeShop()) {
        throw ChuyenTrangThaiSaiException("Chỉ chuyển sang Trả hàng sau khi shipper đã gửi hàng trả về shop");
    }
    don.datTrangThai(make_unique<TraHang>());
}

void TraHang::chuyenTiep(DonHang&) {
    throw ChuyenTrangThaiSaiException("Đơn hàng đã ở trạng thái cuối (Trả hàng), không thể chuyển tiếp");
}

unique_ptr<TrangThaiDonHang> taoTrangThai(const string& ten) {
    if (ten == tt::MOI)       return make_unique<DonMoi>();
    if (ten == tt::XAC_NHAN)  return make_unique<DaXacNhan>();
    if (ten == tt::DANG_GIAO) return make_unique<DangGiao>();
    if (ten == tt::HOAN_TAT)  return make_unique<HoanTat>();
    if (ten == tt::TRA_HANG)  return make_unique<TraHang>();
    throw FileDataException("Trạng thái đơn hàng không hợp lệ: " + ten);
}
