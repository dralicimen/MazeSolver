#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "includes/motors/motors.h"
#include "includes/sensors/sensors.h"
#include "includes/maze/maze.h"

// =======================
// Zaman Aşımı Sabiti
// =======================
#define COMMAND_TIMEOUT_MS 10000
// =======================
extern unsigned long commandStartTime;
/// @brief Motor kontrol nesnesi (global)
extern Motors motorControl;

/// @brief Sensör kontrol nesnesi (global)
extern Sensors sensorControl;

/// @brief Labirent kontrol nesnesi (global)
extern Maze mazeControl;

/// @brief Mevcut aktif komut (1: ileri, 2: sol, 3: sağ, 4: geri)
extern int command;

/**
 * @brief Başlangıç yapılandırma fonksiyonu (Arduino setup).
 */
void setup();

/**
 * @brief Ana döngü fonksiyonu (Arduino loop).
 */
void loop();

/**
 * @brief Rastgele bir yön seçimi yapar. Hücre değerlerine göre ağırlıklandırma uygulanır.
 * @return Komut kodu: 1 (ileri), 2 (sol), 3 (sağ), 4 (geri)
 */
int randomSelectCommand();

/**
 * @brief Geçerli sensör verilerine göre bir hareket komutu hesaplar.
 * @return Komut kodu
 */
int onNewCommandComputed();

/**
 * @brief Verilen komutu uygular ve robotun yön / pozisyon durumunu günceller.
 * @param cmd Uygulanacak komut
 */
void onUseCommand(int cmd);

/**
 * @brief Labirent bilgilerini günceller ve haritayı yazdırır.
 */
void onMazeUpdated();

/**
 * @brief Hareket sırasında önde engel algılanırsa bu fonksiyon çağrılır.
 */
void onObstacleDetectedWhileMoving();

/**
 * @brief Motor hareketi tamamlandığında yeni bir komut üretilip uygulanır.
 */
void onCommandCompleted();

#endif // MAIN_H
