#include "KhoSanPham.h"
#include "OrderException.h"

// Người phụ trách: A | nhánh feature/core-domain | LỚP 3
vector<SanPham> KhoSanPham::tim(const string& tuKhoa, const string& maShop, const string& sapXep) const {
    string tk = inThuong(tuKhoa);
    string shop = inHoa(maShop);
    vector<SanPham> kq;
    for (const auto& sp : m_ds) {
        if (!shop.empty() && shop != "*" && sp.maNguoiBan() != shop) continue;
        if (!tk.empty() && inThuong(sp.ten()).find(tk) == string::npos &&
            inThuong(sp.ma()).find(tk) == string::npos) {
            continue;
        }
        kq.push_back(sp);
    }

    string sx = sapXep.empty() ? "phoBien" : sapXep;
    if (sx == "moiNhat") {
        std::reverse(kq.begin(), kq.end());
    } else if (sx == "banChay") {
        std::stable_sort(kq.begin(), kq.end(), [](const SanPham& a, const SanPham& b) {
            return a.daBan() > b.daBan();
        });
    } else if (sx == "giaTang") {
        std::stable_sort(kq.begin(), kq.end(), [](const SanPham& a, const SanPham& b) {
            return a.donGia() < b.donGia();
        });
    } else if (sx == "giaGiam") {
        std::stable_sort(kq.begin(), kq.end(), [](const SanPham& a, const SanPham& b) {
            return a.donGia() > b.donGia();
        });
    } else {
        // Mặc định: phoBien (daBan * danhGia giảm dần)
        std::stable_sort(kq.begin(), kq.end(), [](const SanPham& a, const SanPham& b) {
            double diemA = a.daBan() * (a.danhGia() > 0 ? a.danhGia() : 1.0);
            double diemB = b.daBan() * (b.danhGia() > 0 ? b.danhGia() : 1.0);
            return diemA > diemB;
        });
    }
    return kq;
}

const SanPham& KhoSanPham::lay(const string& maSP) const {
    for (const auto& sp : m_ds) {
        if (sp.ma() == maSP) return sp;
    }
    throw OrderException("Không tìm thấy sản phẩm có mã: " + maSP);
}

SanPham& KhoSanPham::lay(const string& maSP) {
    for (auto& sp : m_ds) {
        if (sp.ma() == maSP) return sp;
    }
    throw OrderException("Không tìm thấy sản phẩm có mã: " + maSP);
}

void KhoSanPham::them(SanPham sp) {
    for (const auto& h : m_ds) {
        if (inHoa(h.ma()) == inHoa(sp.ma())) {
            throw OrderException("Mã sản phẩm đã tồn tại: " + sp.ma());
        }
    }
    m_ds.push_back(std::move(sp));
}

void KhoSanPham::xoa(const string& maSP) {
    auto it = std::find_if(m_ds.begin(), m_ds.end(), [&](const SanPham& s) { return s.ma() == maSP; });
    if (it == m_ds.end()) {
        throw OrderException("Không tìm thấy sản phẩm để xóa: " + maSP);
    }
    m_ds.erase(it);
}

void KhoSanPham::nhapKho(const string& maSP, int soLuong) {
    lay(maSP).tangTon(soLuong);
}
