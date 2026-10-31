#pragma once
#include "Common.h"
// Người phụ trách: B | nhánh feature/design-patterns | LỚP 1 | State Pattern (chỉ forward-declare DonHang, .cpp mới include DonHang.h)
class DonHang;
class TrangThaiDonHang {                             // abstract
public:
    virtual ~TrangThaiDonHang() = default;
    virtual void   chuyenTiep(DonHang& don) = 0;     // sai luồng -> ChuyenTrangThaiSaiException; thành công thì gọi don.datTrangThai(trạng thái kế tiếp)
    virtual string ten() const = 0;                  // một trong tt::*  (cũng là chuỗi lưu file và hiện lên web)
    virtual unique_ptr<TrangThaiDonHang> clone() const = 0;
};
struct LichSuTrangThai { string trangThai; string thoiDiem; };   // phục vụ tracking
// QUYẾT ĐỊNH: DonHang::datTrangThai() là NƠI DUY NHẤT ghi LichSuTrangThai (B-5). Các lớp trạng thái chỉ gọi datTrangThai, không tự ghi lịch sử.
class DonMoi    : public TrangThaiDonHang { public: void chuyenTiep(DonHang&) override; string ten() const override { return tt::MOI; }       unique_ptr<TrangThaiDonHang> clone() const override { return make_unique<DonMoi>(*this); } };      // -> Xác nhận
class DaXacNhan : public TrangThaiDonHang { public: void chuyenTiep(DonHang&) override; string ten() const override { return tt::XAC_NHAN; }  unique_ptr<TrangThaiDonHang> clone() const override { return make_unique<DaXacNhan>(*this); } };   // -> Đang giao
class DangGiao  : public TrangThaiDonHang { public: void chuyenTiep(DonHang&) override; string ten() const override { return tt::DANG_GIAO; }  unique_ptr<TrangThaiDonHang> clone() const override { return make_unique<DangGiao>(*this); } };    // -> Hoàn tất, CHỈ khi don.coAnhGiaoHang() (B-8)
class HoanTat   : public TrangThaiDonHang { public: void chuyenTiep(DonHang&) override; string ten() const override { return tt::HOAN_TAT; }   unique_ptr<TrangThaiDonHang> clone() const override { return make_unique<HoanTat>(*this); } };     // -> Trả hàng, CHỈ khi don.hangDaVeShop() (đi qua luồng trả hàng; route chung không đi được bước này)
class TraHang   : public TrangThaiDonHang { public: void chuyenTiep(DonHang&) override; string ten() const override { return tt::TRA_HANG; }   unique_ptr<TrangThaiDonHang> clone() const override { return make_unique<TraHang>(*this); } };     // cuối: luôn ném ChuyenTrangThaiSaiException
unique_ptr<TrangThaiDonHang> taoTrangThai(const string& ten);   // TODO(B-6) factory theo tt::*; tên lạ -> FileDataException
// TODO(B-4) cài 5 hàm chuyenTiep trong TrangThaiDonHang.cpp; TODO(B-8), TODO(B-5) như ghi chú ở trên.
