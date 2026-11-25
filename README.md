3 Led - GPIO 39,40,41 ( Basical PORT " Digital I/O "  Control digital signal ( high/low <=> 1/0 ))

1 Moisture  soil sensor - GPIO 1

1 Dht22 - GPIO 2

1 Oled - GPIO 4,5 (scl,sda)

1 pump through L298N channel A:
 - ENA - GPIO 7
 - IN1 - GPIO 8
 - IN2 - GPIO 9

Remove the ENA jumper on the L298N so GPIO 7 can enable/disable the pump. Connect
the pump to OUT1/OUT2, power the pump from a suitable external motor supply, and
connect the ESP32-S3 ground to the L298N ground. The firmware runs the pump in
one direction; swap the pump wires or IN1/IN2 assignments if its direction is
incorrect.

1 measure sensor for check water recently capacity - GPIO6
