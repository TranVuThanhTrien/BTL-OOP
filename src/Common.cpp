#include "Common.h"
#include <cctype>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <random>
#include <cstdint>

string thoiGianHienTai() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[32];
    std::strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}
string inHoa(string s)   { for (char& c : s) c = (char)std::toupper((unsigned char)c); return s; }
string inThuong(string s){ for (char& c : s) c = (char)std::tolower((unsigned char)c); return s; }
double lamTronDong(double x) { return std::round(x); }
string dinhDangTien(double soTien) {
    long long v = (long long)std::llround(soTien);
    bool am = v < 0; if (am) v = -v;
    string s = std::to_string(v), r;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (i && (s.size() - i) % 3 == 0) r += '.';
        r += s[i];
    }
    return (am ? "-" : "") + r + "đ";
}
bool laMaShopHopLe(const string& ma) {
    if (ma.size() < hangso::MA_SHOP_MIN || ma.size() > hangso::MA_SHOP_MAX) return false;
    return std::all_of(ma.begin(), ma.end(), [](unsigned char c){ return std::isalpha(c) != 0; });
}
bool laSoDienThoaiHopLe(const string& s) {
    if (s.size() < hangso::SDT_MIN || s.size() > hangso::SDT_MAX) return false;
    return std::all_of(s.begin(), s.end(), [](unsigned char c){ return std::isdigit(c) != 0; });
}

// Băm có muối (FNV-1a 64 bit, lặp 1000 lần). Chỉ là giữ chỗ để "không lưu mật khẩu thuần" (C-8); C có thể thay bằng SHA-256 mà không đổi chữ ký.
static string bamVoiMuoi(const string& muoi, const string& mk) {
    std::uint64_t h = 1469598103934665603ULL;
    string x = muoi + mk;
    for (int vong = 0; vong < 1000; ++vong)
        for (unsigned char c : x) { h ^= c; h *= 1099511628211ULL; }
    char buf[17]; std::snprintf(buf, sizeof buf, "%016llx", (unsigned long long)h);
    return buf;
}
string bamMatKhau(const string& matKhauThuan) {
    std::random_device rd; std::mt19937_64 g(((std::uint64_t)rd() << 32) ^ rd());
    char m[17]; std::snprintf(m, sizeof m, "%016llx", (unsigned long long)g());
    return string(m) + "$" + bamVoiMuoi(m, matKhauThuan);
}
bool kiemTraBam(const string& matKhauThuan, const string& bam) {
    auto vt = bam.find('$');
    if (vt == string::npos) return false;
    return bamVoiMuoi(bam.substr(0, vt), matKhauThuan) == bam.substr(vt + 1);
}
