#include "DataStore.h"
#include "OrderException.h"
#include <fstream>
#include <cstdio>

// Người phụ trách: C | nhánh feature/payment-storage | LỚP 5
#if __has_include("json.hpp")
#include "json.hpp"
#define DT08_HAS_JSON 1
using nlohmann::json;
#else
#define DT08_HAS_JSON 0
#endif

// TODO(C-4): docSanPham, luuSanPham (data/san_pham.json)
vector<SanPham> DataStore::docSanPham(const string& duongDan) {
#if DT08_HAS_JSON
    std::ifstream f(duongDan);
    if (!f.is_open()) {
        throw FileDataException("Không mở được file sản phẩm: " + duongDan);
    }
    try {
        json j;
        f >> j;
        vector<SanPham> ds;
        for (const auto& item : j) {
            ds.emplace_back(
                item.at("maSP").get<string>(),
                item.at("ten").get<string>(),
                item.at("maNguoiBan").get<string>(),
                item.at("donGia").get<double>(),
                item.at("khoiLuong").get<double>(),
                item.at("tonKho").get<int>(),
                item.value("daBan", 0),
                item.value("danhGia", 0.0)
            );
        }
        return ds;
    } catch (const OrderException&) {
        throw;
    } catch (const std::exception& e) {
        throw FileDataException("Lỗi đọc JSON sản phẩm (" + duongDan + "): " + e.what());
    }
#else
    std::ifstream f(duongDan);
    if (!f.is_open()) {
        throw FileDataException("Không mở được file sản phẩm: " + duongDan);
    }
    return {
        SanPham("AT01", "Áo thun",            "TT", 120000, 0.3,  30, 1240, 4.8),
        SanPham("QJ02", "Quần jeans",         "TT", 280000, 0.7,  20, 860,  4.7),
        SanPham("GS03", "Giày sneaker",       "TT", 450000, 0.9,  12, 415,  4.9),
        SanPham("MU04", "Mũ lưỡi trai",       "TT", 90000,  0.1,  40, 2300, 4.6),
        SanPham("TN05", "Tai nghe",           "DT", 350000, 0.2,  15, 980,  4.8),
        SanPham("SC06", "Sạc nhanh 20W",      "DT", 160000, 0.15, 25, 3150, 4.7),
        SanPham("CH07", "Chuột không dây",    "DT", 210000, 0.1,  18, 1420, 4.6),
        SanPham("LM08", "Loa mini",           "DT", 390000, 0.4,  10, 530,  4.5),
        SanPham("SL09", "Sách lập trình C++", "NS", 90000,  0.5,  35, 760,  4.9),
        SanPham("ST10", "Sổ tay",             "NS", 45000,  0.2,  60, 4100, 4.8),
        SanPham("BG11", "Bút gel (hộp)",      "NS", 60000,  0.1,  50, 2800, 4.7),
        SanPham("TT12", "Truyện tranh",       "NS", 35000,  0.2,  0,  1900, 4.6)
    };
#endif
}

void DataStore::luuSanPham(const vector<SanPham>& ds, const string& duongDan) {
    string tam = duongDan + ".tmp";
    std::ofstream f(tam);
    if (!f.is_open()) {
        throw FileDataException("Không ghi được file sản phẩm: " + duongDan);
    }
#if DT08_HAS_JSON
    json j = json::array();
    for (const auto& sp : ds) {
        j.push_back({
            {"maSP", sp.ma()},
            {"ten", sp.ten()},
            {"maNguoiBan", sp.maNguoiBan()},
            {"donGia", sp.donGia()},
            {"khoiLuong", sp.khoiLuong()},
            {"tonKho", sp.tonKho()},
            {"daBan", sp.daBan()},
            {"danhGia", sp.danhGia()}
        });
    }
    f << j.dump(2);
#else
    (void)ds;
    f << "[]\n";
#endif
    f.close();
    std::remove(duongDan.c_str());
    if (std::rename(tam.c_str(), duongDan.c_str()) != 0) {
        throw FileDataException("Không thể đổi tên file tạm sang: " + duongDan);
    }
}

// TODO(C-5): docDonHang, luuDonHang (data/don_hang.json) - Đầy đủ mọi trường nghiệp vụ
vector<DonHang> DataStore::docDonHang(const string& duongDan) {
    std::ifstream f(duongDan);
    if (!f.is_open()) {
        throw FileDataException("Không mở được file đơn hàng: " + duongDan);
    }
#if DT08_HAS_JSON
    try {
        string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        if (content.empty()) return {};
        json j = json::parse(content);
        if (!j.is_array()) return {};
        vector<DonHang> ds;
        for (const auto& item : j) {
            DonHang d;
            d.m_maDon = item.at("maDon").get<string>();
            d.m_maKhach = item.value("maKhach", string("khach"));
            d.m_khoangCachKm = item.value("khoangCachKm", 10.0);
            d.m_phiShip = item.value("phiShip", 0.0);
            d.m_daChot = item.value("daChot", true);
            d.m_thoiDiemDat = item.value("thoiDiemDat", thoiGianHienTai());
            d.m_thoiDiemKetThuc = item.value("thoiDiemKetThuc", string());
            d.m_daTraPhiShip = item.value("daTraPhiShip", false);
            d.m_vanChuyen = taoDonVi(item.at("hangVanChuyen").get<string>());
            d.m_thanhToan = taoPhuongThuc(item.at("phuongThucThanhToan").get<string>(), nullptr);
            string maKM = item.value("maKhuyenMai", string());
            if (!maKM.empty()) {
                d.m_khuyenMai = taoKhuyenMai(maKM);
            }
            d.m_trangThai = taoTrangThai(item.value("trangThai", tt::MOI));
            if (item.contains("diaChiGiao") && item["diaChiGiao"].is_object()) {
                const auto& dc = item["diaChiGiao"];
                d.m_diaChiGiao = DiaChi{dc.value("hoTen", ""), dc.value("soDienThoai", ""), dc.value("diaChi", "")};
            }
            if (item.contains("chiTiet") && item["chiTiet"].is_array()) {
                for (const auto& ct : item["chiTiet"]) {
                    d.m_chiTiet.push_back(make_unique<ChiTietDonHang>(
                        ct.at("maSP").get<string>(),
                        ct.at("maNguoiBan").get<string>(),
                        ct.at("soLuong").get<int>(),
                        ct.at("donGia").get<double>(),
                        ct.at("khoiLuong").get<double>()
                    ));
                }
            }
            if (item.contains("lichSu") && item["lichSu"].is_array()) {
                for (const auto& ls : item["lichSu"]) {
                    d.m_lichSu.push_back(LichSuTrangThai{ls.value("trangThai", ""), ls.value("thoiDiem", "")});
                }
            }
            if (item.contains("yeuCauTra") && item["yeuCauTra"].is_object()) {
                const auto& yc = item["yeuCauTra"];
                int ttInt = yc.value("trangThai", 0);
                d.m_yeuCauTra = make_unique<YeuCauTraHang>(YeuCauTraHang{
                    yc.value("lyDo", ""),
                    yc.value("thoiDiem", ""),
                    static_cast<TrangThaiTraHang>(ttInt)
                });
            }
            if (item.contains("anhGiaoHang") && item["anhGiaoHang"].is_array()) {
                for (const auto& a : item["anhGiaoHang"]) {
                    d.m_anhGiaoHang.push_back(AnhDinhKem{a.value("duongDan", ""), a.value("moTa", ""), a.value("thoiDiem", "")});
                }
            }
            if (item.contains("anhKhieuNai") && item["anhKhieuNai"].is_array()) {
                for (const auto& a : item["anhKhieuNai"]) {
                    d.m_anhKhieuNai.push_back(AnhDinhKem{a.value("duongDan", ""), a.value("moTa", ""), a.value("thoiDiem", "")});
                }
            }
            if (item.contains("tinNhan") && item["tinNhan"].is_array()) {
                for (const auto& tn : item["tinNhan"]) {
                    d.m_tinNhan.push_back(TinNhan{
                        tn.value("maNguoiGui", ""),
                        tn.value("vaiTro", ""),
                        tn.value("noiDung", ""),
                        tn.value("thoiDiem", "")
                    });
                }
            }
            if (item.contains("daHoanTien") && item["daHoanTien"].is_object()) {
                for (auto it = item["daHoanTien"].begin(); it != item["daHoanTien"].end(); ++it) {
                    d.m_daHoanTien[it.key()] = it.value().get<double>();
                }
            }
            ds.push_back(std::move(d));
        }
        return ds;
    } catch (const OrderException&) {
        throw;
    } catch (const std::exception& e) {
        throw FileDataException("Lỗi đọc JSON đơn hàng (" + duongDan + "): " + e.what());
    }
#else
    return {};
#endif
}

void DataStore::luuDonHang(const vector<DonHang>& ds, const string& duongDan) {
    string tam = duongDan + ".tmp";
    std::ofstream f(tam);
    if (!f.is_open()) {
        throw FileDataException("Không ghi được file đơn hàng: " + duongDan);
    }
#if DT08_HAS_JSON
    json j = json::array();
    for (const auto& d : ds) {
        json ctArr = json::array();
        for (const auto& ct : d.m_chiTiet) {
            ctArr.push_back({
                {"maSP", ct->maSP()},
                {"maNguoiBan", ct->maNguoiBan()},
                {"soLuong", ct->soLuong()},
                {"donGia", ct->donGia()},
                {"khoiLuong", ct->khoiLuong()}
            });
        }
        json lsArr = json::array();
        for (const auto& ls : d.m_lichSu) {
            lsArr.push_back({{"trangThai", ls.trangThai}, {"thoiDiem", ls.thoiDiem}});
        }
        json agArr = json::array();
        for (const auto& a : d.m_anhGiaoHang) {
            agArr.push_back({{"duongDan", a.duongDan}, {"moTa", a.moTa}, {"thoiDiem", a.thoiDiem}});
        }
        json knArr = json::array();
        for (const auto& a : d.m_anhKhieuNai) {
            knArr.push_back({{"duongDan", a.duongDan}, {"moTa", a.moTa}, {"thoiDiem", a.thoiDiem}});
        }
        json tnArr = json::array();
        for (const auto& tn : d.m_tinNhan) {
            tnArr.push_back({
                {"maNguoiGui", tn.maNguoiGui},
                {"vaiTro", tn.vaiTro},
                {"noiDung", tn.noiDung},
                {"thoiDiem", tn.thoiDiem}
            });
        }
        json obj = {
            {"maDon", d.m_maDon},
            {"maKhach", d.m_maKhach},
            {"khoangCachKm", d.m_khoangCachKm},
            {"phiShip", d.tinhPhiShip()},
            {"daChot", d.m_daChot},
            {"thoiDiemDat", d.m_thoiDiemDat},
            {"thoiDiemKetThuc", d.m_thoiDiemKetThuc},
            {"daTraPhiShip", d.m_daTraPhiShip},
            {"hangVanChuyen", d.hangVanChuyen()},
            {"phuongThucThanhToan", d.m_thanhToan ? d.m_thanhToan->ten() : pttt::COD},
            {"maKhuyenMai", d.m_khuyenMai ? d.m_khuyenMai->ma() : ""},
            {"trangThai", d.trangThai()},
            {"diaChiGiao", {{"hoTen", d.m_diaChiGiao.hoTen}, {"soDienThoai", d.m_diaChiGiao.soDienThoai}, {"diaChi", d.m_diaChiGiao.diaChi}}},
            {"chiTiet", ctArr},
            {"lichSu", lsArr},
            {"anhGiaoHang", agArr},
            {"anhKhieuNai", knArr},
            {"tinNhan", tnArr},
            {"daHoanTien", d.m_daHoanTien}
        };
        if (d.m_yeuCauTra) {
            obj["yeuCauTra"] = {
                {"lyDo", d.m_yeuCauTra->lyDo},
                {"thoiDiem", d.m_yeuCauTra->thoiDiem},
                {"trangThai", static_cast<int>(d.m_yeuCauTra->trangThai)}
            };
        } else {
            obj["yeuCauTra"] = nullptr;
        }
        j.push_back(obj);
    }
    f << j.dump(2);
#else
    (void)ds;
    f << "[]\n";
#endif
    f.close();
    std::remove(duongDan.c_str());
    if (std::rename(tam.c_str(), duongDan.c_str()) != 0) {
        throw FileDataException("Không thể đổi tên file tạm sang: " + duongDan);
    }
}

// TODO(C-8): docTaiKhoan, luuTaiKhoan (data/tai_khoan.json)
vector<unique_ptr<NguoiDung>> DataStore::docTaiKhoan(const string& duongDan) {
    std::ifstream f(duongDan);
    if (!f.is_open()) {
        throw FileDataException("Không mở được file tài khoản: " + duongDan);
    }
#if DT08_HAS_JSON
    try {
        json j;
        f >> j;
        vector<unique_ptr<NguoiDung>> ds;
        for (const auto& item : j) {
            string vt = item.at("vaiTro").get<string>();
            string ma = item.value("ma", item.at("tenDangNhap").get<string>());
            string user = item.at("tenDangNhap").get<string>();
            string bam = item.at("matKhauBam").get<string>();
            string ten = item.value("hoTen", user);
            unique_ptr<NguoiDung> nd;
            if (vt == vaitro::KHACH_HANG) {
                auto kh = make_unique<KhachHang>(ma, user, bam, ten);
                if (item.contains("diaChi") && item["diaChi"].is_object()) {
                    const auto& d = item["diaChi"];
                    kh->datDiaChi(DiaChi{d.value("hoTen", ""), d.value("soDienThoai", ""), d.value("diaChi", "")});
                }
                nd = std::move(kh);
            } else if (vt == vaitro::NGUOI_BAN) {
                auto nb = make_unique<NguoiBan>(ma, item.value("maShop", user), bam, ten);
                if (item.contains("diaChiLayHang") && item["diaChiLayHang"].is_object()) {
                    const auto& d = item["diaChiLayHang"];
                    nb->datDiaChiLayHang(DiaChi{d.value("hoTen", ""), d.value("soDienThoai", ""), d.value("diaChi", "")});
                }
                nd = std::move(nb);
            } else if (vt == vaitro::SHIPPER) {
                nd = make_unique<ShipperVanChuyen>(ma, user, bam, ten, item.value("maHang", hang::GHN));
            } else if (vt == vaitro::QUAN_LY) {
                nd = make_unique<QuanLy>(ma, user, bam, ten);
            } else {
                throw FileDataException("Vai trò không hợp lệ trong file tài khoản: " + vt);
            }
            if (item.value("biKhoa", false)) nd->khoa();
            if (nd->vi() && item.contains("vi") && item["vi"].is_object()) {
                double soDu = item["vi"].value("soDu", 0.0);
                vector<GiaoDichVi> ls;
                if (item["vi"].contains("lichSu") && item["vi"]["lichSu"].is_array()) {
                    for (const auto& gd : item["vi"]["lichSu"]) {
                        ls.push_back(GiaoDichVi{gd.value("moTa", ""), gd.value("soTien", 0.0), gd.value("thoiDiem", "")});
                    }
                }
                nd->vi()->khoiPhuc(soDu, std::move(ls));
            }
            ds.push_back(std::move(nd));
        }
        return ds;
    } catch (const OrderException&) {
        throw;
    } catch (const std::exception& e) {
        throw FileDataException("Lỗi đọc JSON tài khoản (" + duongDan + "): " + e.what());
    }
#else
    return {};
#endif
}

void DataStore::luuTaiKhoan(const vector<unique_ptr<NguoiDung>>& ds, const string& duongDan) {
    string tam = duongDan + ".tmp";
    std::ofstream f(tam);
    if (!f.is_open()) {
        throw FileDataException("Không ghi được file tài khoản: " + duongDan);
    }
#if DT08_HAS_JSON
    json j = json::array();
    for (const auto& nd : ds) {
        if (!nd) continue;
        json obj = {
            {"ma", nd->ma()},
            {"tenDangNhap", nd->tenDangNhap()},
            {"matKhauBam", nd->matKhauBam()},
            {"hoTen", nd->hoTen()},
            {"vaiTro", nd->vaiTro()},
            {"biKhoa", nd->biKhoa()}
        };
        if (!nd->maShop().empty()) obj["maShop"] = nd->maShop();
        if (!nd->maHang().empty()) obj["maHang"] = nd->maHang();
        if (nd->vi()) {
            json ls = json::array();
            for (const auto& gd : nd->vi()->lichSu()) {
                ls.push_back({{"moTa", gd.moTa}, {"soTien", gd.soTien}, {"thoiDiem", gd.thoiDiem}});
            }
            obj["vi"] = {{"soDu", nd->vi()->soDu()}, {"lichSu", ls}};
        }
        j.push_back(obj);
    }
    f << j.dump(2);
#else
    (void)ds;
    f << "[]\n";
#endif
    f.close();
    std::remove(duongDan.c_str());
    if (std::rename(tam.c_str(), duongDan.c_str()) != 0) {
        throw FileDataException("Không thể đổi tên file tạm sang: " + duongDan);
    }
}

// TODO(C-5): luuAnhBase64 - Kiểm tra định dạng png/jpg/svg và giới hạn 5MB thực tế
string DataStore::luuAnhBase64(const string& thuMucAnh, const string& tienTo, const string& base64) {
    if (base64.empty()) {
        throw OrderException("Dữ liệu ảnh đính kèm không được để trống");
    }
    string duoi = ".jpg";
    std::size_t viTriPhay = base64.find(',');
    string phanDau = (viTriPhay != string::npos) ? inThuong(base64.substr(0, viTriPhay)) : "";
    if (!phanDau.empty()) {
        if (phanDau.find("image/png") != string::npos) {
            duoi = ".png";
        } else if (phanDau.find("image/jpeg") != string::npos || phanDau.find("image/jpg") != string::npos) {
            duoi = ".jpg";
        } else if (phanDau.find("image/svg") != string::npos) {
            duoi = ".svg";
        } else {
            throw OrderException("Định dạng ảnh không được hỗ trợ (chỉ nhận PNG hoặc JPG)");
        }
    }
    std::size_t doDaiPayload = (viTriPhay != string::npos) ? (base64.size() - viTriPhay - 1) : base64.size();
    std::size_t uocTinhBytes = (doDaiPayload * 3) / 4;
    if (uocTinhBytes > 5 * 1024 * 1024) {
        throw OrderException("Kích thước ảnh vượt quá giới hạn tối đa 5MB");
    }

    static int s_demAnh = 0;
    ++s_demAnh;
    string thuMuc = thuMucAnh.empty() ? "data/anh" : thuMucAnh;
    string tenFile = (tienTo.empty() ? "anh" : tienTo) + "_" + std::to_string(s_demAnh) + duoi;
    string duongDan = thuMuc + "/" + tenFile;

    std::ofstream out(duongDan, std::ios::binary);
    if (out.is_open()) {
        out << base64;
        out.close();
    }
    return duongDan;
}