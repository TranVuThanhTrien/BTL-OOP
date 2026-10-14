#pragma once
#include "GioHang.h"
// Người phụ trách: A | nhánh feature/core-domain
class KhuyenMai {                                    // abstract
public:
    virtual ~KhuyenMai() = default;
    virtual bool   dieuKienHopLe(const GioHang& gio) const = 0;
    virtual double apDung(const GioHang& gio, double phiShip) const = 0;   // số tiền được giảm
    virtual string ma() const = 0;
    virtual unique_ptr<KhuyenMai> clone() const = 0;
};
// TODO(A-6): class KMPhanTram : public KhuyenMai   // GIAM10: giảm 10%, tối đa 50.000đ, giỏ >= 200.000đ
// TODO(A-6): class KMGiamTien : public KhuyenMai   // GIAM30K: giảm 30.000đ khi mua từ 2 nhà bán
// TODO(A-6): class KMFreeShip : public KhuyenMai   // FREESHIP: miễn ship khi giỏ >= 300.000đ
// TODO(A-7): unique_ptr<KhuyenMai> taoKhuyenMai(const string& ma);  // factory; mã sai -> MaGiamGiaKhongHopLeException
