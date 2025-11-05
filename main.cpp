// === BAO GỒM CÁC THƯ VIỆN (HEADER FILES) ===
// Các thư viện này cung cấp các công cụ/hàm/lớp mà chúng ta cần sử dụng

#include <SFML/Graphics.hpp> // Thư viện chính của SFML (Đồ họa, Cửa sổ, Sự kiện, Text, Font...)
#include <vector>            // Thư viện C++ để sử dụng std::vector (dùng để lưu thân con rắn)
#include <cstdlib>           // Thư viện C++ cho các hàm chung, bao gồm rand() (tạo số ngẫu nhiên)
#include <ctime>             // Thư viện C++ cho hàm time() (lấy thời gian làm "hạt giống" cho rand)
#include <iostream>          // Thư viện C++ cho việc nhập/xuất console (dùng cout, cerr để báo lỗi)
#include <optional>          // Thư viện C++ (từ C++17), cần cho hàm pollEvent của SFML 3.0

// === SỬ DỤNG NAMESPACE ===
// Giúp chúng ta gõ tên hàm/lớp ngắn gọn hơn
using namespace sf;   // Cho phép gõ "RenderWindow" thay vì "sf::RenderWindow"
using namespace std;  // Cho phép gõ "vector" thay vì "std::vector", "cout" thay vì "std::cout"

// === CÁC HẰNG SỐ CỦA GAME ===
// Các biến cố định, không thay đổi trong suốt quá trình chạy game
const int WIDTH = 20;     // Chiều rộng của khu vực chơi (tính bằng số ô)
const int HEIGHT = 20;    // Chiều cao của khu vực chơi (tính bằng số ô)
const int CELL_SIZE = 32; // Kích thước của mỗi ô vuông (tính bằng pixel)
const float SPEED = 0.1f; // Tốc độ di chuyển của rắn (giây). Số càng nhỏ, rắn đi càng nhanh.

// === ENUM (LIỆT KÊ) CHO CÁC HƯỚNG ===
// Định nghĩa các hằng số đại diện cho các hướng di chuyển
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN }; // STOP=0, LEFT=1, RIGHT=2,...

// === ĐỊNH NGHĨA LỚP (CLASS) SNAKEGAME ===
// Lớp là một "bản thiết kế" chứa tất cả dữ liệu (biến) và logic (hàm) của game
class SnakeGame {
private: // Các thành viên "private" chỉ có thể được truy cập từ bên trong lớp này
    
    // --- CÁC BIẾN THÀNH VIÊN (LƯU TRỮ TRẠNG THÁI GAME) ---
    RenderWindow window;     // Cửa sổ game chính
    vector<Vector2i> body; // Một vector (danh sách) lưu tọa độ (x,y) của TỪNG ĐỐT rắn
    Direction dir;           // Hướng di chuyển hiện tại của rắn
    int score;               // Điểm số của người chơi
    Vector2i food;           // Tọa độ (x,y) của mồi
    bool gameOver;           // Trạng thái game (true = đã thua, false = đang chơi)
    Clock gameClock;         // Đồng hồ SFML để tính toán thời gian cho mỗi khung hình (frame)
    float moveTimer;         // Biến đếm thời gian, dùng để so sánh với SPEED

    // --- CÁC BIẾN TEXTURE (LƯU TRỮ ẢNH TỪ FILE VÀO VRAM) ---
    Texture grassTexture;   // Ảnh nền cỏ
    Texture wallTexture;    // Ảnh tường
    Texture foodTexture;    // Ảnh mồi

    // 4 texture cho đầu rắn
    Texture texHeadUp, texHeadDown, texHeadLeft, texHeadRight;

    // 4 texture cho đuôi rắn
    Texture texTailUp, texTailDown, texTailLeft, texTailRight;

    // 2 texture cho thân thẳng (dọc và ngang)
    Texture texBodyVertical, texBodyHorizontal;

    // 4 texture cho các góc cua
    Texture texCornerUL, texCornerUR, texCornerDL, texCornerDR;

    // --- CÁC BIẾN FONT VÀ TEXT (ĐỂ HIỂN THỊ CHỮ) ---
    Font font;         // Biến lưu trữ file font chữ (ví dụ: arial.ttf)
    Text scoreText;    // Đối tượng Text để hiển thị điểm số và hướng dẫn
    Text gameOverText; // Đối tượng Text để hiển thị chữ "GAME OVER"

public: // Các thành viên "public" có thể được gọi từ bên ngoài lớp (ví dụ: từ hàm main)

    // --- HÀM KHỞI TẠO (CONSTRUCTOR) ---
    // Hàm này được gọi tự động MỘT LẦN khi đối tượng SnakeGame được tạo ra
    SnakeGame() :
        // Danh sách khởi tạo (cách tối ưu để gán giá trị ban đầu cho các biến)
        window(VideoMode({800, 640}), "Snake Game"), // Tạo cửa sổ (Nền game 640x640, thanh bên 160px)
        scoreText(font, "", 20),                     // Khởi tạo text điểm (dùng font, nội dung rỗng, cỡ chữ 20)
        gameOverText(font, "GAME OVER!\nPress R to restart", 30) // Khởi tạo text game over
    { // Bắt đầu thân hàm constructor
        
        loadTextures(); // Gọi hàm tải tất cả ảnh (định nghĩa ở dưới)

        if (!font.openFromFile("arial.ttf")) { // Thử tải file font
            cerr << "Failed to load font!" << endl; // Nếu thất bại, báo lỗi ra console
        }

        // Thiết lập cho text điểm
        scoreText.setFillColor(Color::White); // Đặt màu chữ là màu trắng
        scoreText.setPosition(Vector2f(660.0f, 20.0f)); // Đặt vị trí ở thanh bên phải

        // Thiết lập cho text game over
        gameOverText.setFillColor(Color::Red); // Đặt màu chữ là màu đỏ
        
        // Căn giữa text "Game Over" (cho SFML 3)
        // 1. Lấy hình chữ nhật bao quanh text (SFML 3 dùng sf::Rect)
        sf::Rect textRect = gameOverText.getLocalBounds(); 
        
        // 2. Đặt điểm gốc (origin) vào chính giữa của text
        // SFML 3: dùng {x, y} để tạo Vector2f
        // SFML 3: dùng .position.x/y và .size.x/y thay vì .left/.top và .width/.height
        gameOverText.setOrigin({textRect.position.x + textRect.size.x / 2.0f,
                                textRect.position.y + textRect.size.y / 2.0f});

        // 3. Đặt VỊ TRÍ của text (dựa trên điểm gốc mới) vào giữa màn hình game
        gameOverText.setPosition(Vector2f(WIDTH * CELL_SIZE / 2.0f, HEIGHT * CELL_SIZE / 2.0f));

        resetGame(); // Gọi hàm reset game để bắt đầu ván chơi đầu tiên
    } // Kết thúc hàm constructor

    // --- HÀM TẢI ẢNH (TEXTURE) ---
    void loadTextures() {
        cout << "=== LOADING TEXTURES ===" << endl; // In ra console để biết tiến trình
        
        // Nếu load file thất bại (!...), thì gọi hàm createDefaultTexture để tạo ảnh màu
        if (!grassTexture.loadFromFile("grass.png")) createDefaultTexture(grassTexture, Color(100, 200, 100));
        if (!wallTexture.loadFromFile("wall.png")) createDefaultTexture(wallTexture, Color(100, 100, 255));
        if (!foodTexture.loadFromFile("food.png")) createDefaultTexture(foodTexture, Color::Red);
        
        // Tải 4 file ĐẦU
        if (!texHeadUp.loadFromFile("snake_head_up.png")) createDefaultTexture(texHeadUp, Color::Green);
        if (!texHeadDown.loadFromFile("snake_head_down.png")) createDefaultTexture(texHeadDown, Color::Green);
        if (!texHeadLeft.loadFromFile("snake_head_left.png")) createDefaultTexture(texHeadLeft, Color::Green);
        if (!texHeadRight.loadFromFile("snake_head_right.png")) createDefaultTexture(texHeadRight, Color::Green);

        // Tải 4 file ĐUÔI
        if (!texTailUp.loadFromFile("snake_tail_up.png")) createDefaultTexture(texTailUp, Color(0,150,0));
        if (!texTailDown.loadFromFile("snake_tail_down.png")) createDefaultTexture(texTailDown, Color(0,150,0));
        if (!texTailLeft.loadFromFile("snake_tail_left.png")) createDefaultTexture(texTailLeft, Color(0,150,0));
        if (!texTailRight.loadFromFile("snake_tail_right.png")) createDefaultTexture(texTailRight, Color(0,150,0));

        // Tải 2 file THÂN THẲNG
        if (!texBodyVertical.loadFromFile("snake_body_vertical.png")) createDefaultTexture(texBodyVertical, Color(0,200,0));
        if (!texBodyHorizontal.loadFromFile("snake_body_horizontal.png")) createDefaultTexture(texBodyHorizontal, Color(0,200,0));

        // Tải 4 file GÓC CUA
        if (!texCornerUL.loadFromFile("snake_corner_ul.png")) createDefaultTexture(texCornerUL, Color(0,180,0));
        if (!texCornerUR.loadFromFile("snake_corner_ur.png")) createDefaultTexture(texCornerUR, Color(0,180,0));
        if (!texCornerDL.loadFromFile("snake_corner_dl.png")) createDefaultTexture(texCornerDL, Color(0,180,0));
        if (!texCornerDR.loadFromFile("snake_corner_dr.png")) createDefaultTexture(texCornerDR, Color(0,180,0));

        cout << "=== TEXTURE LOADING COMPLETE ===" << endl; // Báo đã tải xong
    } // Kết thúc hàm loadTextures

    // --- HÀM TẠO TEXTURE MẶC ĐỊNH ---
    // (Được gọi nếu không tìm thấy file ảnh)
    void createDefaultTexture(Texture& texture, Color color) {
        // 1. Tạo một "Image" (ảnh trong RAM) với kích thước và màu sắc cho trước
        Image image({CELL_SIZE, CELL_SIZE}, color); 
        // 2. Tải "Image" (RAM) đó vào "Texture" (VRAM)
        if (!texture.loadFromImage(image)) { 
            cerr << "Error creating default texture!" << endl; // Báo lỗi nếu thất bại
        }
    } // Kết thúc hàm createDefaultTexture

    // --- HÀM RESET GAME ---
    // Đặt lại game về trạng thái ban đầu
    void resetGame() {
        body.clear(); // Xóa sạch vector thân rắn cũ
        
        // Thêm 3 đốt rắn ban đầu (để có đủ đầu, thân, đuôi cho logic render)
        body.push_back(Vector2i(WIDTH / 2, HEIGHT / 2));     // Đốt 0 (Đầu) ở giữa
        body.push_back(Vector2i(WIDTH / 2 - 1, HEIGHT / 2)); // Đốt 1 (Thân) bên trái
        body.push_back(Vector2i(WIDTH / 2 - 2, HEIGHT / 2)); // Đốt 2 (Đuôi) bên trái
        
        dir = STOP;       // Dừng di chuyển
        score = 0;        // Reset điểm về 0
        gameOver = false; // Đặt trạng thái "đang chơi"
        moveTimer = 0;    // Reset bộ đếm thời gian
        
        spawnFood();      // Tạo mồi mới
        updateScoreText(); // Cập nhật text điểm (hiển thị "Score: 0" và hướng dẫn)
    } // Kết thúc hàm resetGame

    // --- HÀM TẠO MỒI ---
    void spawnFood() {
        do { // Bắt đầu vòng lặp do-while
            // 1. Tạo tọa độ x ngẫu nhiên (từ 1 đến WIDTH - 2, để không nằm trên tường)
            food.x = 1 + rand() % (WIDTH - 2); 
            // 2. Tạo tọa độ y ngẫu nhiên (từ 1 đến HEIGHT - 2)
            food.y = 1 + rand() % (HEIGHT - 2);
        } while (isPositionInSnake(food)); // 3. Lặp lại nếu vị trí mồi trùng với thân rắn
    } // Kết thúc hàm spawnFood

    // --- HÀM KIỂM TRA VA CHẠM VỚI RẮN ---
    bool isPositionInSnake(Vector2i pos) {
        // Vòng lặp "for-each": duyệt qua từng 'segment' (đốt rắn) trong vector 'body'
        for (auto segment : body) { 
            if (segment == pos) return true; // Nếu vị trí 'pos' trùng với đốt rắn -> trả về true
        }
        return false; // Nếu duyệt hết mà không trùng -> trả về false
    } // Kết thúc hàm isPositionInSnake

    // --- HÀM CẬP NHẬT TEXT ĐIỂM ---
    void updateScoreText() {
        // Đặt nội dung mới cho 'scoreText'
        scoreText.setString("Score: " + to_string(score) + // Chuyển điểm (int) thành chuỗi (string)
                          "\n\nControls:\nW - Up\nA - Left\nS - Down\nD - Right\nR - Restart\nESC - Exit");
    } // Kết thúc hàm updateScoreText

    // --- HÀM XỬ LÝ INPUT (SỰ KIỆN) ---
    void processInput() {
        // Vòng lặp while, xử lý TẤT CẢ sự kiện trong hàng đợi
        // SFML 3: pollEvent() trả về std::optional<Event>
        while (optional<Event> event = window.pollEvent()) {
            
            // Kiểm tra sự kiện "Đóng cửa sổ" (nhấn nút X)
            if (event->is<Event::Closed>()) { 
                window.close(); // Đóng cửa sổ
                return; // Thoát khỏi hàm processInput
            }

            // Kiểm tra xem sự kiện có phải là "Nhấn phím" (KeyPressed) không
            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                
                if (gameOver) { // Nếu game đã kết thúc
                    if (keyPressed->scancode == Keyboard::Scancode::R) { // Nếu nhấn phím R
                        resetGame(); // Chơi lại từ đầu
                    }
                } else { // Nếu game đang chạy
                    // Kiểm tra mã phím (scancode)
                    switch (keyPressed->scancode) { 
                        // Phím W hoặc Mũi tên lên
                        case Keyboard::Scancode::W:
                        case Keyboard::Scancode::Up:
                            if (dir != DOWN) dir = UP; // Không cho phép quay đầu 180 độ
                            break; // Thoát khỏi switch
                        // Phím S hoặc Mũi tên xuống
                        case Keyboard::Scancode::S:
                        case Keyboard::Scancode::Down:
                            if (dir != UP) dir = DOWN;
                            break;
                        // Phím A hoặc Mũi tên trái
                        case Keyboard::Scancode::A:
                        case Keyboard::Scancode::Left:
                            if (dir != RIGHT) dir = LEFT;
                            break;
                        // Phím D hoặc Mũi tên phải
                        case Keyboard::Scancode::D:
                        case Keyboard::Scancode::Right:
                            if (dir != LEFT) dir = RIGHT;
                            break;
                        // Phím Escape
                        case Keyboard::Scancode::Escape:
                            window.close(); // Thoát game
                            break;
                    } // Kết thúc switch
                } // Kết thúc else (game đang chạy)
            } // Kết thúc if (KeyPressed)
        } // Kết thúc while (pollEvent)
    } // Kết thúc hàm processInput

    // --- HÀM CẬP NHẬT LOGIC GAME (DI CHUYỂN, VA CHẠM) ---
    void update() {
        // Nếu game over hoặc rắn đang đứng yên (dir == STOP), không làm gì cả
        if (gameOver || dir == STOP) return; 

        // 1. Tính toán thời gian
        // Lấy thời gian trôi qua từ lần cuối gọi restart() và khởi động lại đồng hồ
        float deltaTime = gameClock.restart().asSeconds(); 
        moveTimer += deltaTime; // Cộng dồn thời gian vào bộ đếm

        // 2. Chỉ di chuyển khi đã đủ thời gian
        if (moveTimer >= SPEED) {
            moveTimer = 0; // Reset bộ đếm về 0
            Vector2i newHead = body[0]; // Lấy tọa độ đầu rắn hiện tại (đốt 0)

            // 3. Tính toán vị trí đầu rắn mới dựa trên hướng (dir)
            switch (dir) {
                case UP:    newHead.y--; break; // Lên: giảm y
                case DOWN:  newHead.y++; break; // Xuống: tăng y
                case LEFT:  newHead.x--; break; // Trái: giảm x
                case RIGHT: newHead.x++; break; // Phải: tăng x
            }

            // 4. Kiểm tra va chạm tường
            // (Tường là 0 và WIDTH/HEIGHT - 1)
            if (newHead.x <= 0 || newHead.x >= WIDTH - 1 || newHead.y <= 0 || newHead.y >= HEIGHT - 1) {
                gameOver = true; // Thua
                return; // Thoát hàm update ngay lập tức
            }

            // 5. Kiểm tra va chạm thân (tự cắn)
            for (size_t i = 1; i < body.size(); i++) { // Duyệt từ đốt 1 (thân, không duyệt đầu)
                if (newHead == body[i]) { // Nếu đầu mới trùng với 1 đốt thân
                    gameOver = true; // Thua
                    return; // Thoát hàm update
                }
            }

            // 6. Di chuyển
            // Thêm đầu mới (newHead) vào vị trí đầu tiên (0) của vector 'body'
            body.insert(body.begin(), newHead);

            // 7. Kiểm tra ăn mồi
            if (newHead == food) { // Nếu đầu mới trùng vị trí mồi
                score++; // Tăng điểm
                updateScoreText(); // Cập nhật text hiển thị điểm
                spawnFood(); // Tạo mồi mới
                // LƯU Ý: Không gọi body.pop_back() -> rắn sẽ dài ra 1 đốt
            } else { // Nếu không ăn mồi
                body.pop_back(); // Xóa đốt cuối cùng (đuôi) đi
                // -> Việc này làm cho rắn di chuyển mà không dài ra
            }
        } // Kết thúc if (moveTimer >= SPEED)
    } // Kết thúc hàm update

    // --- HÀM VẼ (RENDER) MỌI THỨ LÊN MÀN HÌNH ---
    // Được gọi 1 lần mỗi khung hình (frame)
    void render() {
        window.clear(Color(40, 40, 40)); // Xóa màn hình với màu xám tối (để chuẩn bị vẽ khung hình mới)

        // 1. VẼ NỀN, TƯỜNG, THỨC ĂN
        // Tạo các đối tượng Sprite (đại diện cho ảnh) từ các Texture (ảnh gốc)
        Sprite grassSprite(grassTexture);
        Sprite wallSprite(wallTexture);
        Sprite foodSprite(foodTexture);

        // Vẽ nền cỏ (lặp 2 vòng for để vẽ đầy 20x20 ô)
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
                // Đặt vị trí cho sprite cỏ
                grassSprite.setPosition(Vector2f(static_cast<float>(x * CELL_SIZE), static_cast<float>(y * CELL_SIZE)));
                window.draw(grassSprite); // Vẽ sprite cỏ lên cửa sổ
            }
        }
        // Vẽ tường trên và dưới
        for (int x = 0; x < WIDTH; x++) {
            wallSprite.setPosition(Vector2f(static_cast<float>(x * CELL_SIZE), 0.0f));
            window.draw(wallSprite);
            wallSprite.setPosition(Vector2f(static_cast<float>(x * CELL_SIZE), static_cast<float>((HEIGHT - 1) * CELL_SIZE)));
            window.draw(wallSprite);
        }
        // Vẽ tường trái và phải
        for (int y = 0; y < HEIGHT; y++) {
            wallSprite.setPosition(Vector2f(0.0f, static_cast<float>(y * CELL_SIZE)));
            window.draw(wallSprite);
            wallSprite.setPosition(Vector2f(static_cast<float>((WIDTH - 1) * CELL_SIZE), static_cast<float>(y * CELL_SIZE)));
            window.draw(wallSprite);
        }
        // Vẽ mồi
        foodSprite.setPosition(Vector2f(static_cast<float>(food.x * CELL_SIZE), static_cast<float>(food.y * CELL_SIZE)));
        window.draw(foodSprite);

        // 2. VẼ RẮN (LOGIC PHỨC TẠP)
        
        // SFML 3: Phải khởi tạo Sprite với 1 texture (lấy tạm texHeadRight, vì đằng nào cũng bị gán đè)
        Sprite snakeSegmentSprite(texHeadRight); 

        // Lặp qua TỪNG ĐỐT của thân rắn (biến 'i' là chỉ số của đốt)
        for (size_t i = 0; i < body.size(); i++) {
            // Lấy vị trí (pixel) để vẽ đốt rắn này (tọa độ ô * kích thước ô)
            Vector2f pos(static_cast<float>(body[i].x * CELL_SIZE),
                         static_cast<float>(body[i].y * CELL_SIZE));
            snakeSegmentSprite.setPosition(pos); // Đặt vị trí cho sprite

            // --- A. VẼ ĐẦU (nếu i == 0) ---
            if (i == 0) {
                if (dir == STOP) snakeSegmentSprite.setTexture(texHeadRight); // Mặc định khi dừng
                else if (dir == UP) snakeSegmentSprite.setTexture(texHeadUp);
                else if (dir == DOWN) snakeSegmentSprite.setTexture(texHeadDown);
                else if (dir == LEFT) snakeSegmentSprite.setTexture(texHeadLeft);
                else if (dir == RIGHT) snakeSegmentSprite.setTexture(texHeadRight);
            }
            // --- B. VẼ ĐUÔI (nếu i là đốt cuối cùng) ---
            else if (i == body.size() - 1) { 
                Vector2i prev = body[i - 1]; // Lấy đốt KẾ TRƯỚC đuôi (đốt thân)
                Vector2i curr = body[i];     // Đốt đuôi hiện tại

                // So sánh 2 đốt để biết đuôi đang hướng về đâu
                if (prev.y < curr.y) snakeSegmentSprite.setTexture(texTailUp);    // Đốt trước ở trên -> đuôi hướng lên
                else if (prev.y > curr.y) snakeSegmentSprite.setTexture(texTailDown);  // Đốt trước ở dưới -> đuôi hướng xuống
                else if (prev.x < curr.x) snakeSegmentSprite.setTexture(texTailLeft);  // Đốt trước ở trái -> đuôi hướng trái
                else if (prev.x > curr.x) snakeSegmentSprite.setTexture(texTailRight); // Đốt trước ở phải -> đuôi hướng phải
            }
            // --- C. VẼ THÂN (ở giữa) ---
            else {
                // Chúng ta cần biết vị trí của 3 đốt: trước - hiện tại - sau
                Vector2i prev = body[i - 1]; // Đốt phía trước (gần đầu)
                Vector2i curr = body[i];     // Đốt hiện tại đang vẽ
                Vector2i next = body[i + 1]; // Đốt phía sau (gần đuôi)

                // Tính toán vector chênh lệch (từ đốt trước/sau đến đốt hiện tại)
                int prev_dx = prev.x - curr.x; // Vd: 1 (đốt trước ở bên phải)
                int prev_dy = prev.y - curr.y; // Vd: -1 (đốt trước ở bên trên)
                int next_dx = next.x - curr.x; // Vd: -1 (đốt sau ở bên trái)
                int next_dy = next.y - curr.y; // Vd: 1 (đốt sau ở bên dưới)

                // Trường hợp 1: Thân thẳng
                if (prev_dx == next_dx) { // Cả 2 đốt đều ở bên trái/phải -> thẳng đứng
                    snakeSegmentSprite.setTexture(texBodyVertical);
                } else if (prev_dy == next_dy) { // Cả 2 đốt đều ở trên/dưới -> thẳng ngang
                    snakeSegmentSprite.setTexture(texBodyHorizontal);
                }
                
                // === TRƯỜNG HỢP 2: THÂN CONG (LOGIC ĐÃ SỬA CHUẨN) ===
                else {
                    // Logic Lên-Phải (Up-Right) -> Gán ảnh Xuống-Phải (DR)
                    // (Nối từ trên xuống HOẶC từ phải qua) VÀ (nối sang phải HOẶC nối lên trên)
                    if ((prev_dy == -1 && next_dx == 1) || (prev_dx == 1 && next_dy == -1)) {
                        snakeSegmentSprite.setTexture(texCornerDR); 
                    } 
                    // Logic Lên-Trái (Up-Left) -> Gán ảnh Xuống-Trái (DL)
                    else if ((prev_dy == -1 && next_dx == -1) || (prev_dx == -1 && next_dy == -1)) {
                        snakeSegmentSprite.setTexture(texCornerDL);
                    } 
                    // Logic Xuống-Phải (Down-Right) -> Gán ảnh Lên-Phải (UR)
                    else if ((prev_dy == 1 && next_dx == 1) || (prev_dx == 1 && next_dy == 1)) {
                        snakeSegmentSprite.setTexture(texCornerUR);
                    } 
                    // Logic Xuống-Trái (Down-Left) -> Gán ảnh Lên-Trái (UL)
                    else if ((prev_dy == 1 && next_dx == -1) || (prev_dx == -1 && next_dy == 1)) {
                        snakeSegmentSprite.setTexture(texCornerUL);
                    }
                } // Kết thúc else (thân cong)
            } // Kết thúc else (vẽ thân)

            window.draw(snakeSegmentSprite); // Vẽ đốt rắn (đầu, thân hoặc đuôi) đã chọn
        } // Kết thúc vòng lặp for (duyệt qua thân rắn)

        // 3. VẼ TEXT (VẼ SAU CÙNG ĐỂ NỔI LÊN TRÊN)
        window.draw(scoreText); // Vẽ điểm
        if (gameOver) { // Nếu thua
            window.draw(gameOverText); // Vẽ chữ "Game Over"
        }

        window.display(); // Hiển thị tất cả những gì đã vẽ lên cửa sổ
    } // Kết thúc hàm render

    // --- HÀM CHẠY (RUN) VÒNG LẶP GAME CHÍNH ---
    void run() {
        // Khởi tạo "hạt giống" (seed) cho hàm rand() dựa trên thời gian hiện tại
        // (Để mỗi lần chạy game, mồi sẽ ở vị trí ngẫu nhiên khác nhau)
        srand(static_cast<unsigned int>(time(nullptr)));

        while (window.isOpen()) { // Vòng lặp game (chạy liên tục khi cửa sổ còn mở)
            
            // 3 bước cơ bản của một vòng lặp game
            processInput(); // 1. Xử lý input (bàn phím)
            update();       // 2. Cập nhật logic (di chuyển, va chạm)
            render();       // 3. Vẽ lại màn hình
        } // Kết thúc vòng lặp game
    } // Kết thúc hàm run
}; // === KẾT THÚC ĐỊNH NGHĨA LỚP SNAKEGAME ===


// === HÀM MAIN (ĐIỂM BẮT ĐẦU CỦA CHƯƠNG TRÌNH) ===
int main() {
    SnakeGame game; // Tạo một đối tượng (instance) của lớp SnakeGame
    game.run();     // Gọi hàm run() của đối tượng đó để bắt đầu game
    
    return 0;       // Kết thúc chương trình
} // Kết thúc hàm main