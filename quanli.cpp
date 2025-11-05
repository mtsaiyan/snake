#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <optional>

using namespace std;

// Lớp cơ sở Người
class Person {
protected:
    string id;
    string name;
    int age;
    string gender;

public:
    Person(string id, string name, int age, string gender)
        : id(id), name(name), age(age), gender(gender) {}

    virtual string getInfo() = 0;
    string getName() const { return name; }
    string getId() const { return id; }
};

// Lớp Bệnh nhân
class Patient : public Person {
private:
    string disease;
    string admissionDate;

public:
    Patient(string id, string name, int age, string gender, string disease, string admissionDate)
        : Person(id, name, age, gender), disease(disease), admissionDate(admissionDate) {}

    string getInfo() override {
        return "BN: " + id + " | " + name + " | " + to_string(age) + " | " + gender + 
               " | Benh: " + disease + " | Ngay: " + admissionDate;
    }
};

// Lớp Bác sĩ
class Doctor : public Person {
private:
    string specialty;
    string shift;

public:
    Doctor(string id, string name, int age, string gender, string specialty, string shift)
        : Person(id, name, age, gender), specialty(specialty), shift(shift) {}

    string getInfo() override {
        return "BS: " + id + " | " + name + " | " + to_string(age) + " | " + gender + 
               " | Chuyen khoa: " + specialty + " | Ca: " + shift;
    }
};

// Lớp Lịch khám
class Appointment {
private:
    string patientId;
    string doctorId;
    string date;
    string time;
    string status;

public:
    Appointment(string patientId, string doctorId, string date, string time, string status = "Cho kham")
        : patientId(patientId), doctorId(doctorId), date(date), time(time), status(status) {}

    string getInfo() {
        return "Lich: BN-" + patientId + " | BS-" + doctorId + " | " + date + " " + time + " | " + status;
    }
};

// Lớp Đơn thuốc
class Prescription {
private:
    string patientId;
    string doctorId;
    string medicine;
    string dosage;
    string date;

public:
    Prescription(string patientId, string doctorId, string medicine, string dosage, string date)
        : patientId(patientId), doctorId(doctorId), medicine(medicine), dosage(dosage), date(date) {}

    string getInfo() {
        return "Don thuoc: BN-" + patientId + " | BS-" + doctorId + " | " + medicine + " | " + dosage + " | " + date;
    }
};

// Hàm tạo text cho SFML 3.0
sf::Text createText(const sf::Font& font, const string& str, unsigned int size, const sf::Color& color, float x, float y) {
    sf::Text text(font, str, size);
    text.setFillColor(color);
    text.setPosition(sf::Vector2f(x, y));
    return text;
}

// Lớp quản lý bệnh viện
class HospitalManager {
private:
    vector<Patient> patients;
    vector<Doctor> doctors;
    vector<Appointment> appointments;
    vector<Prescription> prescriptions;

    int currentView; // 0: Patients, 1: Doctors, 2: Appointments, 3: Prescriptions
    sf::Font font;
    bool fontLoaded;

public:
    HospitalManager() : currentView(0), fontLoaded(false) {
        // Thử tải font
        if (font.openFromFile("arial.ttf")) {
            fontLoaded = true;
            cout << "Font loaded successfully!" << endl;
        } else {
            cout << "Cannot load arial.ttf. Text will not display." << endl;
            cout << "Please make sure arial.ttf is in the same folder as the executable." << endl;
        }
        
        // Thêm dữ liệu mẫu
        addSampleData();
    }

    void addSampleData() {
        // Thêm bệnh nhân mẫu
        patients.push_back(Patient("P001", "Nguyen Van A", 35, "Nam", "Cam cum", "2024-01-15"));
        patients.push_back(Patient("P002", "Tran Thi B", 28, "Nu", "Da day", "2024-01-16"));
        patients.push_back(Patient("P003", "Le Van C", 45, "Nam", "Tieu duong", "2024-01-17"));

        // Thêm bác sĩ mẫu
        doctors.push_back(Doctor("D001", "BS. Pham Van X", 40, "Nam", "Noi khoa", "Sang"));
        doctors.push_back(Doctor("D002", "BS. Nguyen Thi Y", 35, "Nu", "Nhi khoa", "Chieu"));
        doctors.push_back(Doctor("D003", "BS. Le Van Z", 50, "Nam", "Ngoai khoa", "Sang"));

        // Thêm lịch hẹn mẫu
        appointments.push_back(Appointment("P001", "D001", "2024-01-18", "08:00"));
        appointments.push_back(Appointment("P002", "D002", "2024-01-18", "14:00"));
        appointments.push_back(Appointment("P003", "D003", "2024-01-19", "09:00"));

        // Thêm đơn thuốc mẫu
        prescriptions.push_back(Prescription("P001", "D001", "Paracetamol", "500mg x 3 lan/ngay", "2024-01-18"));
        prescriptions.push_back(Prescription("P002", "D002", "Omeprazole", "20mg x 1 lan/ngay", "2024-01-18"));
        prescriptions.push_back(Prescription("P003", "D003", "Metformin", "500mg x 2 lan/ngay", "2024-01-19"));
    }

    void switchView(int view) {
        currentView = view;
    }

    void draw(sf::RenderWindow& window) {
        // Vẽ background
        window.clear(sf::Color(30, 30, 60));

        if (!fontLoaded) {
            drawNoFontMessage(window);
            return;
        }

        // Vẽ tiêu đề
        drawTitle(window);

        // Vẽ nút chuyển đổi
        drawButtons(window);

        // Vẽ thông tin theo view hiện tại
        drawContent(window);
    }

private:
    void drawNoFontMessage(sf::RenderWindow& window) {
        // Hiển thị thông báo lỗi font (không dùng font)
        sf::Text errorText(font, "LOI: KHONG TIM THAY FONT arial.ttf", 24);
        errorText.setFillColor(sf::Color::Red);
        errorText.setPosition(sf::Vector2f(200.f, 250.f));
        window.draw(errorText);

        sf::Text helpText(font, "Hay dat file arial.ttf vao cung thu muc voi chuong trinh", 18);
        helpText.setFillColor(sf::Color::Yellow);
        helpText.setPosition(sf::Vector2f(150.f, 300.f));
        window.draw(helpText);
    }

    void drawTitle(sf::RenderWindow& window) {
        vector<string> titles = {
            "QUAN LY BENH NHAN", 
            "QUAN LY BAC SI", 
            "QUAN LY LICH HEN", 
            "QUAN LY DON THUOC"
        };
        
        sf::Text title = createText(font, titles[currentView], 28, sf::Color::Yellow, 250.f, 20.f);
        window.draw(title);
    }

    void drawButtons(sf::RenderWindow& window) {
        vector<string> buttonLabels = {"BENH NHAN", "BAC SI", "LICH HEN", "DON THUOC"};
        vector<sf::Color> buttonColors = {
            sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Magenta
        };

        for (int i = 0; i < 4; i++) {
            // Vẽ nút background
            sf::RectangleShape button(sf::Vector2f(140.f, 40.f));
            button.setPosition(sf::Vector2f(20.f + i * 160.f, 70.f));
            button.setFillColor(currentView == i ? buttonColors[i] : sf::Color(100, 100, 100));
            button.setOutlineColor(sf::Color::White);
            button.setOutlineThickness(2.f);
            window.draw(button);

            // Vẽ text trên nút
            sf::Text text = createText(font, buttonLabels[i], 14, sf::Color::White, 25.f + i * 160.f, 78.f);
            window.draw(text);
        }
    }

    void drawContent(sf::RenderWindow& window) {
        float startY = 130.f;
        
        switch(currentView) {
            case 0: // Patients
                for (auto& patient : patients) {
                    drawTextLine(window, patient.getInfo(), startY, sf::Color::White);
                    startY += 35.f;
                }
                break;
            case 1: // Doctors
                for (auto& doctor : doctors) {
                    drawTextLine(window, doctor.getInfo(), startY, sf::Color::White);
                    startY += 35.f;
                }
                break;
            case 2: // Appointments
                for (auto& appointment : appointments) {
                    drawTextLine(window, appointment.getInfo(), startY, sf::Color::White);
                    startY += 35.f;
                }
                break;
            case 3: // Prescriptions
                for (auto& prescription : prescriptions) {
                    drawTextLine(window, prescription.getInfo(), startY, sf::Color::White);
                    startY += 35.f;
                }
                break;
        }
    }

    void drawTextLine(sf::RenderWindow& window, const string& text, float y, sf::Color color) {
        sf::Text textObj = createText(font, text, 16, color, 30.f, y);
        window.draw(textObj);
    }

public:
    void handleClick(int x, int y) {
        // Kiểm tra click trên các nút
        for (int i = 0; i < 4; i++) {
            if (x >= 20 + i * 160 && x <= 160 + i * 160 && y >= 70 && y <= 110) {
                switchView(i);
                break;
            }
        }
    }
};

int main() {
    // Tạo cửa sổ
    sf::RenderWindow window(sf::VideoMode({800, 600}), "He Thong Quan Ly Benh Vien");
    HospitalManager manager;

    cout << "=== HE THONG QUAN LY BENH VIEN ===" << endl;
    cout << "Huong dan:" << endl;
    cout << "- Click vao cac nut de chuyen doi giua cac che do" << endl;
    cout << "- Benh nhan, Bac si, Lich hen, Don thuoc" << endl;

    while (window.isOpen()) {
        // Xử lý sự kiện
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    manager.handleClick(mouseEvent->position.x, mouseEvent->position.y);
                }
            }
        }

        // Vẽ
        manager.draw(window);
        window.display();
    }

    return 0;
}