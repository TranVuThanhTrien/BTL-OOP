#pragma once
#include <string>
#include <memory>
#include "Exceptions.h"

// Forward declarations để tránh vòng lặp #include với DonHang.h
class GioHang;
class DonHang;

// ============================================================================
// PHẦN 1: KHUYẾN MÃI (Abstract Class + 3 loại khuyến mãi)
// ============================================================================

class KhuyenMai {
public:
    virtual ~KhuyenMai() = default;

    // Kiểm tra giỏ hàng có đủ điều kiện áp dụng mã hay không
    virtual bool apDungDuoc(const GioHang& gio) const = 0;

    // Tính số tiền được giảm; ném KhuyenMaiException nếu không đủ điều kiện
    virtual double tinhGiam(const GioHang& gio, double phiShip) const = 0;

    // Trả về mã khuyến mãi (VD: "GIAM10", "GIAM30K", "FREESHIP")
    virtual std::string getMa() const = 0;

    // Nhân bản đối tượng phục vụ Big 5 của lớp DonHang
    virtual std::unique_ptr<KhuyenMai> clone() const = 0;
};

// 1. Giảm theo phần trăm (VD: GIAM10: -10%, tối đa 50k, đơn tối thiểu 200k)
class GiamPhanTram : public KhuyenMai {
private:
    std::string m_ma;
    double m_phanTram;      // Tỷ lệ giảm (VD: 0.1 cho 10%, hoặc nhập 10 cũng tự chuẩn hóa)
    double m_toiDa;         // Số tiền giảm tối đa (VNĐ)
    double m_donToiThieu;   // Giá trị giỏ hàng tối thiểu (VNĐ)

public:
    // Khởi tạo mã giảm theo phần trăm (mặc định cấu hình chuẩn của mã GIAM10)
    explicit GiamPhanTram(std::string ma = "GIAM10", double phanTram = 0.1, double toiDa = 50000.0, double donToiThieu = 200000.0);

    // Kiểm tra tổng tiền giỏ hàng >= m_donToiThieu
    bool apDungDuoc(const GioHang& gio) const override;

    // Tính mức giảm phần trăm (không vượt quá m_toiDa); ném KhuyenMaiException nếu không đủ điều kiện
    double tinhGiam(const GioHang& gio, double phiShip) const override;

    // Lấy mã khuyến mãi
    std::string getMa() const override;

    // Nhân bản đối tượng GiamPhanTram
    std::unique_ptr<KhuyenMai> clone() const override;

    double getPhanTram() const;
    double getToiDa() const;
    double getDonToiThieu() const;
};

// 2. Giảm số tiền cố định (VD: GIAM30K: -30k khi mua từ >= 2 nhà bán)
class GiamCoDinh : public KhuyenMai {
private:
    std::string m_ma;
    double m_soTien;            // Số tiền giảm cố định (VNĐ)
    int m_soNhaBanToiThieu;     // Số lượng nhà bán tối thiểu trong giỏ

public:
    // Khởi tạo mã giảm tiền cố định (mặc định cấu hình chuẩn của mã GIAM30K)
    explicit GiamCoDinh(std::string ma = "GIAM30K", double soTien = 30000.0, int soNhaBanToiThieu = 2);

    // Kiểm tra số nhà bán trong giỏ >= m_soNhaBanToiThieu
    bool apDungDuoc(const GioHang& gio) const override;

    // Trả về số tiền giảm cố định (tối đa bằng tổng tiền hàng); ném KhuyenMaiException nếu sai điều kiện
    double tinhGiam(const GioHang& gio, double phiShip) const override;

    // Lấy mã khuyến mãi
    std::string getMa() const override;

    // Nhân bản đối tượng GiamCoDinh
    std::unique_ptr<KhuyenMai> clone() const override;

    double getSoTien() const;
    int getSoNhaBanToiThieu() const;
};

// 3. Miễn phí vận chuyển (VD: FREESHIP: miễn 100% phí ship cho giỏ >= 300k)
class MienPhiShip : public KhuyenMai {
private:
    std::string m_ma;
    double m_donToiThieu;       // Giá trị giỏ hàng tối thiểu để được miễn ship (VNĐ)

public:
    // Khởi tạo mã miễn phí vận chuyển (mặc định cấu hình chuẩn của mã FREESHIP)
    explicit MienPhiShip(std::string ma = "FREESHIP", double donToiThieu = 300000.0);

    // Kiểm tra tổng tiền giỏ hàng >= m_donToiThieu
    bool apDungDuoc(const GioHang& gio) const override;

    // Trả về mức giảm đúng bằng phiShip; ném KhuyenMaiException nếu giỏ chưa đạt mức tối thiểu
    double tinhGiam(const GioHang& gio, double phiShip) const override;

    // Lấy mã khuyến mãi
    std::string getMa() const override;

    // Nhân bản đối tượng MienPhiShip
    std::unique_ptr<KhuyenMai> clone() const override;

    double getDonToiThieu() const;
};


// ============================================================================
// PHẦN 2: TRẠNG THÁI ĐƠN HÀNG (State Pattern - 5 trạng thái)
// Luồng chuẩn: Mới -> Xác nhận -> Đang giao -> Hoàn tất
// Luồng trả hàng: Đang giao / Hoàn tất -> Trả hàng
// ============================================================================

class TrangThaiDon {
public:
    virtual ~TrangThaiDon() = default;

    // Chuyển đơn hàng sang trạng thái kế tiếp; ném TrangThaiException nếu sai luồng
    virtual void tiepTuc(DonHang& don) = 0;

    // Chuyển đơn hàng sang trạng thái Trả hàng; ném TrangThaiException nếu không hợp lệ
    virtual void traHang(DonHang& don) = 0;

    // Trả về tên trạng thái chuẩn ("Mới", "Xác nhận", "Đang giao", "Hoàn tất", "Trả hàng")
    virtual std::string ten() const = 0;

    // Nhân bản đối tượng phục vụ Big 5 của lớp DonHang
    virtual std::unique_ptr<TrangThaiDon> clone() const = 0;
};

// 1. Trạng thái Mới
class TTMoi : public TrangThaiDon {
public:
    // Chuyển từ "Mới" sang "Xác nhận"
    void tiepTuc(DonHang& don) override;

    // Đơn mới chưa giao không thể trả hàng -> ném TrangThaiException
    void traHang(DonHang& don) override;

    // Trả về chuỗi "Mới"
    std::string ten() const override;

    // Nhân bản trạng thái TTMoi
    std::unique_ptr<TrangThaiDon> clone() const override;
};

// 2. Trạng thái Xác nhận
class TTXacNhan : public TrangThaiDon {
public:
    // Chuyển từ "Xác nhận" sang "Đang giao"
    void tiepTuc(DonHang& don) override;

    // Đơn mới xác nhận chưa giao không thể trả hàng -> ném TrangThaiException
    void traHang(DonHang& don) override;

    // Trả về chuỗi "Xác nhận"
    std::string ten() const override;

    // Nhân bản trạng thái TTXacNhan
    std::unique_ptr<TrangThaiDon> clone() const override;
};

// 3. Trạng thái Đang giao
class TTDangGiao : public TrangThaiDon {
public:
    // Chuyển từ "Đang giao" sang "Hoàn tất"
    void tiepTuc(DonHang& don) override;

    // Chuyển từ "Đang giao" sang "Trả hàng"
    void traHang(DonHang& don) override;

    // Trả về chuỗi "Đang giao"
    std::string ten() const override;

    // Nhân bản trạng thái TTDangGiao
    std::unique_ptr<TrangThaiDon> clone() const override;
};

// 4. Trạng thái Hoàn tất
class TTHoanTat : public TrangThaiDon {
public:
    // Đơn đã hoàn tất không thể tiếp tục -> ném TrangThaiException
    void tiepTuc(DonHang& don) override;

    // Chuyển từ "Hoàn tất" sang "Trả hàng"
    void traHang(DonHang& don) override;

    // Trả về chuỗi "Hoàn tất"
    std::string ten() const override;

    // Nhân bản trạng thái TTHoanTat
    std::unique_ptr<TrangThaiDon> clone() const override;
};

// 5. Trạng thái Trả hàng
class TTTraHang : public TrangThaiDon {
public:
    // Đơn đã trả hàng là kết thúc, không thể tiếp tục -> ném TrangThaiException
    void tiepTuc(DonHang& don) override;

    // Đơn đã ở trạng thái trả hàng rồi -> ném TrangThaiException
    void traHang(DonHang& don) override;

    // Trả về chuỗi "Trả hàng"
    std::string ten() const override;

    // Nhân bản trạng thái TTTraHang
    std::unique_ptr<TrangThaiDon> clone() const override;
};
