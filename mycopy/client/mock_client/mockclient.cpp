#include <Arduino.h>

// program that mocks client function
void consumeSerialInput() {
    while (Serial.available() > 0) Serial.read();
}

int recNumWayAndAck(int &waypoints, int max_waypoints) {
    // reading and validating
    char num = Serial.read();
    char space = Serial.read();
    if (num != 'N' || space != ' ') {
        return -1;
    }

    waypoints = static_cast<int32_t>(Serial.read());
    Serial.print('A\n');
    if (waypoints > max_waypoints) {
        //status_message("too many waypoints requested");
        return 0;
    }

    if (waypoints > 0) return 1;
    else if (waypoints == 0) return 0;
    else return -1;
}

int recWayAndAck(uint16_t counter, int waypoints) {
    char way = Serial.read();
    char space = Serial.read();
    if (way != 'W' || space != ' ') return -1;

    int32_t lat = Serial.readStringUntil(' ').toInt();
    int32_t lon = Serial.readStringUntil('\n').toInt();
    //waypoints[counter] = {lat, lon};
    if (waypoints-1 > counter) Serial.print('A\n');

    if (way == 'W') return 0;
    else if (way == '1') return 1;
    else return -1;
}

bool waitOnSerial(uint8_t nBytes, uint32_t timeout) {
    uint32_t limit = millis() + timeout;
    while (Serial3.available() < nBytes && (timeout < 0 || millis() < limit)) {
        delay(1);
    }
    return Serial.available() >= nBytes;
}

void sendRequest(int32_t slon, int32_t slat, int32_t elon, int32_t elat) {
  
    Serial.write("R ");
    Serial.print(slon);
    Serial.write(' ');
    Serial.print(slat);
    Serial.write(' ');
    Serial.print(elon);
    Serial.write(' ');
    Serial.print(elat);
    Serial.print('\n');
}

void setup() {
    init();
    Serial.begin(9600);
}

int main() {
    setup();
    int32_t Slat = 5365486, Slon = -11333915;
    int32_t Elat = 5364728, Elon = -11335891;
    int waypoints = 0;

    enum {START, WAIT_ON_WAYPOINTS, REC_WAYPOINTS, END} currState = START;
    uint16_t oneSecond = 1000, tenSeconds = 10000;
    int32_t wayCounter = 0;

    consumeSerialInput();
    while (currState != END) {
        if (currState == START) {
            sendRequest(Slat, Slon, Elat, Elon);
            currState = WAIT_ON_WAYPOINTS;
        }

        else if (currState == WAIT_ON_WAYPOINTS) {
            // waiting for >= 3 bytes and timeout of 10 seconds
            if (waitOnSerial(3, tenSeconds)) {
                int success = recNumWayAndAck(waypoints, 500);
                if (success == -1) return 0;
                else if (success == 0) currState = END;
                else if (success == 1) currState = REC_WAYPOINTS;
            } else {
                currState = START;
            }
        }

        else if (currState == REC_WAYPOINTS) {
            // waiting for >= 3 bytes and timeout of 1 second
            if (waitOnSerial(3, oneSecond)) {
                int transitionTo = recWayAndAck(wayCounter, waypoints);
                if (transitionTo == -1
                    || wayCounter > waypoints) return 0;
                else if (transitionTo == 0) wayCounter++;
                else if (transitionTo == 1) currState = END;
            } else {
                currState = START;
            }
        }
    }

}