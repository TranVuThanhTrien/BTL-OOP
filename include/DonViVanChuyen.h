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
// Bảng giá sửa được (D-11: /api/quan-ly/van-chuyen/{hang}).
// Đơn giá thực tế rõ rệt theo từng km:
// - GHN:         15.000đ cơ bản + 2.500đ/km + 3.000đ/kg, 1 ngày
// - GHTK:        12.000đ cơ bản + 2.000đ/km + 2.500đ/kg, 2 ngày
// - ViettelPost: 14.000đ cơ bản + 2.200đ/km + 2.000đ/kg, 2 ngày
// Ngày dự kiến = ngayCoBan + km / kmMoiNgay (kmMoiNgay mặc định 50, chia nguyên). KHÔNG lưu file.
// Đơn đã đặt KHÔNG đổi phí khi bảng giá đổi vì DonHang::chotDon() chụp lại phí ship (xem DonHang.h).
struct BangGiaShip { double phiCoBan, donGiaKm, donGiaKg; int ngayCoBan; double kmMoiNgay = 50; };
BangGiaShip& bangGiaShip(const string& hang);
// Gom công thức dùng chung để 3 hãng không lặp code; GHN/GHTK/ViettelPost chỉ khác ten() và clone().
class DonViTheoBangGia : public DonViVanChuyen {
public:
    double tinhPhiShip(double km, double kg) const override;
    int    uocTinhNgay(double km) const override;
};
class GHN          : public DonViTheoBangGia { public: string ten() const override { return hang::GHN; }           unique_ptr<DonViVanChuyen> clone() const override { return make_unique<GHN>(*this); } };
class GHTK         : public DonViTheoBangGia { public: string ten() const override { return hang::GHTK; }          unique_ptr<DonViVanChuyen> clone() const override { return make_unique<GHTK>(*this); } };
class ViettelPost  : public DonViTheoBangGia { public: string ten() const override { return hang::VIETTEL_POST; }  unique_ptr<DonViVanChuyen> clone() const override { return make_unique<ViettelPost>(*this); } };
unique_ptr<DonViVanChuyen> taoDonVi(const string& ten);
string timHangReNhat(double km, double kg);
vector<string> danhSachHang();                                   // {GHN, GHTK, ViettelPost}
