#pragma once
#include "GioHang.h"
// Người phụ trách: A | nhánh feature/core-domain | LỚP 3 | Strategy khuyến mãi. Hằng số lấy từ hangso::* (Common.h), mã từ km::*
class KhuyenMai {                                    // abstract
public:
    virtual ~KhuyenMai() = default;
    virtual bool   dieuKienHopLe(const TomTatGio& gio) const = 0;
    virtual double apDung(const TomTatGio& gio, double phiShip) const = 0;   // số tiền được giảm (đã lamTronDong), không bao giờ vượt tiền hàng + ship
    virtual string ma() const = 0;
    virtual unique_ptr<KhuyenMai> clone() const = 0;
};
class KMPhanTram : public KhuyenMai {                // TODO(A-6) GIAM10: 10%, tối đa 50.000đ, giỏ >= 200.000đ
public:
    bool dieuKienHopLe(const TomTatGio&) const override;  double apDung(const TomTatGio&, double) const override;
    string ma() const override { return km::GIAM10; }     unique_ptr<KhuyenMai> clone() const override { return make_unique<KMPhanTram>(*this); }
};
class KMGiamTien : public KhuyenMai {                // TODO(A-6) GIAM30K: giảm 30.000đ khi mua từ 2 nhà bán
public:
    bool dieuKienHopLe(const TomTatGio&) const override;  double apDung(const TomTatGio&, double) const override;
    string ma() const override { return km::GIAM30K; }    unique_ptr<KhuyenMai> clone() const override { return make_unique<KMGiamTien>(*this); }
};
class KMFreeShip : public KhuyenMai {                // TODO(A-6) FREESHIP: miễn ship khi giỏ >= 300.000đ (giảm đúng bằng phiShip)
public:
    bool dieuKienHopLe(const TomTatGio&) const override;  double apDung(const TomTatGio&, double) const override;
    string ma() const override { return km::FREESHIP; }   unique_ptr<KhuyenMai> clone() const override { return make_unique<KMFreeShip>(*this); }
};
// TODO(A-7): factory. Mã sai HOẶC đang bị quản lý tắt -> MaGiamGiaKhongHopLeException. Không phân biệt hoa thường.
unique_ptr<KhuyenMai> taoKhuyenMai(const string& ma);
// Bật/tắt khuyến mãi (D-11: /api/quan-ly/khuyen-mai/{ma}/bat-tat). Trạng thái là biến tĩnh trong KhuyenMai.cpp,
// KHÔNG lưu file; mặc định cả 3 mã đều bật. Truy cập đã được khóa bằng mutex chung của server (xem docs/KIEN_TRUC.md mục 6).
vector<string> danhSachMaKhuyenMai();                 // {GIAM10, GIAM30K, FREESHIP}
bool khuyenMaiDangBat(const string& ma);
void datKhuyenMaiBat(const string& ma, bool bat);     // mã không tồn tại -> MaGiamGiaKhongHopLeException
