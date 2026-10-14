#include "KhuyenMai_TrangThai.h"
#include "DonHang.h"
#include <algorithm>
#include <sstream>

using namespace std;

// Helper định dạng tiền nguyên VNĐ cho thông báo lỗi rõ ràng
static string formatTienVND(double soTien) {
    long long val = static_cast<long long>(soTien + 0.5);
    string s = to_string(val);
    int n = static_cast<int>(s.length()) - 3;
    while (n > 0) {
        s.insert(n, ".");
        n -= 3;
    }
    return s + "đ";
}

// ============================================================================
// TRIỂN KHAI: GiamPhanTram
// ============================================================================

GiamPhanTram::GiamPhanTram(string ma, double phanTram, double toiDa, double donToiThieu)
    : m_ma(std::move(ma)),
      m_phanTram(phanTram > 1.0 ? phanTram / 100.0 : phanTram)
      m_toiDa(toiDa),
      m_donToiThieu(donToiThieu) {}

bool GiamPhanTram::apDungDuoc(const GioHang& gio) const {
    return gio.tinhTongTien() >= m_donToiThieu;
}

double GiamPhanTram::tinhGiam(const GioHang& gio, double /*phiShip*/) const {
    if (!apDungDuoc(gio)) {
        throw KhuyenMaiException(m_ma + " cần giỏ >= " + formatTienVND(m_donToiThieu));
    }
    double tienHang = gio.tinhTongTien();
    double mucGiam = tienHang * m_phanTram;
    if (m_toiDa > 0.0 && mucGiam > m_toiDa) {
        mucGiam = m_toiDa;
    }
    return min(mucGiam, tienHang);
}

string GiamPhanTram::getMa() const {
    return m_ma;
}

unique_ptr<KhuyenMai> GiamPhanTram::clone() const {
    return make_unique<GiamPhanTram>(*this);
}

double GiamPhanTram::getPhanTram() const {
    return m_phanTram;
}

double GiamPhanTram::getToiDa() const {
    return m_toiDa;
}

double GiamPhanTram::getDonToiThieu() const {
    return m_donToiThieu;
}

// ============================================================================
// TRIỂN KHAI: GiamCoDinh
// ============================================================================

GiamCoDinh::GiamCoDinh(string ma, double soTien, int soNhaBanToiThieu)
    : m_ma(std::move(ma)),
      m_soTien(soTien),
      m_soNhaBanToiThieu(soNhaBanToiThieu) {}

bool GiamCoDinh::apDungDuoc(const GioHang& gio) const {
    return gio.demNhaBan() >= m_soNhaBanToiThieu;
}

double GiamCoDinh::tinhGiam(const GioHang& gio, double /*phiShip*/) const {
    if (!apDungDuoc(gio)) {
        throw KhuyenMaiException(m_ma + " cần >= " + to_string(m_soNhaBanToiThieu) + " nhà bán");
    }
    double tienHang = gio.tinhTongTien();
    return min(m_soTien, tienHang);
}

string GiamCoDinh::getMa() const {
    return m_ma;
}

unique_ptr<KhuyenMai> GiamCoDinh::clone() const {
    return make_unique<GiamCoDinh>(*this);
}

double GiamCoDinh::getSoTien() const {
    return m_soTien;
}

int GiamCoDinh::getSoNhaBanToiThieu() const {
    return m_soNhaBanToiThieu;
}

// ============================================================================
// TRIỂN KHAI: MienPhiShip
// ============================================================================

MienPhiShip::MienPhiShip(string ma, double donToiThieu)
    : m_ma(std::move(ma)),
      m_donToiThieu(donToiThieu) {}

bool MienPhiShip::apDungDuoc(const GioHang& gio) const {
    return gio.tinhTongTien() >= m_donToiThieu;
}

double MienPhiShip::tinhGiam(const GioHang& gio, double phiShip) const {
    if (!apDungDuoc(gio)) {
        throw KhuyenMaiException(m_ma + " cần giỏ >= " + formatTienVND(m_donToiThieu));
    }
    return max(0.0, phiShip);
}

string MienPhiShip::getMa() const {
    return m_ma;
}

unique_ptr<KhuyenMai> MienPhiShip::clone() const {
    return make_unique<MienPhiShip>(*this);
}

double MienPhiShip::getDonToiThieu() const {
    return m_donToiThieu;
}

// ============================================================================
// TRIỂN KHAI: 5 LỚP TRẠNG THÁI (State Pattern)
// ============================================================================

// --- 1. TTMoi ---
void TTMoi::tiepTuc(DonHang& don) {
    don.datTrangThai(make_unique<TTXacNhan>());
}

void TTMoi::traHang(DonHang& /*don*/) {
    throw TrangThaiException("Chỉ trả hàng khi đơn đang giao hoặc đã hoàn tất");
}

string TTMoi::ten() const {
    return "Mới";
}

unique_ptr<TrangThaiDon> TTMoi::clone() const {
    return make_unique<TTMoi>(*this);
}

// --- 2. TTXacNhan ---
void TTXacNhan::tiepTuc(DonHang& don) {
    don.datTrangThai(make_unique<TTDangGiao>());
}

void TTXacNhan::traHang(DonHang& /*don*/) {
    throw TrangThaiException("Chỉ trả hàng khi đơn đang giao hoặc đã hoàn tất");
}

string TTXacNhan::ten() const {
    return "Xác nhận";
}

unique_ptr<TrangThaiDon> TTXacNhan::clone() const {
    return make_unique<TTXacNhan>(*this);
}

// --- 3. TTDangGiao ---
void TTDangGiao::tiepTuc(DonHang& don) {
    don.datTrangThai(make_unique<TTHoanTat>());
}

void TTDangGiao::traHang(DonHang& don) {
    don.datTrangThai(make_unique<TTTraHang>());
}

string TTDangGiao::ten() const {
    return "Đang giao";
}

unique_ptr<TrangThaiDon> TTDangGiao::clone() const {
    return make_unique<TTDangGiao>(*this);
}

// --- 4. TTHoanTat ---
void TTHoanTat::tiepTuc(DonHang& /*don*/) {
    throw TrangThaiException("Đơn đã kết thúc, không thể tiếp tục");
}

void TTHoanTat::traHang(DonHang& don) {
    don.datTrangThai(make_unique<TTTraHang>());
}

string TTHoanTat::ten() const {
    return "Hoàn tất";
}

unique_ptr<TrangThaiDon> TTHoanTat::clone() const {
    return make_unique<TTHoanTat>(*this);
}

// --- 5. TTTraHang ---
void TTTraHang::tiepTuc(DonHang& /*don*/) {
    throw TrangThaiException("Đơn đã kết thúc, không thể tiếp tục");
}

void TTTraHang::traHang(DonHang& /*don*/) {
    throw TrangThaiException("Chỉ trả hàng khi đơn đang giao hoặc đã hoàn tất");
}

string TTTraHang::ten() const {
    return "Trả hàng";
}

unique_ptr<TrangThaiDon> TTTraHang::clone() const {
    return make_unique<TTTraHang>(*this);
}
