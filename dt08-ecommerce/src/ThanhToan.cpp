#include "ThanhToan.h"
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>

namespace {

// ---------- tiện ích kiểm tra ----------
std::string soJson(double v) {
    std::ostringstream o;
    o << std::setprecision(15) << v;
    return o.str();
}

void kiemTraSoTien(double soTien) {
    if (!std::isfinite(soTien) || soTien <= 0)
        throw ThanhToanException("Số tiền thanh toán phải lớn hơn 0");
}

void kiemTraKhongAm(double v, const char* ten) {
    if (!std::isfinite(v) || v < 0)
        throw ThanhToanException(std::string(ten) + " không được âm");
}

// Bỏ khoảng trắng/gạch ngang, kiểm tra 13-19 chữ số và thuật toán Luhn.
std::string chuanHoaSoThe(const std::string& raw) {
    std::string s;
    for (char c : raw) {
        if (c == ' ' || c == '-') continue;
        if (!std::isdigit(static_cast<unsigned char>(c)))
            throw ThanhToanException("Số thẻ chỉ được chứa chữ số");
        s += c;
    }
    if (s.size() < 13 || s.size() > 19)
        throw ThanhToanException("Số thẻ phải có từ 13 đến 19 chữ số");
    int tong = 0;
    bool nhanDoi = false;
    for (int i = static_cast<int>(s.size()) - 1; i >= 0; --i) {
        int d = s[i] - '0';
        if (nhanDoi) { d *= 2; if (d > 9) d -= 9; }
        tong += d;
        nhanDoi = !nhanDoi;
    }
    if (tong % 10 != 0)
        throw ThanhToanException("Số thẻ không hợp lệ");
    return s;
}

// ---------- bộ đọc JSON tối giản (object phẳng + mảng object) ----------
using Map = std::map<std::string, std::string>;

[[noreturn]] void loiJson(const std::string& chiTiet) {
    throw FileException("JSON thanh toán sai định dạng: " + chiTiet);
}

void boQuaTrang(const std::string& s, size_t& i) {
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
}

std::string docChuoi(const std::string& s, size_t& i) {   // i đang ở dấu " mở đầu
    ++i;
    std::string r;
    while (i < s.size() && s[i] != '"') {
        if (s[i] == '\\' && i + 1 < s.size()) {
            ++i;
            switch (s[i]) {
                case 'n': r += '\n'; break;
                case 't': r += '\t'; break;
                default:  r += s[i];
            }
        } else r += s[i];
        ++i;
    }
    if (i >= s.size()) loiJson("chuỗi không đóng");
    ++i;
    return r;
}

Map docObject(const std::string& s) {   // giá trị số/bool giữ nguyên dạng chữ
    Map m;
    size_t i = 0;
    boQuaTrang(s, i);
    if (i >= s.size() || s[i] != '{') loiJson("thiếu '{'");
    ++i;
    while (true) {
        boQuaTrang(s, i);
        if (i >= s.size()) loiJson("object không đóng");
        if (s[i] == '}') break;
        if (s[i] != '"') loiJson("thiếu tên khoá");
        std::string key = docChuoi(s, i);
        boQuaTrang(s, i);
        if (i >= s.size() || s[i] != ':') loiJson("thiếu ':'");
        ++i;
        boQuaTrang(s, i);
        if (i >= s.size()) loiJson("thiếu giá trị");
        std::string val;
        if (s[i] == '"') val = docChuoi(s, i);
        else {
            while (i < s.size() && s[i] != ',' && s[i] != '}' &&
                   !std::isspace(static_cast<unsigned char>(s[i]))) val += s[i++];
        }
        m[key] = val;
        boQuaTrang(s, i);
        if (i < s.size() && s[i] == ',') ++i;
    }
    return m;
}

std::vector<std::string> tachMang(const std::string& s) {   // mảng cấp cao -> các chuỗi object
    std::vector<std::string> r;
    size_t i = 0;
    boQuaTrang(s, i);
    if (i >= s.size() || s[i] != '[') loiJson("thiếu '['");
    ++i;
    int sau = 0;
    bool trongChuoi = false;
    size_t dau = 0;
    for (; i < s.size(); ++i) {
        char c = s[i];
        if (trongChuoi) {
            if (c == '\\') ++i;
            else if (c == '"') trongChuoi = false;
            continue;
        }
        if (c == '"') trongChuoi = true;
        else if (c == '{') { if (sau++ == 0) dau = i; }
        else if (c == '}') {
            if (--sau < 0) loiJson("thừa '}'");
            if (sau == 0) r.push_back(s.substr(dau, i - dau + 1));
        }
        else if (c == ']' && sau == 0) return r;
    }
    loiJson("mảng không đóng");
}

const std::string& lay(const Map& m, const std::string& key) {
    auto it = m.find(key);
    if (it == m.end()) loiJson("thiếu trường \"" + key + "\"");
    return it->second;
}

double laySo(const Map& m, const std::string& key) {
    const std::string& v = lay(m, key);
    try {
        size_t pos = 0;
        double d = std::stod(v, &pos);
        if (pos != v.size()) loiJson("trường \"" + key + "\" không phải số");
        return d;
    } catch (const std::logic_error&) {
        loiJson("trường \"" + key + "\" không phải số");
    }
}

bool layBool(const Map& m, const std::string& key) {
    const std::string& v = lay(m, key);
    if (v == "true") return true;
    if (v == "false") return false;
    loiJson("trường \"" + key + "\" không phải true/false");
}

} // namespace

// ===================== COD =====================
COD::COD(double soTienCanThu, bool daThu) : m_soTienCanThu(soTienCanThu), m_daThu(daThu) {
    kiemTraKhongAm(soTienCanThu, "Số tiền cần thu");
}

bool COD::thanhToan(double soTien) {
    kiemTraSoTien(soTien);
    if (m_daThu) throw ThanhToanException("Đơn COD này đã thu tiền");
    m_soTienCanThu = soTien;   // chưa thu tiền thật, chờ giao hàng
    return true;
}

double COD::tinhPhiXuLy(double) const { return 0; }
std::string COD::getTen() const { return "COD"; }
std::unique_ptr<PhuongThucThanhToan> COD::clone() const { return std::make_unique<COD>(*this); }

void COD::xacNhanDaThu() {
    if (m_soTienCanThu <= 0) throw ThanhToanException("Chưa có khoản COD nào cần thu");
    if (m_daThu) throw ThanhToanException("Đơn COD này đã thu tiền");
    m_daThu = true;
}

bool COD::daThuTien() const { return m_daThu; }
double COD::getSoTienCanThu() const { return m_soTienCanThu; }

// ===================== Thẻ tín dụng =====================
TheTinDung::TheTinDung(const std::string& soThe, double hanMuc)
    : m_soThe(chuanHoaSoThe(soThe)), m_hanMuc(hanMuc) {
    kiemTraKhongAm(hanMuc, "Hạn mức");
}

TheTinDung::TheTinDung(KhoiPhucTag, std::string soTheDaAn, double hanMuc)
    : m_soThe(std::move(soTheDaAn)), m_hanMuc(hanMuc) {}

TheTinDung TheTinDung::khoiPhuc(const std::string& soTheCuoi4, double hanMucConLai) {
    if (soTheCuoi4.size() != 4)
        throw ThanhToanException("4 số cuối của thẻ không hợp lệ");
    for (char c : soTheCuoi4)
        if (!std::isdigit(static_cast<unsigned char>(c)))
            throw ThanhToanException("4 số cuối của thẻ không hợp lệ");
    kiemTraKhongAm(hanMucConLai, "Hạn mức");
    return TheTinDung(KhoiPhucTag{}, std::string(12, '*') + soTheCuoi4, hanMucConLai);
}

bool TheTinDung::thanhToan(double soTien) {
    kiemTraSoTien(soTien);
    double tong = soTien + tinhPhiXuLy(soTien);
    if (tong > m_hanMuc)
        throw ThanhToanException("Vượt hạn mức thẻ tín dụng");
    m_hanMuc -= tong;   // chỉ trừ khi thành công
    return true;
}

double TheTinDung::tinhPhiXuLy(double soTien) const { return soTien * PHI_THE_TIN_DUNG; }
std::string TheTinDung::getTen() const { return "Thẻ tín dụng"; }
std::unique_ptr<PhuongThucThanhToan> TheTinDung::clone() const { return std::make_unique<TheTinDung>(*this); }

std::string TheTinDung::getSoTheDaAn() const {
    return "**** **** **** " + m_soThe.substr(m_soThe.size() - 4);
}

double TheTinDung::getHanMucConLai() const { return m_hanMuc; }

// ===================== Ví điện tử =====================
ViDienTu::ViDienTu(double soDuBanDau) : m_soDu(soDuBanDau) {
    kiemTraKhongAm(soDuBanDau, "Số dư");
}

bool ViDienTu::thanhToan(double soTien) {
    kiemTraSoTien(soTien);
    if (soTien > m_soDu) throw ThanhToanException("Ví không đủ số dư");
    m_soDu -= soTien;
    return true;
}

double ViDienTu::tinhPhiXuLy(double) const { return 0; }
std::string ViDienTu::getTen() const { return "Ví điện tử"; }
std::unique_ptr<PhuongThucThanhToan> ViDienTu::clone() const { return std::make_unique<ViDienTu>(*this); }

void ViDienTu::napTien(double soTien) {
    kiemTraSoTien(soTien);
    m_soDu += soTien;
}

double ViDienTu::getSoDu() const { return m_soDu; }

// ===================== Lưu / đọc JSON =====================
std::string ThanhToanJson::toJson(const PhuongThucThanhToan& pt) {
    if (auto c = dynamic_cast<const COD*>(&pt))
        return "{\"loai\":\"COD\",\"soTienCanThu\":" + soJson(c->getSoTienCanThu()) +
               ",\"daThu\":" + (c->daThuTien() ? "true" : "false") + "}";
    if (auto t = dynamic_cast<const TheTinDung*>(&pt)) {
        std::string da = t->getSoTheDaAn();
        return "{\"loai\":\"TheTinDung\",\"soTheCuoi\":\"" + da.substr(da.size() - 4) +
               "\",\"hanMuc\":" + soJson(t->getHanMucConLai()) + "}";
    }
    if (auto v = dynamic_cast<const ViDienTu*>(&pt))
        return "{\"loai\":\"ViDienTu\",\"soDu\":" + soJson(v->getSoDu()) + "}";
    throw FileException("Không hỗ trợ lưu loại thanh toán: " + pt.getTen());
}

std::unique_ptr<PhuongThucThanhToan> ThanhToanJson::tuJson(const std::string& json) {
    const Map m = docObject(json);
    const std::string loai = lay(m, "loai");
    try {
        if (loai == "COD")      return std::make_unique<COD>(laySo(m, "soTienCanThu"), layBool(m, "daThu"));
        if (loai == "ViDienTu") return std::make_unique<ViDienTu>(laySo(m, "soDu"));
        if (loai == "TheTinDung")
            return std::make_unique<TheTinDung>(TheTinDung::khoiPhuc(lay(m, "soTheCuoi"), laySo(m, "hanMuc")));
    } catch (const ThanhToanException& e) {
        // dữ liệu trong file vi phạm ràng buộc nghiệp vụ -> coi là file hỏng
        throw FileException(std::string("Dữ liệu thanh toán không hợp lệ: ") + e.what());
    }
    loiJson("loại thanh toán không hỗ trợ: " + loai);
}

std::string ThanhToanJson::toJson(const DanhSach& ds) {
    std::string r = "[\n";
    for (size_t i = 0; i < ds.size(); ++i) {
        if (!ds[i]) throw FileException("Danh sách thanh toán chứa con trỏ rỗng");
        r += "  " + toJson(*ds[i]);
        r += (i + 1 < ds.size()) ? ",\n" : "\n";
    }
    return r + "]\n";
}

ThanhToanJson::DanhSach ThanhToanJson::tuJsonDanhSach(const std::string& json) {
    DanhSach ds;
    for (const std::string& obj : tachMang(json))
        ds.push_back(tuJson(obj));
    return ds;
}

void ThanhToanJson::luu(const DanhSach& ds, const std::string& duongDan) {
    namespace fs = std::filesystem;
    const std::string noiDung = toJson(ds);
    try {
        fs::path p(duongDan);
        if (p.has_parent_path()) fs::create_directories(p.parent_path());
        const std::string tam = duongDan + ".tmp";
        {
            std::ofstream f(tam, std::ios::binary | std::ios::trunc);
            if (!f) throw FileException("Không thể ghi file: " + tam);
            f << noiDung;
            f.flush();
            if (!f) throw FileException("Lỗi khi ghi file: " + tam);
        }
        fs::rename(tam, p);
    } catch (const fs::filesystem_error& e) {
        throw FileException(std::string("Lỗi hệ thống tệp: ") + e.what());
    }
}

ThanhToanJson::DanhSach ThanhToanJson::doc(const std::string& duongDan) {
    std::ifstream f(duongDan, std::ios::binary);
    if (!f) throw FileException("Không thể mở file: " + duongDan);
    std::ostringstream ss;
    ss << f.rdbuf();
    return tuJsonDanhSach(ss.str());
}
