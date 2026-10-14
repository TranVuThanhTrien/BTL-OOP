#pragma once
#include "Common.h"
// Người phụ trách: B | nhánh feature/design-patterns | State Pattern
class DonHang;
class TrangThaiDonHang {                             // abstract
public:
    virtual ~TrangThaiDonHang() = default;
    virtual void   chuyenTiep(DonHang& don) = 0;     // sai luồng -> ChuyenTrangThaiSaiException
    virtual string ten() const = 0;
    virtual unique_ptr<TrangThaiDonHang> clone() const = 0;
};
struct LichSuTrangThai { string trangThai; string thoiDiem; };   // phục vụ tracking
// TODO(B-4): DonMoi -> DaXacNhan -> DangGiao -> HoanTat -> TraHang (mỗi lớp tự quyết trạng thái kế tiếp)
// TODO(B-4): TraHang là trạng thái cuối, chuyenTiep() ném ChuyenTrangThaiSaiException
// TODO(B-8): DangGiao::chuyenTiep chỉ được sang HoanTat khi don.coAnhGiaoHang(), thiếu ảnh -> ChuyenTrangThaiSaiException
// TODO(B-5): ghi LichSuTrangThai (thời điểm + tên trạng thái) mỗi lần chuyển
// TODO(B-6): unique_ptr<TrangThaiDonHang> taoTrangThai(const string& ten);  // dùng khi đọc file
