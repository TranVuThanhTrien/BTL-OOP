#pragma once
#include "Common.h"
// Người phụ trách: B | nhánh feature/design-patterns | LỚP 1 | Strategy vận chuyển
class DonViVanChuyen {                               // abstract
public:
    virtual ~DonViVanChuyen() = default;
    virtual double tinhPhiShip(double km, double kg) const = 0;   // = phiCoBan + km*donGiaKm + kg*donGiaKg, km<=0 hoặc kg<=0 -> OrderException (B-2); đã lamTronDong
    virtual int    uocTinhNgay(double km) const = 0;
    virtual string ten() const = 0;                               // một trong hang::*
    virtual unique_ptr<DonViVanChuyen> clone() const = 0;
};
// Bảng giá sửa được (D-11: /api/quan-ly/van-chuyen/{hang}). Mặc định khớp web: GHN 16000+300/km+3000/kg, 1 ngày; GHTK 14000+250+2500, 2 ngày; ViettelPost 15000+400+2000, 2 ngày.
// Ngày dự kiến = ngayCoBan + km / kmMoiNgay (kmMoiNgay mặc định 300, chia nguyên). KHÔNG lưu file.
// Đơn đã đặt KHÔNG đổi phí khi bảng giá đổi vì DonHang::chotDon() chụp lại phí ship (xem DonHang.h).
struct BangGiaShip { double phiCoBan, donGiaKm, donGiaKg; int ngayCoBan; double kmMoiNgay = 300; };
BangGiaShip& bangGiaShip(const string& hang);         // TODO(B-3) hãng lạ -> OrderException
// Gom công thức dùng chung để 3 hãng không lặp code; GHN/GHTK/ViettelPost chỉ khác ten() và clone().
class DonViTheoBangGia : public DonViVanChuyen {
public:
    double tinhPhiShip(double km, double kg) const override;   // TODO(B-1,B-2) đọc bangGiaShip(ten())
    int    uocTinhNgay(double km) const override;              // TODO(B-1)
};
class GHN          : public DonViTheoBangGia { public: string ten() const override { return hang::GHN; }           unique_ptr<DonViVanChuyen> clone() const override { return make_unique<GHN>(*this); } };
class GHTK         : public DonViTheoBangGia { public: string ten() const override { return hang::GHTK; }          unique_ptr<DonViVanChuyen> clone() const override { return make_unique<GHTK>(*this); } };
class ViettelPost  : public DonViTheoBangGia { public: string ten() const override { return hang::VIETTEL_POST; }  unique_ptr<DonViVanChuyen> clone() const override { return make_unique<ViettelPost>(*this); } };
unique_ptr<DonViVanChuyen> taoDonVi(const string& ten);          // TODO(B-3) factory; tên lạ -> OrderException
string timHangReNhat(double km, double kg);                      // TODO(B-3) so 3 hãng; hòa thì lấy hãng đứng trước theo thứ tự GHN, GHTK, ViettelPost
vector<string> danhSachHang();                                   // {GHN, GHTK, ViettelPost}
