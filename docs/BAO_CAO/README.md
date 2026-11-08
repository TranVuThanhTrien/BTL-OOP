# Tài liệu báo cáo ĐT08

Các chương được viết bằng LaTeX, tiếng Việt có dấu. Biên dịch bằng XeLaTeX:

```sh
cd docs/BAO_CAO
 latexmk -xelatex main.tex
```

Nếu chưa có `latexmk`, chạy `xelatex main.tex` hai lần. Bổ sung tên thành viên, lớp, giảng viên và ngày nộp ở `main.tex` trước khi nộp. Nội dung kỹ thuật dựa trên mã nguồn hiện tại và `docs/KIEN_TRUC.md`; chương kiểm thử chỉ ghi kết quả đã quan sát trong môi trường hiện tại.
