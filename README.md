# HangMan
🎮 GIỚI THIỆU GAME HANGMAN

Hangman là trò chơi đoán từ kinh điển, nơi bạn sẽ cứu một chàng trai tội nghiệp khỏi cái chết treo cổ bằng cách đoán đúng các chữ cái trong từ bí mật!  

📜 LUẬT CHƠI ĐƠN GIẢN: 
1. Mỗi lượt, bạn đoán 1 chữ cái (a-z).  
2. Nếu đúng: Chữ cái sẽ hiện ra trong từ cần đoán.  
3. Nếu sai: Hình người treo cổ sẽ dần hoàn thiện (và bạn mất 1 lượt!).  
4. Thắng khi đoán đúng toàn bộ từ trước khi bị treo cổ (sai tối đa 6 lần).  

🌟 ĐẶC ĐIỂM NỔI BẬT:
- Đồ họa sinh động: Hình ảnh minh họa từng bước treo cổ bằng SDL2.  
- Từ vựng đa dạng: Đọc từ file, hỗ trợ thêm từ tùy chỉnh.  
- Hiển thị trực quan:  
  - Từ cần đoán (ví dụ: `_ A N G M A N`).  
  - Các chữ đã đoán (ví dụ: `Đã đoán: A, B, C`).  
  - Số lượt sai còn lại.  

💡 AI NÊN CHƠI?  
- Người yêu thích game đơn giản nhưng kịch tính. 
- Lập trình viên muốn học SDL2 qua ví dụ thực tế.  

---

👉 Tùy chỉnh game: Bạn có thể dễ dàng thêm từ mới vào file `words.txt` hoặc chỉnh độ khó bằng cách sửa số lượt sai tối đa trong code!  

(Bản này dành riêng cho bạn - kết hợp SDL2 + console output)
