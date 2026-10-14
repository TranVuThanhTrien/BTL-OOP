#pragma once
#include "Common.h"
// Người phụ trách: B | nhánh feature/design-patterns | Strategy vận chuyển
class DonViVanChuyen {                               // abstract
public:
    virtual ~DonViVanChuyen() = default;
    virtual double tinhPhiShip(double km, double kg) const = 0;   // = phiCoBan + km*donGiaKm + kg*donGiaKg
    virtual int    uocTinhNgay(double km) const = 0;
    virtual string ten() const = 0;
    virtual unique_ptr<DonViVanChuyen> clone() const = 0;
};
// TODO(B-1): class GHN         // 16000 + 300/km + 3000/kg, giao dự kiến 1 ngày + km/300
// TODO(B-1): class GHTK        // 14000 + 250/km + 2500/kg, 2 ngày + km/300
// TODO(B-1): class ViettelPost // 15000 + 400/km + 2000/kg, 2 ngày + km/300
// TODO(B-2): kiểm tra km <= 0 hoặc kg <= 0 -> ném OrderException
// TODO(B-3): unique_ptr<DonViVanChuyen> taoDonVi(const string& ten);        // factory
// TODO(B-3): string timHangReNhat(double km, double kg);                    // so sánh phí 3 hãng
