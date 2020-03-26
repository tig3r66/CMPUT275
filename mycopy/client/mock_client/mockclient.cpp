#include <Arduino.h>

// program that mocks client functio
void setup() {
    init();
    Serial.begin(9600);
}

int main() {
    setup();

    // sent request
    Serial.write("R ");
    Serial.print(5365486);
    Serial.write(' ');
    Serial.print(11333915);
    Serial.write(' ');
    Serial.print(5364728);
    Serial.write(' ');
    Serial.print(5365486);
    Serial.write('\n');


}