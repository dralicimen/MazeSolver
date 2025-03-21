#include <Arduino.h>
#include <main.h>
#include <Encoder.h>
#include <EEPROM.h>
#include <includes/motors/motors.h>

int leftCalib = 0; // Sol motorun hız kalibrasyonu
int rightCalib = 0; // Sağ motorun hız kalibrasyonu

#define TURNSTEP 750
#define FORWARDSTEP 1800

Encoder knobLeft(3, 7);
Encoder knobRight(2, 4);
const uint8_t w = 33;
const uint8_t h = 33;
uint8_t maze[w][h];
uint8_t posX = 16;
uint8_t posY = 16;
int command = -1;

const int motors[2][2] = {{6, 5}, {10, 9}};

const int sO1 = 11;
const int sO2 = 12;
const int sO3 = 8;
const int sG = 13;

uint8_t leftRange[2] = {50, 200};
uint8_t rightRange[2] = {50, 200};


long positionLeft = 0;
long positionRight = 0;
int direction = 0;
long togoLeft = 0;
long stepLeft = 0;
long togoRight = 0;
long stepRight = 0;

bool oL = false;
bool oR = false;
bool oF = false;
bool oG = false;
bool deadEnd = false;

bool routeRight = 1;
bool routeLeft = 1;
bool forward = false;

// EEPROM'dan motor ayarlarını yükleme
void loadMotorCalibration() {
    EEPROM.get(0, leftCalib);
    EEPROM.get(4, rightCalib);

    // EEPROM'daki değerleri kontrol et, eğer geçerli değilse sıfırla
    if (leftCalib < 0 || leftCalib > 50) leftCalib = 0;
    if (rightCalib < 0 || rightCalib > 50) rightCalib = 0;
}

// EEPROM'a motor ayarlarını kaydetme
void saveMotorCalibration() {
    EEPROM.put(0, leftCalib);
    EEPROM.put(4, rightCalib);
}

void initializeMotors() {
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    stopLeft();
    stopRight();
}

// Sensörleri başlat
void initializeSensors() {
    pinMode(sO1, INPUT);
    pinMode(sO2, INPUT);
    pinMode(sO3, INPUT);
    pinMode(sG, INPUT);
}

void fillMaze() {
    for (int i = 0; i < w; i++) {
        for (int k = 0; k < h; k++) {
            maze[i][k] = 100;
        }
    }
    maze[posX][posY] = maze[posX][posY] - 1;
}

void stopLeft() {
    digitalWrite(motors[0][0], LOW);
    digitalWrite(motors[0][1], LOW);
    togoLeft = 0;
    stepLeft = 0;
}

void stopRight() {
    digitalWrite(motors[1][0], LOW);
    digitalWrite(motors[1][1], LOW);
    togoRight = 0;
    stepRight = 0;
}

void stopMotors() {
    stopLeft();
    stopRight();
}


void checkSensors() {
    oF = !digitalRead(sO1);
    oL = !digitalRead(sO2);
    oR = !digitalRead(sO3);
    oG = (analogRead(A0) < 100);
}

int dx(int dir) {
    int d[4] = {0, 1, 0, -1}; // 0: Yukarı, 1: Sağ, 2: Aşağı, 3: Sol
    return d[dir % 4];
}

int dy(int dir) {
    int d[4] = {-1, 0, 1, 0}; // 0: Yukarı, 1: Sağ, 2: Aşağı, 3: Sol
    return d[dir % 4];
}


//--Control Motors--//
// PID değişkenleri
float Kp = 1; // Proportional katsayısı
float Ki = 0.001; // Integral katsayısı
float Kd = 0.1; // Derivative katsayısı

float integralLeft = 0, lastErrorLeft = 0;
float integralRight = 0, lastErrorRight = 0;


int randomSelectCommand() {
    int frontX = posX + dx(0);
    int frontY = posY + dy(0);
    int rightX = posX + dx(1);
    int rightY = posY + dy(1);
    int leftX = posX + dx(-1);
    int leftY = posY + dy(-1);

    // **Labirentte bu karelerin değerlerini al (Engel varsa 0 kabul et)**
    int frontCell = !oF ? (101 - maze[frontX][frontY]) : 0;
    int rightCell = !oR ? (101 - maze[rightX][rightY]) : 0;
    int leftCell = !oL ? (101 - maze[leftX][leftY]) : 0;


    // **Ağırlıkları belirle (Ön baskın)**
    int frontWeight = (frontCell > 0) ? (101 - frontCell) * 50 : 0; // **Ön için daha yüksek ağırlık**
    int rightWeight = (rightCell > 0) ? (101 - rightCell) * 15 : 0; // **Sağ için düşük ağırlık**
    int leftWeight = (leftCell > 0) ? (101 - leftCell) * 15 : 0; // **Sol için düşük ağırlık**

    // **Toplam ağırlık değerini hesapla**
    int totalWeight = frontWeight + rightWeight + leftWeight;


    // **Ağırlıklı rastgele seçim**
    int randValue = random(0, totalWeight);


    // **Öncelikli hareket seçimi**Y
    if (randValue < frontWeight) {
        return 1;
    } else if (randValue < frontWeight + rightWeight) {
        return 3;
    } else {
        return 2;
    }
}

int computeCommand() {
    if (command != -1) {
        return command;
    }
    int cmd = 0;
    checkSensors();
    if (oF) {
        if (!oL && oR) {
            cmd = 1;
        } else if (oL && !oR) {
            cmd = 3;
        } else if (oL && oR) {
            cmd = 4;
        } else {
            cmd = randomSelectCommand();
        }
    } else {
        cmd = randomSelectCommand();
    }
    Serial.print("C:");
    Serial.print(cmd);
    return cmd;
}

void useCommand(int command) {
    switch (command) {
        case 1:
            // İleri hareket fonksiyonu
            moveForward();
            break;
        case 2:
            turnLeft();
            deadEnd = false;
        // Sola dön fonksiyonu
            break;
        case 3:
            turnRight();
            deadEnd = false;
        // Sağa dön fonksiyonu
            break;
        case 4:
            turnBackwards();
        // Geri hareket fonksiyonu
            break;
        default:
            stopMotors();
            break;
    }
}

// Motorları sürme fonksiyonu
void setMotorSpeed(int motor, float speed) {
    int pwm = abs(speed);
    bool direction = (speed >= 0);

    int pin1 = motors[motor][0];
    int pin2 = motors[motor][1];

    if (direction) {
        analogWrite(pin1, pwm);
        analogWrite(pin2, 0);
    } else {
        analogWrite(pin1, 0);
        analogWrite(pin2, pwm);
    }
}


// PID Hesaplama Fonksiyonu
float computePID(float target, float current, float &integral, float &lastError) {
    float error = target - current;

    // Sadece motor hareket halindeyken integral topla
    if (error != 0) {
        integral += error;
    } else {
        integral = 0; // Hedefe ulaşıldıysa integral sıfırla
    }

    float derivative = error - lastError;
    lastError = error;

    float output = (Kp * error) + (Ki * integral) + (Kd * derivative);

    // **Pozitif ve negatif hız sınırları**
    const int minSpeed = 50;
    const int maxSpeed = 100;

    // **Eğer ileri gitmesi gerekiyorsa (+) 50-100 arasında kalsın**
    if (output > 0) {
        output = constrain(output, minSpeed, maxSpeed);
    }
    // **Eğer geri gitmesi gerekiyorsa (-) -50 ile -100 arasında kalsın**
    else {
        output = constrain(output, -maxSpeed, -minSpeed);
    }

    return output;
}

// **Motorları PID ile güncelleme** (Loop içinde sürekli çağrılmalı)
void updateMotors() {
    stepLeft = -knobLeft.read();
    stepRight = knobRight.read();
    int moving = 0;

    if (command != -1) {
        // **Sol motor kontrolü**
        if (abs(togoLeft - stepLeft) > 10) {
            float speedLeft = computePID(togoLeft, stepLeft, integralLeft, lastErrorLeft);
            setMotorSpeed(0, speedLeft + leftCalib); // **Sol motora kalibrasyon ekleniyor**

            moving++;
        } else {
            setMotorSpeed(0, 0); // **Motoru tamamen durdur**
            integralLeft = 0;
            lastErrorLeft = 0;
        }

        // **Sağ motor kontrolü**
        if (abs(togoRight - stepRight) > 10) {
            float speedRight = computePID(togoRight, stepRight, integralRight, lastErrorRight);
            setMotorSpeed(1, speedRight + rightCalib); // **Sağ motora kalibrasyon ekleniyor**
            moving++;
        } else {
            setMotorSpeed(1, 0); // **Motoru tamamen durdur**
            integralRight = 0;
            lastErrorRight = 0;
        }
    }
    Serial.print(" MV:");
    Serial.print(moving);
    Serial.print(" CM:");
    Serial.print(command);
    Serial.print("||");

    // Eğer motorlar hareket etmiyorsa, değerleri sıfırla ve EEPROM'a kaydet
    if (command != -1 && moving == 0) {
        knobLeft.write(0);
        knobRight.write(0);
        togoLeft = 0;
        togoRight = 0;
        stepLeft = 0;
        stepRight = 0;
        command = -1;

        // EEPROM'a kalibrasyonu kaydet
        saveMotorCalibration();
    }
}

// Motor Güçlerini Kalibre Etme
void calibrateMotors() {
    float diff = stepLeft - stepRight;

    if (abs(diff) > 5) {
        // Motorlar arasındaki fark belirli bir eşiğin üzerindeyse
        if (diff > 0) {
            rightCalib += 1; // Sağ motor daha yavaşsa hızını artır
        } else {
            leftCalib += 1; // Sol motor daha yavaşsa hızını artır
        }
    }
}


// **İleri Hareket**
void moveForward() {
    writeMaze();
    togoLeft = FORWARDSTEP;
    togoRight = FORWARDSTEP;
}

// **Geri Hareket**
void moveBackwards() {
    writeMaze();
    togoLeft = -FORWARDSTEP;
    togoRight = -FORWARDSTEP;
}

// **Sola Dönüş**
void turnLeft() {
    togoLeft = -TURNSTEP;
    togoRight = TURNSTEP;
    direction = (direction + 3) % 4;
}

// **Sağa Dönüş**
void turnRight() {
    togoLeft = TURNSTEP;
    togoRight = -TURNSTEP;
    direction = (direction + 1) % 4;
}

// **Arkaya (180°) Dönüş**
void turnBackwards() {
    togoLeft = 2 * TURNSTEP;
    togoRight = -2 * TURNSTEP;
    direction = (direction + 2) % 4; // **180 derece döndür**

    // **Güncellenen yönü ekrana yaz**
    Serial.print("Yeni yön (Geri Dönüş): ");
    Serial.println(direction);
}

//--Control Motors--//


void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    initializeMotors();
    loadMotorCalibration();
    initializeSensors();
    fillMaze();
}

void loop() {
    // put your main code here, to run repeatedly:
    checkSensors();
    mazeSolved();
    int cmd = computeCommand();
    if (cmd != command) {
        command = cmd;
        useCommand(command);
    }
    updateMotors();
    debug2();
}

void mazeSolved() {
    while (oF && oL && oR && oG) {
        delay(1);
        checkSensors();
    }
}

void writeMaze() {
    // Yeni konumu hesapla (direction yönüne göre bir adım ilerle)
    int newX = posX + dx(direction);
    int newY = posY + dy(direction);

    if (!deadEnd) {
        // **Normal ilerliyorsak, hücre değerini azalt**
        maze[posX][posY]--;
    } else {
        // **Çıkmaz sokaktan dönüyorsak, geçtiğimiz yerleri 0 yap**
        maze[posX][posY] = 0;
    }

    // **Yeni konum geçerli alandaysa güncelle**
    if (newX >= 0 && newX < w && newY >= 0 && newY < h) {
        posX = newX;
        posY = newY;
    }

    Serial.print("Yeni Konum: X = ");
    Serial.print(posX);
    Serial.print(" | Y = ");
    Serial.println(posY);
}


void debug() {
    Serial.println("----- LABİRENT DURUMU -----");
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (x == posX && y == posY) {
                Serial.print("[");
                Serial.print(direction); // Robotun olduğu konumu göster
                Serial.print("]"); // Robotun olduğu konumu göster
            } else if (maze[x][y] == 0) {
                Serial.print(" X "); // Çıkmaz yolları göster
            } else {
                Serial.print(maze[x][y]); // Hücre değerlerini göster
                Serial.print(" ");
            }
        }
        Serial.println();
    }
    Serial.println("--------------------------");
}

void debug2() {
    Serial.print(command);
    Serial.print("--");
    Serial.print(togoLeft);
    Serial.print(":");
    Serial.print(stepLeft);
    Serial.print("|");
    Serial.print(togoRight);
    Serial.print(":");
    Serial.print(stepRight);
    Serial.println("|");
}

void waitForEnter() {
    /*Serial.println("Devam etmek için Enter tuşuna bas...");

    while (true) {
        if (Serial.available() > 0) { // Kullanıcı bir şey girdi mi kontrol et
            char c = Serial.read();  // Seri porttan gelen karakteri oku
            if (c == '\n') {  // Eğer Enter (New Line) tuşuna basıldıysa çık
                break;
            }

        }
    }*/
}
