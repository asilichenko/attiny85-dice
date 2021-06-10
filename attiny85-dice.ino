/**
  Скетч игрального кубика на ATTiny85
  
  Большую часть времени МК спит глубоким сном, но как только

  Copyright (C) 2021 Alexey Silichenko (a.silichenko@gmail.com)
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
  USA
*/

#include <EEPROM.h>
#include <avr/sleep.h>

#define SHOW_NUMBER_DELAY 4000 // Длительность отображения выпавшего значения

#define adc_disable()   bitClear(ADCSRA, ADEN)
#define rand()          random(6)

/**
 * Поскольку "бросание кубика" реализовано через сброс (остальные пины заняты под вывод индикации),
 * то метод setup() выполняется перед каждым "броском", соответственно, 
 * значения сида должны быть максимально разрознены.
 * 
 * Хотя вывод A0 и используется для сброса, но пока кнопка нажата - МК не работает, а вот запускается - только при отпускании
 */
void setup() {
  // для максимизации случайности, смешиваем с аналоговым значением сигнала от неподключенного входа
  // некое сохраненное в памяти значение
  randomSeed(UINT32_MAX & (analogRead(A0) + eeprom_read_dword(0))); // UINT32_MAX & - ограничение верхнего предела значения
}

/**
 * Для максимальной случайности - каждый раз сохраняем в память текущее значение времени
 * и смешиваем его с предыдущим
 */
void updateSeed() {
  uint64_t data = eeprom_read_dword(0) + millis();
  if (data >= UINT32_MAX) data = millis();
  eeprom_write_dword(0, data);
}

void loop() {
  // проснулись, "бросаем кубик"
  roll();
  updateSeed();
  delay(SHOW_NUMBER_DELAY); // отображаем "выпавшее" значение определенное время

  // "прячем кубик", идем спать
  resetPins();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  goSleep();
}

/*
   Pins:
  [1]     [4]
  [3] [2] [3]
  [4]     [1]
*/
byte diceData[6] = {
  B00100, // 1
  B10000, // 2
  B10100, // 3
  B10010, // 4
  B10110, // 5
  B11010  // 6
};

void roll() {
  shuffle();
  for (byte i = 0; i < (31 + rand()); i++) {
    setPins(diceData[i % 6]); // отобразить текущую "верхнюю" грань
    tone(PB0, 120, 10); // изобразить звук кубика, который катится
    delay(getDelay(i)); // кубик постепенно замедляется
  }
}

void shuffle() {
  for (int i = 0; i < 6; i++) {
    int j = rand();
    int temp = diceData[j];
    diceData[j] = diceData[i];
    diceData[i] = temp;
  }
}

/**
 * Замедление по экспоненте: 3 * pow(2.7, x/10) + 120
 */
long getDelay(byte x) {
  const int delayArr[8] = {120, 130, 150, 190, 280, 480};
  return delayArr[x / 8];
}

/**
 * Отобразить "грань"
 */
void setPins (byte pins) {
  DDRB  = B11111; // pin mode PB0-PB4 OUTPUT
  PORTB = pins;   // выставить нужные уровни на пинах
}

/**
 * DDRB – The Port B Data Direction Register (регистр направления передачи данных порта B)
 * PORTB – The Port B Data Register (регистр данных порта B)
*/
void resetPins() {
  DDRB  = 0; // все пины на ввод
  PORTB = 0; // всем пинам низкий уровень
}

/**
 * Вполне стандартный набор инструкций глубокого сна с максимальным энергосбережением
 */
void goSleep() {
  adc_disable();
  sleep_enable();       // Set the SE (sleep enable) bit.
  sleep_bod_disable();  // Recommended practice is to disable the BOD
  sleep_cpu();          // Put the device into sleep mode. The SE bit must be set beforehand, and it is recommended to clear it afterwards.
}
