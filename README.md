# Игральный кубик на ATTiny85

<p align="center">
  <img width="300" src="https://github.com/asilichenko/attiny85-dice/blob/main/img/dice-schematic.png"/>
</p>

Вывод RST подключен к кнопке сброса, остальные выводы - на индикацию "грани кубика", т.е. к светодиодам, и один вывод - на спикер для изображения звука как будто кубик катится.

Большую часть времени МК спит глубоким сном с максимальным энергосбережением, и для того, чтобы "бросить кубик" необходимо нажать кнопку сброса, а именно - кратковременно подать низкий уровень сигнала на RST вывод.

Как только МК проснулся (точнее перезапустился), формируется сид для рандома из аналогового значения с вывода A0 (он же RST, и именно поэтому в данном проекте не следует подтягивать его к VCC) и некого ранее записанного значения.

Далее происходит "бросок кубика":
- "грани перемешиваются"
- "кубик катится", переворачиваясь 31 + случайное количество раз
- при каждом "повороте" отображается текущая "верхняя грань" и издается звук перекатывания кубика
- качение кубика постепенно замедляется по экпоненциальному закону
- последнее выпавшее значение отображается некоторое время
- текущее время в мс суммируется с предыдущей суммой и записывается в постоянную память для формирования следующего сида
- МК засыпает

<p align="center">
  <img width="300" src="https://github.com/asilichenko/attiny85-dice/blob/main/img/dice-PCB.png"/>
</p>
