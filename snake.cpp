// Include các thư viện cần thiết
#include <SFML/Graphics.hpp>  
#include <vector>             
#include <cstdlib>            
#include <ctime>              
#include <iostream>           
#include <optional>           

using namespace sf;  
using namespace std; 

// Hằng số định nghĩa kích thước game
const int WIDTH = 20;        // Chiều rộng bảng game (số ô)
const int HEIGHT = 20;       // Chiều cao bảng game (số ô)
const int CELL_SIZE = 32;    // Kích thước mỗi ô (pixel)
const float SPEED = 0.1f;    // Tốc độ di chuyển của rắn (giây)

// Enum định nghĩa các hướng di chuyển
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

// Lớp chính của game Rắn săn mồi
class SnakeGame {
private:
    // Biến thành viên
    RenderWindow window;      // Cửa sổ game
    vector<Vector2i> body;    // Vector lưu các phần thân rắn (tọa độ x,y)
    vector<Direction> bodyDirections; // Lưu hướng của từng phần thân
    Direction dir;            // Hướng di chuyển hiện tại
    int score;                // Điểm số
    Vector2i food;            // Vị trí thức ăn
    bool gameOver;            // Trạng thái game kết thúc
    Clock gameClock;          // Đồng hồ tính thời gian
    float moveTimer;          // Bộ đếm thời gian cho di chuyển
    
    // Texture cho hình ảnh
    Texture grassTexture;     // Texture cho nền cỏ
    Texture wallTexture;      // Texture cho tường
    Texture foodTexture;      // Texture cho thức ăn
    
    // Texture cho rắn - đầy đủ các hướng
    Texture snakeHeadUpTexture;    // Đầu rắn hướng lên
    Texture snakeHeadDownTexture;  // Đầu rắn hướng xuống
    Texture snakeHeadLeftTexture;  // Đầu rắn hướng trái
    Texture snakeHeadRightTexture; // Đầu rắn hướng phải
    
    Texture snakeBodyVerticalTexture;   // Thân rắn thẳng đứng
    Texture snakeBodyHorizontalTexture; // Thân rắn thẳng ngang
    
    Texture snakeTailUpTexture;    // Đuôi rắn hướng lên
    Texture snakeTailDownTexture;  // Đuôi rắn hướng xuống
    Texture snakeTailLeftTexture;  // Đuôi rắn hướng trái
    Texture snakeTailRightTexture; // Đuôi rắn hướng phải
    
    Texture snakeCornerURTexture;  // Khúc cua Up-Right (⬏)
    Texture snakeCornerULTexture;  // Khúc cua Up-Left (⬑)
    Texture snakeCornerDRTexture;  // Khúc cua Down-Right (⬎)
    Texture snakeCornerDLTexture;  // Khúc cua Down-Left (⬐)
    
    // Biến cho text hiển thị
    Font font;                // Font chữ
    Text scoreText;           // Text hiển thị điểm số
    Text gameOverText;        // Text hiển thị khi game over

public:
    // Constructor - khởi tạo game
    SnakeGame() : 
        window(VideoMode({800, 640}), "Snake Game"),
        scoreText(font, "", 20),                      
        gameOverText(font, "GAME OVER!\nPress R to restart", 30)  
    {
        // Tải hình ảnh
        loadTextures();
        
        // Khởi tạo font
        if (!font.openFromFile("arial.ttf")) {  
            cerr << "Failed to load font!" << endl;  
        }
        
        // Thiết lập text
        scoreText.setFillColor(Color::White);        
        scoreText.setPosition(Vector2f(660.0f, 20.0f));  
        
        gameOverText.setFillColor(Color::Red);       
        gameOverText.setPosition(Vector2f(200.0f, 250.0f));  
        
        resetGame();  
    }

    // Hàm tải texture từ file ảnh
    void loadTextures() {
        cout << "=== LOADING TEXTURES ===" << endl;
        
        // Tải texture nền cỏ
        if (!grassTexture.loadFromFile("grass.png")) {
            cout << "✗ FAILED to load grass.png" << endl;
        } else {
            cout << "✓ grass.png loaded successfully" << endl;
        }
        
        // Tải texture tường
        if (!wallTexture.loadFromFile("wall.png")) {
            cout << "✗ FAILED to load wall.png" << endl;
        } else {
            cout << "✓ wall.png loaded successfully" << endl;
        }
        
        // Tải texture thức ăn
        if (!foodTexture.loadFromFile("food.png")) {
            cout << "✗ FAILED to load food.png" << endl;
        } else {
            cout << "✓ food.png loaded successfully" << endl;
        }
        
        // TẢI TEXTURE CHO RẮN - ĐẦY ĐỦ 4 HƯỚNG
        
        // Đầu rắn - 4 hướng
        if (!snakeHeadUpTexture.loadFromFile("snake_head_up.png")) {
            cout << "✗ FAILED to load snake_head_up.png" << endl;
        } else {
            cout << "✓ snake_head_up.png loaded successfully" << endl;
        }
        
        if (!snakeHeadDownTexture.loadFromFile("snake_head_down.png")) {
            cout << "✗ FAILED to load snake_head_down.png" << endl;
        } else {
            cout << "✓ snake_head_down.png loaded successfully" << endl;
        }
        
        if (!snakeHeadLeftTexture.loadFromFile("snake_head_left.png")) {
            cout << "✗ FAILED to load snake_head_left.png" << endl;
        } else {
            cout << "✓ snake_head_left.png loaded successfully" << endl;
        }
        
        if (!snakeHeadRightTexture.loadFromFile("snake_head_right.png")) {
            cout << "✗ FAILED to load snake_head_right.png" << endl;
        } else {
            cout << "✓ snake_head_right.png loaded successfully" << endl;
        }
        
        // Thân rắn - 2 hướng (dọc và ngang)
        if (!snakeBodyVerticalTexture.loadFromFile("snake_body_vertical.png")) {
            cout << "✗ FAILED to load snake_body_vertical.png" << endl;
        } else {
            cout << "✓ snake_body_vertical.png loaded successfully" << endl;
        }
        
        if (!snakeBodyHorizontalTexture.loadFromFile("snake_body_horizontal.png")) {
            cout << "✗ FAILED to load snake_body_horizontal.png" << endl;
        } else {
            cout << "✓ snake_body_horizontal.png loaded successfully" << endl;
        }
        
        // Đuôi rắn - 4 hướng
        if (!snakeTailUpTexture.loadFromFile("snake_tail_up.png")) {
            cout << "✗ FAILED to load snake_tail_up.png" << endl;
        } else {
            cout << "✓ snake_tail_up.png loaded successfully" << endl;
        }
        
        if (!snakeTailDownTexture.loadFromFile("snake_tail_down.png")) {
            cout << "✗ FAILED to load snake_tail_down.png" << endl;
        } else {
            cout << "✓ snake_tail_down.png loaded successfully" << endl;
        }
        
        if (!snakeTailLeftTexture.loadFromFile("snake_tail_left.png")) {
            cout << "✗ FAILED to load snake_tail_left.png" << endl;
        } else {
            cout << "✓ snake_tail_left.png loaded successfully" << endl;
        }
        
        if (!snakeTailRightTexture.loadFromFile("snake_tail_right.png")) {
            cout << "✗ FAILED to load snake_tail_right.png" << endl;
        } else {
            cout << "✓ snake_tail_right.png loaded successfully" << endl;
        }
        
        // Khúc cua - 4 loại
        if (!snakeCornerURTexture.loadFromFile("snake_corner_ur.png")) { // ⬏
            cout << "✗ FAILED to load snake_corner_ur.png" << endl;
        } else {
            cout << "✓ snake_corner_ur.png loaded successfully" << endl;
        }
        
        if (!snakeCornerULTexture.loadFromFile("snake_corner_ul.png")) { // ⬑
            cout << "✗ FAILED to load snake_corner_ul.png" << endl;
        } else {
            cout << "✓ snake_corner_ul.png loaded successfully" << endl;
        }
        
        if (!snakeCornerDRTexture.loadFromFile("snake_corner_dr.png")) { // ⬎
            cout << "✗ FAILED to load snake_corner_dr.png" << endl;
        } else {
            cout << "✓ snake_corner_dr.png loaded successfully" << endl;
        }
        
        if (!snakeCornerDLTexture.loadFromFile("snake_corner_dl.png")) { // ⬐
            cout << "✗ FAILED to load snake_corner_dl.png" << endl;
        } else {
            cout << "✓ snake_corner_dl.png loaded successfully" << endl;
        }
        
        cout << "=== TEXTURE LOADING COMPLETE ===" << endl;
    }

    // Hàm reset game về trạng thái ban đầu - SỬA QUAN TRỌNG
    void resetGame() {
        body.clear();                                
        bodyDirections.clear();
        body.push_back(Vector2i(WIDTH / 2, HEIGHT / 2));
        bodyDirections.push_back(RIGHT);
        dir = RIGHT;  // SỬA: Bắt đầu với hướng RIGHT thay vì STOP
        score = 0;                                   
        gameOver = false;                            
        moveTimer = 0;                               
        spawnFood();                                 
        updateScoreText();                           
    }

    // Hàm tạo thức ăn ở vị trí ngẫu nhiên
    void spawnFood() {
        do {
            food.x = 1 + rand() % (WIDTH - 2);
            food.y = 1 + rand() % (HEIGHT - 2);
        } while (isPositionInSnake(food));
    }

    // Hàm kiểm tra vị trí có nằm trong thân rắn không
    bool isPositionInSnake(Vector2i pos) {
        for (auto segment : body) {           
            if (segment == pos) return true;  
        }
        return false;  
    }

    // Hàm cập nhật text hiển thị điểm số
    void updateScoreText() {
        scoreText.setString("Score: " + to_string(score) +   
                          "\n\nControls:\nW - Up\nA - Left\nS - Down\nD - Right\nR - Restart\nESC - Exit");
    }

    // Hàm lấy texture đầu rắn theo hướng
    Texture& getHeadTexture() {
        switch (dir) {
            case UP:    return snakeHeadUpTexture;
            case DOWN:  return snakeHeadDownTexture;
            case LEFT:  return snakeHeadLeftTexture;
            case RIGHT: return snakeHeadRightTexture;
            default:    return snakeHeadRightTexture;
        }
    }

    // Hàm lấy texture đuôi rắn theo hướng
    Texture& getTailTexture() {
        if (bodyDirections.size() < 2) return snakeTailRightTexture;
        
        Direction tailDir = bodyDirections.back();
        switch (tailDir) {
            case UP:    return snakeTailUpTexture;
            case DOWN:  return snakeTailDownTexture;
            case LEFT:  return snakeTailLeftTexture;
            case RIGHT: return snakeTailRightTexture;
            default:    return snakeTailRightTexture;
        }
    }

    // Hàm lấy texture cho thân rắn - SỬA LẠI
    Texture& getBodyTexture(int index) {
        // Kiểm tra index hợp lệ
        if (index < 0 || index >= bodyDirections.size()) {
            return snakeBodyHorizontalTexture;
        }
        
        // Nếu là phần đầu hoặc đuôi, dùng thân thẳng
        if (index == 0 || index == bodyDirections.size() - 1) {
            Direction bodyDir = bodyDirections[index];
            if (bodyDir == UP || bodyDir == DOWN) {
                return snakeBodyVerticalTexture;
            } else {
                return snakeBodyHorizontalTexture;
            }
        }
        
        // Xác định khúc cua
        Direction prevDir = bodyDirections[index - 1];
        Direction currDir = bodyDirections[index];
        
        // Nếu cùng hướng -> thân thẳng
        if (prevDir == currDir) {
            if (prevDir == UP || prevDir == DOWN) {
                return snakeBodyVerticalTexture;
            } else {
                return snakeBodyHorizontalTexture;
            }
        }
        
        // Khác hướng -> khúc cua
        if ((prevDir == UP && currDir == RIGHT) || (prevDir == LEFT && currDir == DOWN)) {
            return snakeCornerURTexture; // ⬏
        } else if ((prevDir == UP && currDir == LEFT) || (prevDir == RIGHT && currDir == DOWN)) {
            return snakeCornerULTexture; // ⬑
        } else if ((prevDir == DOWN && currDir == RIGHT) || (prevDir == LEFT && currDir == UP)) {
            return snakeCornerDRTexture; // ⬎
        } else if ((prevDir == DOWN && currDir == LEFT) || (prevDir == RIGHT && currDir == UP)) {
            return snakeCornerDLTexture; // ⬐
        }
        
        // Mặc định
        return snakeBodyHorizontalTexture;
    }

    // Hàm xử lý input từ người dùng
    void processInput() {
        while (optional<Event> event = window.pollEvent()) {  
            if (event->is<Event::Closed>()) {      
                window.close();                    
                return;                           
            }
            
            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {  
                if (gameOver) {                  
                    if (keyPressed->scancode == Keyboard::Scancode::R) {  
                        resetGame();             
                    }
                } else {                         
                    switch (keyPressed->scancode) {  
                        case Keyboard::Scancode::W:
                        case Keyboard::Scancode::Up:
                            if (dir != DOWN) dir = UP;    
                            break;
                        case Keyboard::Scancode::S:
                        case Keyboard::Scancode::Down:
                            if (dir != UP) dir = DOWN;    
                            break;
                        case Keyboard::Scancode::A:
                        case Keyboard::Scancode::Left:
                            if (dir != RIGHT) dir = LEFT; 
                            break;
                        case Keyboard::Scancode::D:
                        case Keyboard::Scancode::Right:
                            if (dir != LEFT) dir = RIGHT; 
                            break;
                        case Keyboard::Scancode::Escape:
                            window.close();               
                            break;
                    }
                }
            }
        }
    }

    // Hàm cập nhật logic game
    void update() {
        if (gameOver || dir == STOP) return;

        float deltaTime = gameClock.restart().asSeconds();  
        moveTimer += deltaTime;               

        if (moveTimer >= SPEED) {             
            moveTimer = 0;                    

            Vector2i newHead = body[0];       
            Direction newHeadDir = dir;

            switch (dir) {                    
                case UP:    newHead.y--; break;    
                case DOWN:  newHead.y++; break;    
                case LEFT:  newHead.x--; break;    
                case RIGHT: newHead.x++; break;    
            }

            // Kiểm tra va chạm tường
            if (newHead.x <= 0 || newHead.x >= WIDTH - 1|| newHead.y <= 0 || newHead.y >= HEIGHT - 1) {
                gameOver = true;  
                return;
            }

            // Kiểm tra va chạm thân
            for (size_t i = 1; i < body.size(); i++) {  
                if (newHead == body[i]) {      
                    gameOver = true;           
                    return;
                }
            }

            // Di chuyển rắn
            body.insert(body.begin(), newHead);  
            bodyDirections.insert(bodyDirections.begin(), newHeadDir);

            // Kiểm tra ăn thức ăn
            if (newHead == food) {           
                score++;                     
                updateScoreText();           
                spawnFood();                 
            } else {
                body.pop_back();             
                bodyDirections.pop_back();
            }
        }
    }

    // Hàm vẽ game lên màn hình
    void render() {
        window.clear(Color(40, 40, 40));

        // Tạo sprite
        Sprite grassSprite(grassTexture);
        Sprite wallSprite(wallTexture);
        Sprite foodSprite(foodTexture);

        // Vẽ nền cỏ
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
                grassSprite.setPosition(Vector2f(static_cast<float>(x * CELL_SIZE), 
                                               static_cast<float>(y * CELL_SIZE)));
                window.draw(grassSprite);
            }
        }

        // Vẽ tường
        for (int x = 0; x < WIDTH; x++) {
            wallSprite.setPosition(Vector2f(static_cast<float>(x * CELL_SIZE), 0.0f));
            window.draw(wallSprite);
            wallSprite.setPosition(Vector2f(static_cast<float>(x * CELL_SIZE), 
                                     static_cast<float>((HEIGHT - 1) * CELL_SIZE)));
            window.draw(wallSprite);
        }
        
        for (int y = 0; y < HEIGHT; y++) {
            wallSprite.setPosition(Vector2f(0.0f, static_cast<float>(y * CELL_SIZE)));
            window.draw(wallSprite);
            wallSprite.setPosition(Vector2f(static_cast<float>((WIDTH - 1) * CELL_SIZE), 
                                     static_cast<float>(y * CELL_SIZE)));
            window.draw(wallSprite);
        }

        // Vẽ thức ăn
        foodSprite.setPosition(Vector2f(static_cast<float>(food.x * CELL_SIZE), 
                                      static_cast<float>(food.y * CELL_SIZE)));
        window.draw(foodSprite);

        // Vẽ rắn bằng texture
        if (!body.empty()) {
            for (size_t i = 0; i < body.size(); i++) {
                Sprite snakeSprite;
                
                if (i == 0) {
                    // Đầu rắn
                    snakeSprite.setTexture(getHeadTexture());
                } else if (i == body.size() - 1) {
                    // Đuôi rắn
                    snakeSprite.setTexture(getTailTexture());
                } else {
                    // Thân rắn
                    snakeSprite.setTexture(getBodyTexture(i));
                }
                
                snakeSprite.setPosition(Vector2f(static_cast<float>(body[i].x * CELL_SIZE), 
                                               static_cast<float>(body[i].y * CELL_SIZE)));
                window.draw(snakeSprite);
            }
        }

        // Vẽ thông tin
        window.draw(scoreText);

        if (gameOver) {
            window.draw(gameOverText);       
        }

        window.display();                    
    }

    // Hàm chạy game chính
    void run() {
        srand(static_cast<unsigned int>(time(nullptr)));  
        
        while (window.isOpen()) {            
            processInput();                  
            update();                        
            render();                        
        }
    }
};

// Hàm main - điểm bắt đầu chương trình
int main() {
    SnakeGame game;    
    game.run();        
    return 0;          
}