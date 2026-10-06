#include "Galil.h"

int main(void) {
	EmbeddedFunctions funcs;
	Galil myGalil(&funcs, "192.168.0.120 -d");

	myGalil.DigitalOutput(0);
	if (myGalil.CheckSuccessfulWrite()) std::cout << "All DO off";
	std::cin.get();

	myGalil.DigitalOutput(7);
	if (myGalil.CheckSuccessfulWrite()) std::cout << "DO 0,1,2 on";
	std::cin.get();

	myGalil.DigitalByteOutput(1, 255);
	if (myGalil.CheckSuccessfulWrite()) std::cout << "High bank DO on";
	std::cin.get();

	myGalil.DigitalBitOutput(0, 15);
	if (myGalil.CheckSuccessfulWrite()) std::cout << "DO 15 off";
	std::cin.get();

	myGalil.DigitalOutput(65535);
	if (myGalil.CheckSuccessfulWrite()) std::cout << "All DO on";
	std::cin.get();

	// 255
	std::cout << "DI = " << myGalil.DigitalInput() << std::endl;

	// 0
	std::cout << "DI High Bank = " << static_cast<int>(myGalil.DigitalByteInput(1)) << std::endl;
	
	// 255
	std::cout << "DI Low Bank = " << static_cast<int>(myGalil.DigitalByteInput(0)) << std::endl;

	// DO7 = 1
	std::cout << "DI7 = " << static_cast<int>(myGalil.DigitalBitInput(7)) << std::endl;

	std::cin.get();
	std::cout << "All DO off";
	myGalil.DigitalOutput(0);
	std::cin.get();

	std::cout << "Analogue" << std::endl;
	std::cout << "Reseting AO0" << std::endl;
	std::cin.get();
	myGalil.AnalogOutput(0, 0.0);
	std::cout << "Reseting Encoder" << std::endl;
	std::cin.get();
	myGalil.WriteEncoder();

	std::cout << "Turning on AO0 to 5V" << std::endl;
	std::cin.get();
	myGalil.AnalogOutput(0, 5.0);
	if (myGalil.CheckSuccessfulWrite()) std::cout << "AO0 changed successfully";
	
	std::cout << "Turning on AO0 to 25V" << std::endl;
	std::cin.get();
	myGalil.AnalogOutput(0, 25.0);
	if (myGalil.CheckSuccessfulWrite()) std::cout << "AO0 changed successfully";
	if (!myGalil.CheckSuccessfulWrite()) std::cout << "AO0 cannot be changed";
	std::cin.get();

	std::cout << "AI0 = " << myGalil.AnalogInput(0) << std::endl;
	std::cin.get();

	std::cout << "PID Setup" << std::endl;
	std::cout << "setPoint: " << myGalil.getSetPoint() << std::endl;
	std::cout << "Kp: " << myGalil.getKp() << std::endl;
	std::cout << "Ki: " << myGalil.getKi() << std::endl;
	std::cout << "Kd: " << myGalil.
	getKd() << std::endl;

	std::cin.get();
	myGalil.setSetPoint(100);
	myGalil.setKp(10);
	myGalil.setKi(5);
	myGalil.setKd(3);

	std::cin.get();
	std::cout << "setPoint: " << myGalil.getSetPoint() << std::endl;
	std::cout << "Kp: " << myGalil.getKp() << std::endl;
	std::cout << "Ki: " << myGalil.getKi() << std::endl;
	std::cout << "Kd: " << myGalil.getKd() << std::endl;


	
	//myGalil.AnalogInputRange(2, 2);

	//myGalil.DigitalOutput(0);
	//std::cin.get();

	//myGalil.DigitalOutput(1);       // DO0 only
	//std::cin.get();

	//myGalil.DigitalOutput(2);       // DO1 only
	//std::cin.get();

	//myGalil.DigitalOutput(4);       // DO2 only
	//std::cin.get();

	//myGalil.DigitalOutput(8);       // DO3 only
	//std::cin.get();

	//myGalil.DigitalOutput(16);      // DO4 only
	//std::cin.get();

	//myGalil.DigitalOutput(32);      // DO5 only
	//std::cin.get();

	//myGalil.DigitalOutput(64);      // DO6 only
	//std::cin.get();

	//myGalil.DigitalOutput(128);     // DO7 only
	//std::cin.get();

	//myGalil.DigitalOutput(65280);   // DO-7 only
	//std::cin.get();

	//myGalil.DigitalOutput(256);     // DO8 only
	//std::cin.get();

	//myGalil.DigitalOutput(512);     // DO9 only
	//std::cin.get();

	//myGalil.DigitalOutput(32768);   // DO15 only
	//std::cin.get();

	return 0;
}