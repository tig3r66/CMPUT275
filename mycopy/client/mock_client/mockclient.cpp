#include <Arduino.h>

// program that mocks client functio
void setup() {
    init();
    Serial.begin(9600);
}

int main() {
    setup();
    int32_t Slat = 5365486, Slon = -11333915;
    int32_t Elat = 5364728, Elon = -11335891;

    // sent request
    Serial.write("R ");
    Serial.print(Slat);
    Serial.write(' ');
    Serial.print(Slon);
    Serial.write(' ');
    Serial.print(Elat);
    Serial.write(' ');
    Serial.print(Elon);
    delay(100);
    Serial.write('\n');


}