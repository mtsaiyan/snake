// === BAO GỒM CÁC THƯ VIỆN ===
// Các thư viện này cung cấp các công cụ/hàm/lớp mà chúng ta cần sử dụng

// Thư viện chính của SFML (Đồ họa, Cửa sổ, Sự kiện, Text, Font...)
#include <SFML/Graphics.hpp> 
// Thư viện Âm thanh của SFML (cho Sound và Music)
#include <SFML/Audio.hpp>    
// Thư viện C++ để sử dụng std::vector (dùng để lưu thân con rắn)
#include <vector>
// Thư viện C++ cho các hàm chung, bao gồm rand() (tạo số ngẫu nhiên)
#include <cstdlib>
// Thư viện C++ cho hàm time() (lấy thời gian làm "hạt giống" cho rand)
#include <ctime>
// Thư viện C++ cho việc nhập/xuất console (dùng cout, cerr để báo lỗi)
#include <iostream>
// Thư viện C++ (từ C++17), cần cho hàm pollEvent của SFML 3.0
#include <optional>
// Thư viện để ĐỌC/GHI FILE (dùng cho highscore)
#include <fstream>
// Thư viện để xử lý chuỗi (nối chuỗi cho text highscore)
#include <sstream>
// Thư viện để dùng std::string
#include <string>

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
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN }; 

// === ENUM CHO TRẠNG THÁI GAME ===
// Dùng để quản lý xem game đang ở màn hình nào
enum GameState { MENU, PLAYING, GAME_OVER, HIGHSCORE_SCREEN };

// === ĐỊNH NGHĨA LỚP (CLASS) SNAKEGAME ===
// Lớp là một "bản thiết kế" chứa tất cả dữ liệu (biến) và logic (hàm) của game
class SnakeGame {
private: // Các thành viên "private" chỉ có thể được truy cập từ bên trong lớp này
    
    // --- CÁC BIẾN TRẠNG THÁI GAME ---
    RenderWindow window;     // Cửa sổ game chính
    vector<Vector2i> body; // Vector (danh sách) lưu tọa độ (x,y) của TỪNG ĐỐT rắn
    Direction dir;           // Hướng di chuyển hiện tại của rắn
    int score;               // Điểm số của người chơi
    Vector2i food;           // Tọa độ (x,y) của mồi
    bool gameOver;           // Cờ báo hiệu đã thua (true = đã thua, false = đang chơi)
    Clock gameClock;         // Đồng hồ SFML để tính toán thời gian cho mỗi khung hình (frame)
    float moveTimer;         // Biến đếm thời gian, dùng để so sánh với SPEED

    // --- CÁC BIẾN TEXTURE (ẢNH) ---
    // (Lưu dữ liệu ảnh trong VRAM)
    Texture grassTexture;    // Ảnh nền cỏ (bạn đang dùng 1x1 ô)
    Texture wallTexture;     // Ảnh tường (32x32)
    Texture foodTexture;     // Ảnh mồi (32x32)

    // 14 ảnh cho rắn (32x32)
    Texture texHeadUp, texHeadDown, texHeadLeft, texHeadRight;
    Texture texTailUp, texTailDown, texTailLeft, texTailRight;
    Texture texBodyVertical, texBodyHorizontal;
    Texture texCornerUL, texCornerUR, texCornerDL, texCornerDR;
    
    // --- CÁC BIẾN FONT VÀ TEXT (CHỮ) ---
    Font font;         // Biến lưu trữ file font chữ (ví dụ: jokerman.ttf)
    Text scoreText;    // Đối tượng Text để hiển thị điểm số (ở thanh bên phải)
    Text gameOverText; // Đối tượng Text để hiển thị chữ "GAME OVER"

    // --- CÁC BIẾN CHO MENU VÀ HIGHSCORE ---
    GameState gameState;       // Biến quan trọng, lưu trạng thái hiện tại (MENU, PLAYING,...)
    int highScore;             // Biến lưu điểm cao nhất
    Texture texMenuBackground; // Ảnh nền cho Menu (800x640)
    
    Text menuPlay;             // Text "1. NewGame"
    Text menuHighscore;        // Text "2. HighScore"
    Text highscoreDisplayText; // Text để hiển thị ở màn hình highscore 
    Texture texGameOverPanel;  // Texture cho bảng ảnh PNG lúc Game Over

    // --- CÁC BIẾN CHO ÂM THANH ---
    // SoundBuffer: Tải file âm thanh vào RAM
    SoundBuffer eatBuffer;
    SoundBuffer gameOverBuffer;
    SoundBuffer moveBuffer; 
    
    // Sound: Dùng để phát SoundBuffer (giống Sprite dùng Texture)
    Sound eatSound;
    Sound gameOverSound;
    Sound moveSound;
    
    // Music: Dùng để phát file nhạc dài (phát trực tiếp từ file)
    Music menuMusic;        
    
    // --- CÁC HÀM PRIVATE (HÀM NỘI BỘ) ---

    // Hàm tải Highscore từ file "highscore.txt"
    void loadHighScore() {
        ifstream file("highscore.txt"); // Mở file để đọc (ifstream = input file stream)
        if (file.is_open()) {           // Kiểm tra xem mở file thành công không
            file >> highScore;          // Đọc số từ file vào biến highScore
            file.close();               // Đóng file
        } else {
            highScore = 0; // Nếu không mở được (file chưa tồn tại), đặt điểm cao là 0
        }
    }

    // Hàm lưu Highscore (nếu điểm hiện tại cao hơn)
    void saveHighScore() {
        if (score > highScore) { // Nếu phá kỷ lục
            highScore = score;   // Cập nhật biến highScore
            
            ofstream file("highscore.txt"); // Mở file để ghi (ofstream = output file stream)
            if (file.is_open()) {
                file << highScore; // Ghi điểm mới vào file
                file.close();      // Đóng file
            }
        }
    }

public: // Các thành viên "public" có thể được gọi từ bên ngoài lớp (ví dụ: từ hàm main)

    // --- HÀM KHỞI TẠO (CONSTRUCTOR) ---
    // Hàm này được gọi tự động MỘT LẦN khi đối tượng SnakeGame được tạo ra
    SnakeGame() :
        // Danh sách khởi tạo (cách tối ưu để gán giá trị ban đầu cho các biến)
        window(VideoMode({800, 640}), "Snake Game"), 
        scoreText(font, "", 20),
        gameOverText(font, "GAME OVER!\nPress R to restart\nPress M for Menu", 30),
        menuPlay(font, "1. NewGame", 40),
        menuHighscore(font, "2. HighScore", 40),
        highscoreDisplayText(font, "", 30),
        
        // Sửa lỗi SFML 3: Khởi tạo Sound với Buffer (dù Buffer đang rỗng)
        eatSound(eatBuffer),
        gameOverSound(gameOverBuffer),
        moveSound(moveBuffer)
    {
        // --- Thân hàm Constructor (chạy sau danh sách khởi tạo) ---
        
        gameState = MENU; // Bắt đầu game ở trạng thái MENU
        loadHighScore();  // Tải điểm cao đã lưu
        loadTextures();   // Tải TẤT CẢ asset (ảnh + âm thanh)
        
        // === GÁN LẠI BUFFER (SAU KHI ĐÃ TẢI) ===
        // (Cách làm đúng để sửa lỗi "dừng hình" và lỗi "SFML 3")
        eatSound.setBuffer(eatBuffer);
        gameOverSound.setBuffer(gameOverBuffer);
        moveSound.setBuffer(moveBuffer);
        // =====================================
        
        // Cài đặt nhạc menu
        menuMusic.setVolume(50); // Đặt âm lượng (tùy chọn)
        
        // Tải file font chữ
        if (!font.openFromFile("jokerman.ttf")) { 
            cerr << "Failed to load font!" << endl;
        }

        // --- Cài đặt các Text ---
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(Vector2f(650.0f, 20.0f)); // Vị trí thanh bên

        // Căn giữa text "Game Over"
        gameOverText.setFillColor(Color::Red);
        sf::Rect textRect = gameOverText.getLocalBounds(); // Lấy kích thước
        // Đặt điểm gốc (origin) vào chính giữa text
        gameOverText.setOrigin({textRect.position.x + textRect.size.x / 2.0f,
                                textRect.position.y + textRect.size.y / 2.0f});
        // Đặt vị trí của text vào giữa màn hình game
        gameOverText.setPosition(Vector2f(WIDTH * CELL_SIZE / 2.0f, HEIGHT * CELL_SIZE / 2.0f));

        // Căn giữa các text của Menu
        float menuCenterX = (WIDTH * CELL_SIZE) / 2.0f; 

        menuPlay.setFillColor(Color::White);
        textRect = menuPlay.getLocalBounds();
        menuPlay.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
        menuPlay.setPosition(Vector2f(menuCenterX, 500.0f)); // Đặt ở dưới

        menuHighscore.setFillColor(Color::White);
        textRect = menuHighscore.getLocalBounds();
        menuHighscore.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
        menuHighscore.setPosition(Vector2f(menuCenterX, 560.0f)); // Đặt ở dưới

        // Cài đặt text cho màn hình Highscore
        highscoreDisplayText.setFillColor(Color::White);
        
        // Chuẩn bị cho ván game đầu tiên
        resetGame(); 
    }

    // --- HÀM TẢI ẢNH VÀ ÂM THANH ---
    void loadTextures() {
        cout << "=== LOADING ASSETS (Textures & Sounds) ===" << endl;
        
        // Tải ảnh nền menu (800x640)
        if (!texMenuBackground.loadFromFile("menu_background.png")) {
            Image img({800, 640}, Color(40, 40, 40)); 
            if (!texMenuBackground.loadFromImage(img)) {
                cerr << "Failed to create default menu background!" << endl;
            }
        }
        
        // Tải ảnh nền game (grass.png)
        if (!grassTexture.loadFromFile("grass.png")) createDefaultTexture(grassTexture, Color(100, 200, 100));
        
        // Tải tường, mồi
        if (!wallTexture.loadFromFile("wall.png")) createDefaultTexture(wallTexture, Color(100, 100, 255));
        if (!foodTexture.loadFromFile("food.png")) createDefaultTexture(foodTexture, Color::Red);
        
        // Tải bảng Game Over
        if (!texGameOverPanel.loadFromFile("game_over_panel.png")) {
            Image img({400, 250}, Color(0, 0, 0, 180)); // Đen mờ 
            if (!texGameOverPanel.loadFromImage(img)) {
                cerr << "Failed to create default game over panel!" << endl;
            }
        }
        
        // Tải 14 ảnh rắn
        if (!texHeadUp.loadFromFile("snake_head_up.png")) createDefaultTexture(texHeadUp, Color::Green);
        if (!texHeadDown.loadFromFile("snake_head_down.png")) createDefaultTexture(texHeadDown, Color::Green);
        if (!texHeadLeft.loadFromFile("snake_head_left.png")) createDefaultTexture(texHeadLeft, Color::Green);
        if (!texHeadRight.loadFromFile("snake_head_right.png")) createDefaultTexture(texHeadRight, Color::Green);
        if (!texTailUp.loadFromFile("snake_tail_up.png")) createDefaultTexture(texTailUp, Color(0,150,0));
        if (!texTailDown.loadFromFile("snake_tail_down.png")) createDefaultTexture(texTailDown, Color(0,150,0));
        if (!texTailLeft.loadFromFile("snake_tail_left.png")) createDefaultTexture(texTailLeft, Color(0,150,0));
        if (!texTailRight.loadFromFile("snake_tail_right.png")) createDefaultTexture(texTailRight, Color(0,150,0));
        if (!texBodyVertical.loadFromFile("snake_body_vertical.png")) createDefaultTexture(texBodyVertical, Color(0,200,0));
        if (!texBodyHorizontal.loadFromFile("snake_body_horizontal.png")) createDefaultTexture(texBodyHorizontal, Color(0,200,0));
        if (!texCornerUL.loadFromFile("snake_corner_ul.png")) createDefaultTexture(texCornerUL, Color(0,180,0));
        if (!texCornerUR.loadFromFile("snake_corner_ur.png")) createDefaultTexture(texCornerUR, Color(0,180,0));
        if (!texCornerDL.loadFromFile("snake_corner_dl.png")) createDefaultTexture(texCornerDL, Color(0,180,0));
        if (!texCornerDR.loadFromFile("snake_corner_dr.png")) createDefaultTexture(texCornerDR, Color(0,180,0));
        
        // Tải Sound Buffer (file âm thanh)
        if (!eatBuffer.loadFromFile("eat.wav")) {
            cerr << "Failed to load eat.wav!" << endl;
        }
        if (!gameOverBuffer.loadFromFile("game_over.wav")) {
            cerr << "Failed to load game_over.wav!" << endl;
        }
        if (!moveBuffer.loadFromFile("move.wav")) {
             cerr << "Failed to load move.wav!" << endl;
        }
        
        // Tải Music (nhạc nền)
        if (!menuMusic.openFromFile("menu_music.ogg")) { // .ogg tốt cho nhạc dài
             cerr << "Failed to load menu_music.ogg!" << endl;
        } else {
             // Sửa lỗi SFML 3: Đặt lặp lại (loop) bằng TimeSpan
             // Dùng ngoặc nhọn {} để khởi tạo
             menuMusic.setLoopPoints(Music::TimeSpan{Time::Zero, menuMusic.getDuration()});
        }
        
        cout << "=== ASSET LOADING COMPLETE ===" << endl;
    }

    // --- HÀM TẠO TEXTURE MẶC ĐỊNH (32x32) ---
    // (Dùng nếu không tải được ảnh 32x32)
    void createDefaultTexture(Texture& texture, Color color) {
        Image image({CELL_SIZE, CELL_SIZE}, color); // Tạo ảnh 32x32 (SFML 3)
        if (!texture.loadFromImage(image)) { 
            cerr << "Error creating default texture!" << endl;
        }
    } 

    // --- HÀM RESET GAME (CHƠI MỚI) ---
    // Đặt lại game về trạng thái ban đầu
    void resetGame() {
        body.clear(); // Xóa sạch vector thân rắn cũ
        
        // Thêm 3 đốt rắn ban đầu
        body.push_back(Vector2i(WIDTH / 2, HEIGHT / 2));
        body.push_back(Vector2i(WIDTH / 2 - 1, HEIGHT / 2));
        body.push_back(Vector2i(WIDTH / 2 - 2, HEIGHT / 2));
        
        dir = STOP;       // Dừng di chuyển
        score = 0;        // Reset điểm về 0
        gameOver = false; // Đặt trạng thái "đang chơi"
        moveTimer = 0;    // Reset bộ đếm thời gian
        
        spawnFood();      // Tạo mồi mới
        updateScoreText(); // Cập nhật text điểm (Score: 0)
    } 

    // --- HÀM TẠO MỒI ---
    void spawnFood() {
        do { // Bắt đầu vòng lặp
            // Tạo tọa độ x,y ngẫu nhiên (từ 1 đến 18)
            food.x = 1 + rand() % (WIDTH - 2); 
            food.y = 1 + rand() % (HEIGHT - 2); 
        } while (isPositionInSnake(food)); // Lặp lại nếu vị trí mồi trùng với thân rắn
    }

    // --- HÀM KIỂM TRA VA CHẠM VỚI RẮN ---
    bool isPositionInSnake(Vector2i pos) {
        for (auto segment : body) { // Duyệt qua từng đốt rắn
            if (segment == pos) return true; // Nếu trùng -> trả về true
        }
        return false; // Nếu không -> trả về false
    }

    // --- HÀM CẬP NHẬT TEXT ĐIỂM (BÊN PHẢI) ---
    void updateScoreText() {
        // Nối chuỗi (string) và gán cho scoreText
        scoreText.setString("Score: " + to_string(score) + 
                          "\nHigh Score: " + to_string(highScore) + // Thêm Highscore
                          "\n\nControls:\nW - Up\nA - Left\nS - Down\nD - Right\n\nESC - Menu"); 
    } 

    // --- HÀM XỬ LÝ INPUT (SỰ KIỆN) ---
    void processInput() {
        // Lấy các sự kiện (nhấn phím, đóng cửa sổ...)
        while (optional<Event> event = window.pollEvent()) {
            
            // 1. Xử lý đóng cửa sổ (nhấn nút X)
            if (event->is<Event::Closed>()) {
                window.close(); // Đóng
                return;         // Thoát hàm
            }

            // 2. Chỉ xử lý khi nhấn phím (KeyPressed)
            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                
                // 3. Phân luồng xử lý tùy theo trạng thái game (gameState)
                switch (gameState) {
                    
                    // --- INPUT KHI Ở MENU ---
                    case MENU:
                        if (keyPressed->scancode == Keyboard::Scancode::Num1) { // Nhấn phím '1'
                            gameState = PLAYING; // Chuyển sang trạng thái chơi
                            resetGame();         // Reset game
                        } 
                        else if (keyPressed->scancode == Keyboard::Scancode::Num2) { // Nhấn phím '2'
                            gameState = HIGHSCORE_SCREEN; // Chuyển sang màn hình điểm
                            
                            // Cập nhật nội dung text highscore
                            stringstream ss; // Dùng stringstream để nối chuỗi
                            ss << "High Score: " << highScore;
                            ss << "\n\n\nPress M or ESC to return to Menu";
                            highscoreDisplayText.setString(ss.str());
                            
                            // Căn giữa text này (vì nội dung vừa thay đổi)
                            sf::Rect textRect = highscoreDisplayText.getLocalBounds();
                            highscoreDisplayText.setOrigin({textRect.position.x + textRect.size.x / 2.0f,
                                                            textRect.position.y + textRect.size.y / 2.0f});
                            float menuCenterX = (WIDTH * CELL_SIZE) / 2.0f; // 320.0f
                            highscoreDisplayText.setPosition(Vector2f(menuCenterX, 530.0f));
                        }
                        break; // Hết case MENU

                    // --- INPUT KHI ĐANG CHƠI ---
                    case PLAYING:
                        if (gameOver) break; // Nếu thua thì không nhận phím di chuyển nữa
                        
                        switch (keyPressed->scancode) {
                            // 4 phím di chuyển
                            case Keyboard::Scancode::W: case Keyboard::Scancode::Up:
                                if (dir != DOWN) dir = UP; break;
                            case Keyboard::Scancode::S: case Keyboard::Scancode::Down:
                                if (dir != UP) dir = DOWN; break;
                            case Keyboard::Scancode::A: case Keyboard::Scancode::Left:
                                if (dir != RIGHT) dir = LEFT; break;
                            case Keyboard::Scancode::D: case Keyboard::Scancode::Right:
                                if (dir != LEFT) dir = RIGHT; break;
                            // Phím ESC
                            case Keyboard::Scancode::Escape:
                                gameState = MENU; // Quay về Menu
                                saveHighScore();  // Lưu điểm (nếu cao)
                                break;
                        }
                        break; // Hết case PLAYING

                    // --- INPUT KHI GAME OVER ---
                    case GAME_OVER:
                        if (keyPressed->scancode == Keyboard::Scancode::R) { // Nhấn R
                            gameState = PLAYING; // Chơi lại
                            resetGame();
                        }
                        if (keyPressed->scancode == Keyboard::Scancode::M || keyPressed->scancode == Keyboard::Scancode::Escape) { // Nhấn M hoặc ESC
                            gameState = MENU; // Về Menu
                        }
                        break; // Hết case GAME_OVER

                    // --- INPUT KHI XEM HIGHSCORE ---
                    case HIGHSCORE_SCREEN:
                        if (keyPressed->scancode == Keyboard::Scancode::M || keyPressed->scancode == Keyboard::Scancode::Escape) { // Nhấn M hoặc ESC
                            gameState = MENU; // Về Menu
                        }
                        break; // Hết case HIGHSCORE_SCREEN
                
                } // Kết thúc switch(gameState)
            } // Kết thúc if(KeyPressed)
        } // Kết thúc while(pollEvent)
    } // Kết thúc hàm processInput

    // --- HÀM CẬP NHẬT LOGIC GAME ---
    // (Chạy liên tục)
    void update() {
        
        // --- QUẢN LÝ NHẠC NỀN ---
        // (Đây là logic sửa lỗi nhạc không phát)
        // 1. Nếu đang ở MENU hoặc HIGHSCORE
        if (gameState == MENU || gameState == HIGHSCORE_SCREEN) {
            // 2. Và nếu nhạc đang không phát...
            // (SFML 3: Dùng 'SoundSource::Status::Playing')
            if (menuMusic.getStatus() != SoundSource::Status::Playing) {
                menuMusic.play(); // 3. ...Thì phát nhạc
            }
        }
        // 4. Ngược lại (nếu đang ở PLAYING hoặc GAME_OVER)
        else {
            // 5. Và nếu nhạc đang phát...
            if (menuMusic.getStatus() == SoundSource::Status::Playing) {
                menuMusic.stop(); // 6. ...Thì dừng nhạc
            }
        }
        
        // --- LOGIC DI CHUYỂN CỦA RẮN ---
        
        // Chỉ chạy logic di chuyển rắn khi đang ở trạng thái PLAYING
        if (gameState != PLAYING) return;

        // Nếu game thua (gameOver) hoặc rắn đang đứng yên (STOP), không làm gì cả
        if (gameOver || dir == STOP) return; 

        // Tính toán thời gian
        float deltaTime = gameClock.restart().asSeconds(); 
        moveTimer += deltaTime; // Cộng dồn thời gian

        // Chỉ di chuyển khi đủ thời gian (theo SPEED)
        if (moveTimer >= SPEED) {
            moveTimer = 0; // Reset giờ
            Vector2i newHead = body[0]; // Lấy đầu rắn

            // Tính tọa độ đầu mới dựa trên hướng
            switch (dir) {
                case UP:    newHead.y--; break;
                case DOWN:  newHead.y++; break;
                case LEFT:  newHead.x--; break;
                case RIGHT: newHead.x++; break;
            }
            
            // --- XỬ LÝ VA CHẠM (CÓ ÂM THANH) ---
            
            // Kiểm tra va chạm tường
            if (newHead.x <= 0 || newHead.x >= WIDTH - 1 || newHead.y <= 0 || newHead.y >= HEIGHT - 1) {
                gameOver = true; gameState = GAME_OVER; saveHighScore();
                gameOverSound.play(); // <-- Phát âm thanh thua
                return;
            }
            // Kiểm tra va chạm thân
            for (size_t i = 1; i < body.size(); i++) {
                if (newHead == body[i]) {
                    gameOver = true; gameState = GAME_OVER; saveHighScore();
                    gameOverSound.play(); // <-- Phát âm thanh thua
                    return;
                }
            }
            
            // Phát âm thanh di chuyển (nếu rắn không đứng yên)
            /*if (dir != STOP) {
                moveSound.play();
            }*/

            // Di chuyển: Thêm đầu mới
            body.insert(body.begin(), newHead);

            // Kiểm tra ăn mồi (có âm thanh)
            if (newHead == food) {
                score++; updateScoreText(); spawnFood();
                eatSound.play(); // <-- Phát âm thanh ăn
            } else {
                body.pop_back(); // Xóa đuôi (nếu không ăn)
            }
        } // Kết thúc if (moveTimer)
    } // Kết thúc hàm update

    // --- HÀM VẼ (RENDER) ---
    // (Vẽ mọi thứ lên màn hình)
    void render() {
        window.clear(Color(40, 40, 40)); // Luôn xóa màn hình bằng màu xám

        // 1. Phân luồng VẼ tùy theo trạng thái game
        switch (gameState) {
            
            // --- VẼ KHI Ở MENU ---
            case MENU:
            { 
                // Vẽ ảnh nền menu (lớp dưới cùng)
                Sprite menuBgSprite(texMenuBackground);
                menuBgSprite.setPosition({0.0f, 0.0f}); // SFML 3 (dùng Vector2f)
                window.draw(menuBgSprite);

                // Vẽ các text menu (lớp trên)
                window.draw(menuPlay);
                window.draw(menuHighscore);
            } 
            break; // Hết case MENU

            // --- VẼ KHI ĐANG CHƠI HOẶC GAME OVER ---
            case PLAYING:
            case GAME_OVER:
            { 
                // 2. VẼ NỀN, TƯỜNG, MỒI
                Sprite grassSprite(grassTexture);
                Sprite wallSprite(wallTexture);
                Sprite foodSprite(foodTexture);

                // Vòng lặp 1x1 của bạn (vẽ 1 ô cỏ)
                for (int x = 0; x < 1; x++) { 
                    for (int y = 0; y < 1; y++) {
                        grassSprite.setPosition(Vector2f(static_cast<float>(x * CELL_SIZE), static_cast<float>(y * CELL_SIZE)));
                        window.draw(grassSprite);
                    }
                }
                
                // Vẽ 4 cạnh tường
                for (int x = 0; x < WIDTH; x++) {
                    wallSprite.setPosition(Vector2f(static_cast<float>(x * CELL_SIZE), 0.0f));
                    window.draw(wallSprite);
                    wallSprite.setPosition(Vector2f(static_cast<float>(x * CELL_SIZE), static_cast<float>((HEIGHT - 1) * CELL_SIZE)));
                    window.draw(wallSprite);
                }
                for (int y = 0; y < HEIGHT; y++) {
                    wallSprite.setPosition(Vector2f(0.0f, static_cast<float>(y * CELL_SIZE)));
                    window.draw(wallSprite);
                    wallSprite.setPosition(Vector2f(static_cast<float>((WIDTH - 1) * CELL_SIZE), static_cast<float>(y * CELL_SIZE)));
                    window.draw(wallSprite);
                }
                // Vẽ mồi
                foodSprite.setPosition(Vector2f(static_cast<float>(food.x * CELL_SIZE), static_cast<float>(food.y * CELL_SIZE)));
                window.draw(foodSprite);

                // 3. VẼ RẮN
                Sprite snakeSegmentSprite(texHeadRight); // Khởi tạo (SFML 3)
                
                // Lặp qua từng đốt rắn
                for (size_t i = 0; i < body.size(); i++) {
                    Vector2f pos(static_cast<float>(body[i].x * CELL_SIZE),
                                 static_cast<float>(body[i].y * CELL_SIZE));
                    snakeSegmentSprite.setPosition(pos); // Đặt vị trí

                    // A. VẼ ĐẦU
                    if (i == 0) {
                        if (dir == STOP) snakeSegmentSprite.setTexture(texHeadRight);
                        else if (dir == UP) snakeSegmentSprite.setTexture(texHeadUp);
                        else if (dir == DOWN) snakeSegmentSprite.setTexture(texHeadDown);
                        else if (dir == LEFT) snakeSegmentSprite.setTexture(texHeadLeft);
                        else if (dir == RIGHT) snakeSegmentSprite.setTexture(texHeadRight);
                    }
                    // B. VẼ ĐUÔI
                    else if (i == body.size() - 1) { 
                        Vector2i prev = body[i - 1]; Vector2i curr = body[i];
                        if (prev.y < curr.y) snakeSegmentSprite.setTexture(texTailUp);
                        else if (prev.y > curr.y) snakeSegmentSprite.setTexture(texTailDown);
                        else if (prev.x < curr.x) snakeSegmentSprite.setTexture(texTailLeft);
                        else if (prev.x > curr.x) snakeSegmentSprite.setTexture(texTailRight);
                    }
                    // C. VẼ THÂN (THẲNG HOẶC CONG)
                    else {
                        Vector2i prev = body[i - 1]; Vector2i curr = body[i]; Vector2i next = body[i + 1];
                        int prev_dx = prev.x - curr.x; int prev_dy = prev.y - curr.y;
                        int next_dx = next.x - curr.x; int next_dy = next.y - curr.y;
                        if (prev_dx == next_dx) snakeSegmentSprite.setTexture(texBodyVertical);
                        else if (prev_dy == next_dy) snakeSegmentSprite.setTexture(texBodyHorizontal);
                        else {
                            if ((prev_dy == -1 && next_dx == 1) || (prev_dx == 1 && next_dy == -1)) snakeSegmentSprite.setTexture(texCornerDR); 
                            else if ((prev_dy == -1 && next_dx == -1) || (prev_dx == -1 && next_dy == -1)) snakeSegmentSprite.setTexture(texCornerDL);
                            else if ((prev_dy == 1 && next_dx == 1) || (prev_dx == 1 && next_dy == 1)) snakeSegmentSprite.setTexture(texCornerUR);
                            else if ((prev_dy == 1 && next_dx == -1) || (prev_dx == -1 && next_dy == 1)) snakeSegmentSprite.setTexture(texCornerUL);
                        }
                    }
                    window.draw(snakeSegmentSprite); // Vẽ đốt đã chọn
                } 

                // 4. VẼ TEXT (trên cùng)
                window.draw(scoreText); // Vẽ text điểm bên phải
                
                // 5. VẼ BẢNG GAME OVER (nếu thua)
                if (gameOver) { 
                    Sprite panelSprite(texGameOverPanel); // Tạo sprite cục bộ
                    sf::Rect panelRect = panelSprite.getLocalBounds();
                    // Căn giữa bảng
                    panelSprite.setOrigin({panelRect.position.x + panelRect.size.x / 2.0f,
                                           panelRect.position.y + panelRect.size.y / 2.0f});
                    panelSprite.setPosition(Vector2f(WIDTH * CELL_SIZE / 2.0f, HEIGHT * CELL_SIZE / 2.0f));
                    
                    window.draw(panelSprite); // Vẽ bảng (bên dưới)
                    window.draw(gameOverText);  // Vẽ text (đè lên trên)
                }
            } 
            break; // Hết case PLAYING/GAME_OVER

            // --- VẼ KHI Ở MÀN HÌNH HIGHSCORE ---
            case HIGHSCORE_SCREEN:
            { 
                // Vẽ nền menu
                Sprite menuBgSprite(texMenuBackground);
                menuBgSprite.setPosition({0.0f, 0.0f}); // SFML 3
                window.draw(menuBgSprite);
                
                // Vẽ text điểm cao
                window.draw(highscoreDisplayText); 
            } 
            break; // Hết case HIGHSCORE_SCREEN

        } // Kết thúc switch(gameState)

        window.display(); // Hiển thị mọi thứ lên cửa sổ
    } // Kết thúc hàm render

    // --- HÀM CHẠY (RUN) VÒNG LẶP GAME CHÍNH ---
    void run() {
        // Khởi tạo "hạt giống" (seed) cho hàm rand()
        srand(static_cast<unsigned int>(time(nullptr)));

        // Vòng lặp chính của game
        while (window.isOpen()) {
            processInput(); // 1. Xử lý phím
            update();       // 2. Cập nhật logic
            render();       // 3. Vẽ lại màn hình
        } // Kết thúc vòng lặp
    } // Kết thúc hàm run
}; // === KẾT THÚC ĐỊNH NGHĨA LỚP SNAKEGAME ===


// === HÀM MAIN (ĐIỂM BẮT ĐẦU CỦA CHƯƠNG TRÌNH) ===
int main() {
    SnakeGame game; // Tạo một đối tượng (instance) của lớp SnakeGame
    game.run();     // Gọi hàm run() của đối tượng đó để bắt đầu game
    
    return 0;       // Kết thúc chương trình
} // Kết thúc hàm main