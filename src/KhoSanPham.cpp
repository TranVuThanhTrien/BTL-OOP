#include "KhoSanPham.h"
#include "OrderException.h"
// Người phụ trách: A | A-23, D-21
// Thứ tự trong m_ds là thứ tự thêm vào; "moiNhat" lấy phần tử thêm sau lên trước (khớp web: P.indexOf(b) - P.indexOf(a)).

namespace {
bool chuaTuKhoa(const SanPham& sp, const string& tuKhoaThuong) {
    if (tuKhoaThuong.empty()) return true;
    return inThuong(sp.ten()).find(tuKhoaThuong) != string::npos
        || inThuong(sp.ma()).find(tuKhoaThuong)  != string::npos;
}
} // namespace

vector<SanPham> KhoSanPham::tim(const string& tuKhoa, const string& maShop, const string& sapXep) const {
    const string tk = inThuong(tuKhoa), shop = inThuong(maShop);
    // Lọc, giữ kèm vị trí gốc để sắp xếp "moiNhat" và để sắp xếp ổn định khi bằng điểm.
    vector<pair<size_t, const SanPham*>> ds;
    for (size_t i = 0; i < m_ds.size(); ++i) {
        const SanPham& sp = m_ds[i];
        if (!shop.empty() && inThuong(sp.maNguoiBan()) != shop) continue;
        if (!chuaTuKhoa(sp, tk)) continue;
        ds.emplace_back(i, &sp);
    }
    using P = pair<size_t, const SanPham*>;
    if (sapXep == "moiNhat") {
        std::stable_sort(ds.begin(), ds.end(), [](const P& a, const P& b) { return a.first > b.first; });
    } else if (sapXep == "banChay") {
        std::stable_sort(ds.begin(), ds.end(), [](const P& a, const P& b) { return a.second->daBan() > b.second->daBan(); });
    } else if (sapXep == "giaTang") {
        std::stable_sort(ds.begin(), ds.end(), [](const P& a, const P& b) { return a.second->donGia() < b.second->donGia(); });
    } else if (sapXep == "giaGiam") {
        std::stable_sort(ds.begin(), ds.end(), [](const P& a, const P& b) { return a.second->donGia() > b.second->donGia(); });
    } else {   // "phoBien" hoặc rỗng/không biết: daBan x danhGia giảm dần (khớp web: sold * rate)
        std::stable_sort(ds.begin(), ds.end(), [](const P& a, const P& b) {
            return a.second->daBan() * a.second->danhGia() > b.second->daBan() * b.second->danhGia();
        });
    }
    vector<SanPham> kq;
    kq.reserve(ds.size());
    for (const auto& p : ds) kq.push_back(*p.second);
    return kq;
}

const SanPham& KhoSanPham::lay(const string& maSP) const {
    for (const auto& sp : m_ds) if (sp.ma() == maSP) return sp;
    throw OrderException("Không tìm thấy sản phẩm: " + maSP);
}
SanPham& KhoSanPham::lay(const string& maSP) {
    for (auto& sp : m_ds) if (sp.ma() == maSP) return sp;
    throw OrderException("Không tìm thấy sản phẩm: " + maSP);
}

void KhoSanPham::them(SanPham sp) {
    for (const auto& x : m_ds)
        if (x.ma() == sp.ma()) throw OrderException("Mã sản phẩm đã tồn tại: " + sp.ma());
    m_ds.push_back(std::move(sp));
}

void KhoSanPham::xoa(const string& maSP) {
    auto it = std::find_if(m_ds.begin(), m_ds.end(), [&](const SanPham& s) { return s.ma() == maSP; });
    if (it == m_ds.end()) throw OrderException("Không tìm thấy sản phẩm: " + maSP);
    m_ds.erase(it);
}

void KhoSanPham::nhapKho(const string& maSP, int soLuong) { lay(maSP).tangTon(soLuong); }
