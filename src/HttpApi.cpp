#include "DonHangService.h"
#include "ThongKeService.h"
#include "OrderException.h"
#include "httplib.h"
#include "json.hpp"
#include <mutex>
#include <filesystem>
#include <set>

using nlohmann::json;

namespace {
std::mutex apiMutex;

void jsonResponse(httplib::Response& res, const json& body, int status = 200) {
    res.status = status;
    res.set_content(body.dump(), "application/json; charset=utf-8");
}

json diaChiJson(const DiaChi& d) {
    return {{"hoTen", d.hoTen}, {"soDienThoai", d.soDienThoai}, {"diaChi", d.diaChi}};
}

json sanPhamJson(const SanPham& sp) {
    return {{"id", sp.ma()}, {"maSP", sp.ma()}, {"ten", sp.ten()},
            {"sh", sp.maNguoiBan()}, {"maNguoiBan", sp.maNguoiBan()},
            {"gia", sp.donGia()}, {"donGia", sp.donGia()}, {"kg", sp.khoiLuong()},
            {"khoiLuong", sp.khoiLuong()}, {"ton", sp.tonKho()}, {"tonKho", sp.tonKho()},
            {"sold", sp.daBan()}, {"daBan", sp.daBan()}, {"rate", sp.danhGia()}, {"danhGia", sp.danhGia()}};
}

int chiSoTrangThai(const string& ten) {
    if (ten == tt::XAC_NHAN) return 1;
    if (ten == tt::DANG_GIAO) return 2;
    if (ten == tt::HOAN_TAT) return 3;
    if (ten == tt::TRA_HANG) return 4;
    return 0;
}

string tenNguoi(const AuthService& auth, const string& ma) {
    for (const auto& nd : auth.danhSach())
        if (nd && (nd->tenDangNhap() == ma || nd->ma() == ma)) return nd->hoTen();
    return ma;
}

json ketQuaJson(const KetQuaTinhTien& k) {
    return {{"tienHang", k.tienHang}, {"giamGia", k.giamGia}, {"phiShip", k.phiShip},
            {"phiXuLy", k.phiXuLy}, {"tongThanhToan", k.tongThanhToan},
            {"khoiLuong", k.khoiLuong}, {"ngayDuKien", k.ngayDuKien},
            {"hg", k.tienHang}, {"giam", k.giamGia}, {"ship", k.phiShip},
            {"phi", k.phiXuLy}, {"tong", k.tongThanhToan}, {"kg", k.khoiLuong}};
}

json donHangJson(const DonHang& d, const AuthService& auth, const KhoSanPham& kho) {
    json items = json::array();
    for (const auto& ct : d.chiTiet()) {
        string ten = ct->maSP();
        try { ten = kho.lay(ct->maSP()).ten(); } catch (const OrderException&) {}
        items.push_back({{"id", ct->maSP()}, {"maSP", ct->maSP()}, {"sh", ct->maNguoiBan()},
                         {"maNguoiBan", ct->maNguoiBan()}, {"ten", ten},
                         {"gia", ct->donGia()}, {"donGia", ct->donGia()}, {"kg", ct->khoiLuong()},
                         {"q", ct->soLuong()}, {"soLuong", ct->soLuong()}, {"thanhTien", ct->tinhThanhTien()}});
    }
    const auto k = d.tinhKetQua();
    json ls = json::array();
    for (const auto& x : d.lichSu()) ls.push_back({{"tt", x.trangThai}, {"trangThai", x.trangThai}, {"t", x.thoiDiem}, {"thoiDiem", x.thoiDiem}});
    json ag = json::array(), kn = json::array(), chat = json::array();
    for (const auto& a : d.anhGiaoHang()) ag.push_back({{"duongDan", a.duongDan}, {"moTa", a.moTa}, {"thoiDiem", a.thoiDiem}});
    for (const auto& a : d.anhKhieuNai()) kn.push_back({{"duongDan", a.duongDan}, {"moTa", a.moTa}, {"thoiDiem", a.thoiDiem}});
    for (const auto& m : d.tinNhan()) chat.push_back({{"maNguoiGui", m.maNguoiGui}, {"tenNguoiGui", tenNguoi(auth, m.maNguoiGui)}, {"vaiTro", m.vaiTro}, {"noiDung", m.noiDung}, {"thoiDiem", m.thoiDiem}});
    json tra = nullptr;
    if (d.yeuCauTra()) tra = {{"lyDo", d.yeuCauTra()->lyDo}, {"thoiDiem", d.yeuCauTra()->thoiDiem}, {"trangThai", static_cast<int>(d.yeuCauTra()->trangThai)}};
    json dc = {{"ten", d.diaChiGiao().hoTen}, {"hoTen", d.diaChiGiao().hoTen}, {"sdt", d.diaChiGiao().soDienThoai}, {"soDienThoai", d.diaChiGiao().soDienThoai}, {"diaChi", d.diaChiGiao().diaChi}};
    json hoan = json::object();
    for (const auto& x : d.daHoanTien()) hoan[x.first] = x.second;
    json diaChiLayHang = json::array();
    std::set<string> shopDaThem;
    for (const auto& ct : d.chiTiet()) {
        if (!shopDaThem.insert(ct->maNguoiBan()).second) continue;
        bool daThem = false;
        for (const auto& nguoi : auth.danhSach()) {
            if (auto* shop = dynamic_cast<NguoiBan*>(nguoi.get()); shop && shop->maShop() == ct->maNguoiBan()) {
                diaChiLayHang.push_back({{"maShop", shop->maShop()}, {"tenShop", shop->tenShop()}, {"hoTen", shop->diaChiLayHang().hoTen}, {"soDienThoai", shop->diaChiLayHang().soDienThoai}, {"diaChi", shop->diaChiLayHang().diaChi}});
                daThem = true;
                break;
            }
        }
        if (!daThem) diaChiLayHang.push_back({{"maShop", ct->maNguoiBan()}, {"diaChi", ""}});
    }
    return {{"id", d.maDon()}, {"maDon", d.maDon()}, {"kh", d.maKhach()}, {"tenKhach", tenNguoi(auth, d.maKhach())},
            {"h", d.hangVanChuyen()}, {"hangVanChuyen", d.hangVanChuyen()},
            {"p", d.thanhToan() ? d.thanhToan()->ten() : pttt::COD},
            {"ma", d.khuyenMai() ? d.khuyenMai()->ma() : ""},
            {"k", d.khoangCachKm()}, {"st", chiSoTrangThai(d.trangThai())}, {"trangThai", d.trangThai()},
            {"items", items}, {"chiTiet", items}, {"r", ketQuaJson(k)}, {"ketQua", ketQuaJson(k)},
            {"ls", ls}, {"lichSu", ls}, {"dc", dc}, {"diaChiGiao", diaChiJson(d.diaChiGiao())},
            {"tra", tra}, {"ht", hoan}, {"daHoanTien", hoan}, {"ag", ag}, {"anhGiaoHang", ag},
            {"kn", kn}, {"anhKhieuNai", kn}, {"chat", chat}, {"tinNhan", chat},
            {"diaChiLayHang", diaChiLayHang},
            {"kt", d.thoiDiemKetThuc()}, {"thoiDiemKetThuc", d.thoiDiemKetThuc()}, {"daKetThuc", d.daKetThuc()},
            {"sp", d.daTraPhiShip()}, {"thoiDiemDat", d.thoiDiemDat()}};
}

string bearer(const httplib::Request& req) {
    string h = req.get_header_value("Authorization");
    const string prefix = "Bearer ";
    return h.rfind(prefix, 0) == 0 ? h.substr(prefix.size()) : string();
}

json bodyJson(const httplib::Request& req) {
    if (req.body.empty()) return json::object();
    try {
        json j = json::parse(req.body);
        if (!j.is_object()) throw OrderException("Body JSON phải là object");
        return j;
    } catch (const json::exception& e) {
        throw OrderException(string("JSON không hợp lệ: ") + e.what());
    }
}

NguoiDung& xacThuc(AuthService& auth, const httplib::Request& req) {
    return auth.xacThuc(bearer(req));
}

void yeuCauQuanLyHoacChuShop(AuthService& auth, NguoiDung& nd, const SanPham* sp = nullptr) {
    if (nd.vaiTro() == vaitro::QUAN_LY) {
        auth.yeuCauQuyen(nd, quyen::QUAN_LY_SAN_PHAM);
        return;
    }
    if (nd.vaiTro() != vaitro::NGUOI_BAN) throw KhongCoQuyenException("Chỉ người bán hoặc quản lý được quản lý sản phẩm");
    auth.yeuCauQuyen(nd, quyen::SUA_SAN_PHAM_SHOP);
    if (sp && sp->maNguoiBan() != nd.maShop()) throw KhongCoQuyenException("Người bán chỉ được sửa sản phẩm của shop mình");
}

string maGio(const httplib::Request& req, DonHangService& orders) {
    string id = req.get_header_value("X-Gio-Hang");
    if (id.empty()) id = orders.taoMaGio();
    return id;
}

void setGioHeader(httplib::Response& res, const string& id) {
    res.set_header("X-Gio-Hang", id);
}

string param(const httplib::Request& req, const string& key) {
    return req.has_param(key) ? req.get_param_value(key) : string();
}

json thongKeJson(const vector<DonHang>& ds) {
    json doanhThu = ThongKeService::doanhThuTheoNguoiBan(ds);
    json demHang = ThongKeService::demTheoHang(ds);
    return {{"tyLeGiaoThanhCong", ThongKeService::tyLeGiaoThanhCong(ds)},
            {"tyLeHoanHang", ThongKeService::tyLeHoanHang(ds)},
            {"doanhThuTheoNguoiBan", doanhThu}, {"demTheoHang", demHang}};
}

}

int runHttpServer() {
    try {
        KhoSanPham kho(DataStore::docSanPham("data/san_pham.json"));
        AuthService auth("data/tai_khoan.json");
        DonHangService orders(kho, auth, "data/don_hang.json");
        httplib::Server svr;
        svr.set_payload_max_length(7 * 1024 * 1024);
        if (!svr.set_mount_point("/", "./web"))
            throw FileDataException("Không mount được thư mục web/");
        std::filesystem::create_directories("data/anh");
        if (!svr.set_mount_point("/anh", "./data/anh"))
            throw FileDataException("Unable to mount image directory data/anh/");
        svr.set_exception_handler([](const httplib::Request&, httplib::Response& res, std::exception_ptr ep) {
            try { if (ep) std::rethrow_exception(ep); }
            catch (const OrderException& e) { jsonResponse(res, {{"loi", e.loai()}, {"thongBao", e.what()}}, e.maHttp()); }
            catch (const json::exception& e) { jsonResponse(res, {{"loi", "OrderException"}, {"thongBao", e.what()}}, 400); }
            catch (const std::exception& e) { jsonResponse(res, {{"loi", "InternalServerError"}, {"thongBao", e.what()}}, 500); }
            catch (...) { jsonResponse(res, {{"loi", "InternalServerError"}, {"thongBao", "Lỗi không xác định"}}, 500); }
        });

        svr.Get("/api/san-pham", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex);
            string sort = param(req, "sapXep");
            if (sort == "phoBien") sort = "phoBien";
            auto found = kho.tim(param(req, "tuKhoa"), param(req, "maShop"), sort);
            json out = json::array(); for (const auto& p : found) out.push_back(sanPhamJson(p));
            jsonResponse(res, out);
        });

        svr.Post("/api/dang-nhap", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); auto j = bodyJson(req);
            string token = auth.dangNhap(j.value("user", j.value("tenDangNhap", "")), j.value("pass", j.value("matKhau", "")));
            NguoiDung& nd = auth.xacThuc(token);
            jsonResponse(res, {{"token", token}, {"nguoiDung", {{"ma", nd.ma()}, {"user", nd.tenDangNhap()}, {"tenDangNhap", nd.tenDangNhap()}, {"ten", nd.hoTen()}, {"hoTen", nd.hoTen()}, {"vaiTro", nd.vaiTro()}, {"role", nd.vaiTro()}, {"maShop", nd.maShop()}, {"maHang", nd.maHang()}, {"soDu", nd.vi() ? nd.vi()->soDu() : 0}}}});
        });
        svr.Post("/api/dang-xuat", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); auth.dangXuat(bearer(req)); jsonResponse(res, {{"ok", true}});
        });
        svr.Post("/api/dang-ky", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); auto j = bodyJson(req);
            string role = j.value("vaiTro", string(vaitro::KHACH_HANG));
            if (role == vaitro::NGUOI_BAN) auth.dangKyNguoiBan(j.value("hoTen", j.value("ten", "")), j.value("maShop", j.value("user", "")), j.value("matKhau", j.value("pass", "")));
            else if (role == vaitro::KHACH_HANG) auth.dangKyKhachHang(j.value("hoTen", j.value("ten", "")), j.value("user", ""), j.value("matKhau", j.value("pass", "")));
            else throw OrderException("Vai trò đăng ký không hợp lệ");
            jsonResponse(res, {{"ok", true}}, 201);
        });
        svr.Get("/api/toi", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req);
            jsonResponse(res, {{"ma", nd.ma()}, {"user", nd.tenDangNhap()}, {"tenDangNhap", nd.tenDangNhap()}, {"ten", nd.hoTen()}, {"hoTen", nd.hoTen()}, {"vaiTro", nd.vaiTro()}, {"role", nd.vaiTro()}, {"maShop", nd.maShop()}, {"maHang", nd.maHang()}, {"soDu", nd.vi() ? nd.vi()->soDu() : 0}});
        });
        svr.Get("/api/shop/dia-chi-lay-hang", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req);
            auto* shop = dynamic_cast<NguoiBan*>(&nd);
            if (!shop) throw KhongCoQuyenException("Chá»‰ ngÆ°á»i bÃ¡n Ä‘Æ°á»£c xem Ä‘á»‹a chá»‰ láº¥y hÃ ng");
            jsonResponse(res, {{"maShop", shop->maShop()}, {"diaChiLayHang", diaChiJson(shop->diaChiLayHang())}});
        });
        svr.Put("/api/shop/dia-chi-lay-hang", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req);
            auto* shop = dynamic_cast<NguoiBan*>(&nd);
            if (!shop) throw KhongCoQuyenException("Chá»‰ ngÆ°á»i bÃ¡n Ä‘Æ°á»£c cáº­p nháº­t Ä‘á»‹a chá»‰ láº¥y hÃ ng");
            auto j = bodyJson(req); const auto d = j.contains("diaChiLayHang") && j["diaChiLayHang"].is_object() ? j["diaChiLayHang"] : j;
            DiaChi dc{d.value("hoTen", d.value("ten", shop->hoTen())), d.value("soDienThoai", d.value("sdt", string())), d.value("diaChi", string())};
            if (dc.hoTen.empty() || !laSoDienThoaiHopLe(dc.soDienThoai) || dc.diaChi.size() < hangso::DIA_CHI_TOI_THIEU)
                throw OrderException("Nháº­p há» tÃªn, sá»‘ Ä‘iá»‡n thoáº¡i há»£p lá»‡ vÃ  Ä‘á»‹a chá»‰ láº¥y hÃ ng (tá»‘i thiá»ƒu 8 kÃ½ tá»±)");
            shop->datDiaChiLayHang(dc); auth.luu();
            jsonResponse(res, {{"maShop", shop->maShop()}, {"diaChiLayHang", diaChiJson(shop->diaChiLayHang())}});
        });

        svr.Post("/api/gio-hang/them", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); auto j = bodyJson(req); string id = maGio(req, orders);
            orders.gio(id).themSanPham(kho.lay(j.value("maSP", j.value("id", ""))), j.value("soLuong", j.value("q", 1)));
            jsonResponse(res, {{"ok", true}}); setGioHeader(res, id);
        });
        svr.Get("/api/gio-hang", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); string id = maGio(req, orders);
            json out = json::array(); for (const auto& d : orders.gio(id).dongHang()) {
                json p = sanPhamJson(d.first); p["q"] = d.second; p["soLuong"] = d.second; out.push_back(p);
            }
            jsonResponse(res, {{"items", out}, {"tongTien", orders.gio(id).tinhTongTien()}, {"tongKhoiLuong", orders.gio(id).tinhTongKhoiLuong()}}); setGioHeader(res, id);
        });
        svr.Put(R"(/api/gio-hang/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); string id = req.get_header_value("X-Gio-Hang");
            if (id.empty()) throw OrderException("Thiếu mã giỏ hàng trong header X-Gio-Hang");
            auto j = bodyJson(req); orders.gio(id).capNhatSoLuong(req.matches[1], j.value("soLuong", j.value("q", 0)));
            jsonResponse(res, {{"ok", true}}); setGioHeader(res, id);
        });
        svr.Delete(R"(/api/gio-hang/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); string id = req.get_header_value("X-Gio-Hang");
            if (id.empty()) throw OrderException("Thiếu mã giỏ hàng trong header X-Gio-Hang");
            orders.gio(id).xoaSanPham(req.matches[1]); jsonResponse(res, {{"ok", true}}); setGioHeader(res, id);
        });
        svr.Post("/api/gio-hang/khuyen-mai", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); auto j = bodyJson(req); string id = req.get_header_value("X-Gio-Hang");
            if (id.empty()) throw HetHangException("Giỏ hàng trống");
            string code = j.value("ma", j.value("maKhuyenMai", ""));
            if (!code.empty()) (void)taoKhuyenMai(code);
            jsonResponse(res, {{"ok", true}, {"maKhuyenMai", code}}); setGioHeader(res, id);
        });

        auto readCheckout = [&](const httplib::Request& req, bool withUser, httplib::Response& res, bool preview) {
            auto j = bodyJson(req); string cartId = req.get_header_value("X-Gio-Hang");
            if (cartId.empty()) throw HetHangException("Giỏ hàng trống");
            const string carrier = j.value("hang", j.value("hangVanChuyen", hang::GHN));
            const string payment = j.value("phuongThucThanhToan", j.value("pttt", pttt::COD));
            const string coupon = j.value("maKhuyenMai", j.value("ma", ""));
            const double km = j.value("khoangCachKm", j.value("km", 10.0));
            if (preview) { jsonResponse(res, ketQuaJson(orders.xemTruoc(orders.gio(cartId), carrier, payment, coupon, km))); return; }
            if (!withUser) throw DangNhapThatBaiException("Vui lòng đăng nhập để đặt hàng");
            NguoiDung& nd = xacThuc(auth, req);
            DiaChi dc; if (j.contains("diaChiGiao") && j["diaChiGiao"].is_object()) {
                const auto& x = j["diaChiGiao"]; dc = DiaChi{x.value("hoTen", x.value("ten", "")), x.value("soDienThoai", x.value("sdt", "")), x.value("diaChi", "")};
            }
            const DonHang& d = orders.taoDon(nd, orders.gio(cartId), dc, carrier, payment, coupon, km);
            jsonResponse(res, donHangJson(d, auth, kho), 201);
        };
        svr.Post("/api/don-hang/xem-truoc", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); readCheckout(req, false, res, true); });
        svr.Post("/api/don-hang", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); readCheckout(req, true, res, false); });

        svr.Get("/api/don-hang", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); json out = json::array();
            for (const DonHang* d : orders.layDanhSach(nd)) out.push_back(donHangJson(*d, auth, kho));
            jsonResponse(res, out);
        });
        svr.Get(R"(/api/don-hang/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); const auto& d = orders.layChiTiet(req.matches[1], xacThuc(auth, req)); jsonResponse(res, donHangJson(d, auth, kho));
        });
        svr.Post(R"(/api/don-hang/([^/]+)/chuyen-trang-thai)", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); orders.chuyenTrangThai(req.matches[1], nd); jsonResponse(res, donHangJson(orders.layChiTiet(req.matches[1], nd), auth, kho));
        });
        svr.Get("/api/thong-ke", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auth.yeuCauQuyen(nd, quyen::XEM_THONG_KE);
            json j = thongKeJson(orders.tatCaDon());
            if (nd.vaiTro() == vaitro::NGUOI_BAN) { json one = json::object(); string shop = nd.maShop(); if (j["doanhThuTheoNguoiBan"].contains(shop)) one[shop] = j["doanhThuTheoNguoiBan"][shop]; j["doanhThuTheoNguoiBan"] = one; }
            jsonResponse(res, j);
        });

        svr.Get("/api/vi", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auth.yeuCauQuyen(nd, quyen::DUNG_VI);
            if (!nd.vi()) throw KhongCoQuyenException("Tài khoản không có ví");
            json ls = json::array();
            for (const auto& gd : nd.vi()->lichSu()) ls.push_back({{"moTa", gd.moTa}, {"soTien", gd.soTien}, {"thoiDiem", gd.thoiDiem}});
            jsonResponse(res, {{"soDu", nd.vi()->soDu()}, {"lichSu", ls}, {"giaoDich", ls}});
        });
        svr.Get("/api/vi/giao-dich", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auth.yeuCauQuyen(nd, quyen::DUNG_VI); json ls = json::array();
            if (nd.vi()) for (const auto& gd : nd.vi()->lichSu()) ls.push_back({{"moTa", gd.moTa}, {"soTien", gd.soTien}, {"thoiDiem", gd.thoiDiem}});
            jsonResponse(res, ls);
        });
        svr.Post("/api/vi/nap-tien", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auth.yeuCauQuyen(nd, quyen::DUNG_VI); if (!nd.vi()) throw KhongCoQuyenException("Tài khoản không có ví");
            auto j = bodyJson(req); nd.vi()->napTien(j.value("soTien", 0.0), j.value("nguon", string("Console/Web"))); auth.luu(); jsonResponse(res, {{"soDu", nd.vi()->soDu()}});
        });

        svr.Get("/api/giao-hang", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auth.yeuCauQuyen(nd, quyen::GIAO_HANG); json out = json::array();
            for (const DonHang* d : orders.layDonGiaoHang(nd)) out.push_back(donHangJson(*d, auth, kho));
            jsonResponse(res, out);
        });

        svr.Post(R"(/api/don-hang/([^/]+)/ket-thuc)", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); orders.ketThuc(req.matches[1], nd); jsonResponse(res, donHangJson(orders.layChiTiet(req.matches[1], nd), auth, kho));
        });
        svr.Post(R"(/api/don-hang/([^/]+)/xac-nhan-giao)", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auto j = bodyJson(req); orders.xacNhanGiao(req.matches[1], nd, j.value("anh", string())); jsonResponse(res, donHangJson(orders.layChiTiet(req.matches[1], nd), auth, kho));
        });
        svr.Post(R"(/api/don-hang/([^/]+)/tra-hang)", [&](const httplib::Request& req, httplib::Response& res) {
            std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auto j = bodyJson(req); orders.traHang(req.matches[1], nd, j.value("lyDo", string())); jsonResponse(res, {{"ok", true}});
        });
        svr.Post(R"(/api/don-hang/([^/]+)/hoan-hang)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); orders.hoanHang(req.matches[1], nd); jsonResponse(res, {{"ok", true}}); });
        svr.Post(R"(/api/don-hang/([^/]+)/tu-choi-tra-hang)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); orders.tuChoiTraHang(req.matches[1], nd); jsonResponse(res, {{"ok", true}}); });
        svr.Post(R"(/api/don-hang/([^/]+)/hoan-tien)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auto j = bodyJson(req); orders.hoanTien(req.matches[1], nd, j.value("maShop", string())); jsonResponse(res, {{"ok", true}}); });
        svr.Post(R"(/api/don-hang/([^/]+)/nhan-don-tra)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); orders.nhanDonTra(req.matches[1], nd); jsonResponse(res, {{"ok", true}}); });
        svr.Post(R"(/api/don-hang/([^/]+)/xac-nhan-gui-ve-shop)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); orders.xacNhanGuiVeShop(req.matches[1], nd); jsonResponse(res, {{"ok", true}}); });
        svr.Post(R"(/api/don-hang/([^/]+)/anh-giao-hang)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auto j = bodyJson(req); orders.guiAnh(req.matches[1], nd, "giao", j.value("anh", string()), j.value("moTa", string())); jsonResponse(res, {{"ok", true}}); });
        svr.Post(R"(/api/don-hang/([^/]+)/anh-khieu-nai)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auto j = bodyJson(req); orders.guiAnh(req.matches[1], nd, "khieuNai", j.value("anh", string()), j.value("moTa", string())); jsonResponse(res, {{"ok", true}}); });
        svr.Get(R"(/api/don-hang/([^/]+)/tin-nhan)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auto ds = orders.layTinNhan(req.matches[1], nd); json out = json::array(); for (const auto& x : ds) out.push_back({{"maNguoiGui", x.maNguoiGui}, {"tenNguoiGui", tenNguoi(auth, x.maNguoiGui)}, {"vaiTro", x.vaiTro}, {"noiDung", x.noiDung}, {"thoiDiem", x.thoiDiem}}); jsonResponse(res, out); });
        svr.Post(R"(/api/don-hang/([^/]+)/tin-nhan)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auto j = bodyJson(req); orders.guiTinNhan(req.matches[1], nd, j.value("noiDung", string())); jsonResponse(res, {{"ok", true}}); });
        svr.Get("/api/tin-nhan", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); json out = json::array(); for (const auto* d : orders.layDanhSach(nd)) if (!d->tinNhan().empty()) out.push_back({{"maDon", d->maDon()}, {"soTinChuaDoc", d->soTinChuaDoc(nd)}, {"tinNhanCuoi", d->tinNhan().back().noiDung}}); jsonResponse(res, out); });

        svr.Post(R"(/api/quan-ly/khuyen-mai/([^/]+)/bat-tat)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auth.yeuCauQuyen(nd, quyen::QUAN_LY_KHUYEN_MAI); auto j = bodyJson(req); string code = req.matches[1]; bool bat = j.value("bat", !khuyenMaiDangBat(code)); datKhuyenMaiBat(code, bat); jsonResponse(res, {{"ma", code}, {"bat", bat}}); });
        svr.Get("/api/quan-ly/tai-khoan", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auth.yeuCauQuyen(nd, quyen::QUAN_LY_TAI_KHOAN); json out = json::array(); for (const auto& a : auth.danhSach()) if (a) out.push_back({{"ma", a->ma()}, {"user", a->tenDangNhap()}, {"tenDangNhap", a->tenDangNhap()}, {"hoTen", a->hoTen()}, {"vaiTro", a->vaiTro()}, {"maShop", a->maShop()}, {"maHang", a->maHang()}, {"biKhoa", a->biKhoa()}, {"soDu", a->vi() ? a->vi()->soDu() : 0}}); jsonResponse(res, out); });
        svr.Post(R"(/api/quan-ly/tai-khoan/([^/]+)/khoa)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auto j = bodyJson(req); auth.khoaTaiKhoan(nd, req.matches[1], j.value("khoa", true)); jsonResponse(res, {{"ok", true}}); });
        svr.Get(R"(/api/quan-ly/van-chuyen/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auth.yeuCauQuyen(nd, quyen::QUAN_LY_VAN_CHUYEN); auto& b = bangGiaShip(req.matches[1]); jsonResponse(res, {{"phiCoBan", b.phiCoBan}, {"donGiaKm", b.donGiaKm}, {"donGiaKg", b.donGiaKg}, {"ngayCoBan", b.ngayCoBan}, {"kmMoiNgay", b.kmMoiNgay}}); });
        svr.Put(R"(/api/quan-ly/van-chuyen/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auth.yeuCauQuyen(nd, quyen::QUAN_LY_VAN_CHUYEN); auto j = bodyJson(req); auto& b = bangGiaShip(req.matches[1]); double base = j.value("phiCoBan", b.phiCoBan), km = j.value("donGiaKm", b.donGiaKm), kg = j.value("donGiaKg", b.donGiaKg); if (base < 0 || km < 0 || kg < 0) throw OrderException("Bảng giá không được âm"); b.phiCoBan = base; b.donGiaKm = km; b.donGiaKg = kg; jsonResponse(res, {{"ok", true}}); });
        svr.Get("/api/quan-ly/khuyen-mai", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); auth.yeuCauQuyen(nd, quyen::QUAN_LY_KHUYEN_MAI); json out = json::array(); for (const auto& x : danhSachMaKhuyenMai()) out.push_back({{"ma", x}, {"bat", khuyenMaiDangBat(x)}}); jsonResponse(res, out); });

        svr.Get("/api/quan-ly/san-pham", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); yeuCauQuanLyHoacChuShop(auth, nd); string shop = nd.vaiTro() == vaitro::NGUOI_BAN ? nd.maShop() : param(req, "maShop"); json out = json::array(); for (const auto& p : kho.tim(param(req, "tuKhoa"), shop, param(req, "sapXep"))) out.push_back(sanPhamJson(p)); jsonResponse(res, out); });
        svr.Post("/api/quan-ly/san-pham", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); yeuCauQuanLyHoacChuShop(auth, nd); auto j = bodyJson(req); string ma = j.value("maSP", j.value("id", string())); if (ma.empty()) ma = "SP" + std::to_string(kho.tatCa().size() + 1); string shop = nd.vaiTro() == vaitro::NGUOI_BAN ? nd.maShop() : j.value("maNguoiBan", j.value("sh", string())); SanPham p(ma, j.value("ten", string()), shop, j.value("donGia", j.value("gia", 0.0)), j.value("khoiLuong", j.value("kg", 0.1)), j.value("tonKho", j.value("ton", 0)), j.value("daBan", j.value("sold", 0)), j.value("danhGia", j.value("rate", 0.0))); kho.them(std::move(p)); DataStore::luuSanPham(kho.tatCa(), "data/san_pham.json"); jsonResponse(res, sanPhamJson(kho.lay(ma)), 201); });
        svr.Put(R"(/api/quan-ly/san-pham/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); SanPham& p = kho.lay(req.matches[1]); yeuCauQuanLyHoacChuShop(auth, nd, &p); auto j = bodyJson(req); if (j.contains("ten")) p.datTen(j["ten"].get<string>()); if (j.contains("donGia") || j.contains("gia")) p.datDonGia(j.value("donGia", j.value("gia", p.donGia()))); if (j.contains("khoiLuong") || j.contains("kg")) p.datKhoiLuong(j.value("khoiLuong", j.value("kg", p.khoiLuong()))); if (j.contains("tonKho") || j.contains("ton")) p.datTonKho(j.value("tonKho", j.value("ton", p.tonKho()))); DataStore::luuSanPham(kho.tatCa(), "data/san_pham.json"); jsonResponse(res, sanPhamJson(p)); });
        svr.Delete(R"(/api/quan-ly/san-pham/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); const SanPham& p = kho.lay(req.matches[1]); yeuCauQuanLyHoacChuShop(auth, nd, &p); kho.xoa(req.matches[1]); DataStore::luuSanPham(kho.tatCa(), "data/san_pham.json"); jsonResponse(res, {{"ok", true}}); });
        svr.Post(R"(/api/quan-ly/san-pham/([^/]+)/nhap-kho)", [&](const httplib::Request& req, httplib::Response& res) { std::lock_guard<std::mutex> guard(apiMutex); NguoiDung& nd = xacThuc(auth, req); const SanPham& p = kho.lay(req.matches[1]); yeuCauQuanLyHoacChuShop(auth, nd, &p); auto j = bodyJson(req); kho.nhapKho(req.matches[1], j.value("soLuong", 0)); DataStore::luuSanPham(kho.tatCa(), "data/san_pham.json"); jsonResponse(res, sanPhamJson(kho.lay(req.matches[1]))); });

        std::cout << "ĐT08 HTTP server tại http://127.0.0.1:8080  (console: dt08.exe --console)\n";
        if (!svr.listen("127.0.0.1", 8080)) throw OrderException("Không mở được cổng 8080; có thể cổng đang được sử dụng");
        return 0;
    } catch (const OrderException& e) {
        std::cerr << e.loai() << ": " << e.what() << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Lỗi khởi động server: " << e.what() << '\n';
        return 1;
    }
}
