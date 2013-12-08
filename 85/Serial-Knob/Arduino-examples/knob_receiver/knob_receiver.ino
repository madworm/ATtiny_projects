#include <SoftwareSerial.h>

// Serial port to receive data from Knob-boards
SoftwareSerial knob_serial(11, 10);	// RX (connects to TX on Knob-board, TX (not used)
byte data;

void setup()
{
	Serial.begin(57600);	// Serial port for PC communication (serial monitor)
	while (!Serial) {
		;		// wait for serial port to connect. Needed for Leonardo only
	}
	knob_serial.begin(9600);
}

void loop()
{
	if (knob_serial.available()) {
		data = knob_serial.read();

		switch (data) {
		case '+':
			Serial.println("Turned clockwise");
			break;
		case '-':
			Serial.println("Turned counterclockwise");
			break;
		case '/':
			Serial.println("Just pushed the button");
			break;
		case '\\':
			Serial.println("Just released the button");
			break;
		case 175:
			Serial.println("Button is pressed");
			break;
		default:
			break;
		}
	}
}
