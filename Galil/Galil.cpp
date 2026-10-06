#include "Galil.h"

#include <sstream>
#include <iomanip>

constexpr uint16_t trueLowerByte = 0x00FF;
constexpr uint16_t trueUpperByte = 0xFF00;

uint8_t lowerByte(uint16_t old) {
	return static_cast<uint8_t>(old & trueLowerByte);
}

uint8_t higherByte(uint16_t old) {
	return static_cast<uint8_t>(old >> 8);
}

Galil::Galil() :
	Functions(new EmbeddedFunctions()),
	g(0),
	ControlParameters{ 0.0, 0.0, 0.0 },
	setPoint(0),
	Owner(true),
	LastWriteResponse(""),
	LastWriteStatus(G_NO_ERROR),
	CurrentValue(0)
{
	Functions->GOpen("192.168.0.120 -d", &g);
}

Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) :
	Functions(Funcs),
	g(0),
	ControlParameters{ 0.0, 0.0, 0.0 },
	setPoint(0),
	Owner(false),
	LastWriteResponse(""),
	LastWriteStatus(G_NO_ERROR),
	CurrentValue(0)
{
	Functions->GOpen(address, &g);
}

Galil::Galil(const Galil& other) :
	Functions(new EmbeddedFunctions()),
	g(0),
	ControlParameters{other.ControlParameters[0], other.ControlParameters[1], other.ControlParameters[2]},
	setPoint(other.setPoint),
	Owner(true),
	LastWriteResponse(other.LastWriteResponse),
	LastWriteStatus(other.LastWriteStatus),
	CurrentValue(other.CurrentValue)
{
	Functions->GOpen("192.168.0.120 -d", &g);
}

Galil::~Galil() {
	if (g != 0) Functions->GClose(g);
	if (Owner) delete Functions;
}

void Galil::DigitalOutput(uint16_t value) {
	uint8_t lowByte = value & trueLowerByte;
	uint8_t highByte = (value >> 8);
	std::string command = "OP " + std::to_string(lowByte) + ", " + std::to_string(highByte) + ";";
	CurrentValue = value;

	char response[128] = {};
	GSize bytesReturned = 0;

	LastWriteStatus = Functions->GCommand(g, command.c_str(), response, sizeof(response), &bytesReturned);
	LastWriteResponse = response;
}

void Galil::DigitalByteOutput(bool bank, uint8_t value) {
	if (!bank) {
		CurrentValue = (CurrentValue & trueUpperByte) | static_cast<uint16_t>(value);
	} else {
		CurrentValue = (CurrentValue & trueLowerByte) | (static_cast<uint16_t>(value) << 8);
	}

	DigitalOutput(CurrentValue);
}

void Galil::DigitalBitOutput(bool val, uint8_t bit) {
	if (bit > 15) return;
	uint16_t newValue = 0x0001 << bit;
	if (val) {
		newValue |= CurrentValue;
	} else {
		newValue = CurrentValue & ~newValue;
	}
	CurrentValue = newValue;

	DigitalOutput(CurrentValue);
}

uint16_t Galil::DigitalInput() {
	uint16_t res = 0x0000;
	for (int i = 0; i < 16; i++) {
		std::string command = "MG @IN[" + std::to_string(i) + "];";

		char response[128] = {};
		GSize bytesReturned = 0;

		GReturn status = Functions->GCommand(g, command.c_str(), response, sizeof(response), &bytesReturned);
		if (status != G_NO_ERROR) return 0;
		
		if (static_cast<int>(std::stod(response)) == 1) {
			res |= (0x0001 << i);
		}
	}
	return res;
}

uint8_t Galil::DigitalByteInput(bool bank) {
	uint8_t res = 0x00;
	uint16_t res16 = DigitalInput();
	if (bank) {
		res = (res16 >> 8);
	} else {
		res = res16 & 0x00FF;
	}
	return res;
}

bool Galil::DigitalBitInput(uint8_t bit) {
	if (bit > 15) return false;
	uint16_t res16 = DigitalInput();
	return static_cast<bool>((res16 >> bit) & 0x0001);
}

bool Galil::CheckSuccessfulWrite() {
	return (LastWriteStatus == G_NO_ERROR)
		&& (LastWriteResponse.find(":") != std::string::npos)
		&& (LastWriteResponse.find("?") == std::string::npos);
}

float Galil::AnalogInput(uint8_t channel) {
	if (channel > 7) return 0.0;
	std::string command = "MG @AN[" + std::to_string(channel) + "];";

	char response[128] = {};
	GSize bytesReturned = 0;

	GReturn status = Functions->GCommand(g, command.c_str(), response, sizeof(response), &bytesReturned);

	if (status != G_NO_ERROR) return 0.0;
	return static_cast<float>(std::stod(response));
}

void Galil::AnalogOutput(uint8_t channel, double voltage) {
	if (channel > 7) return;

	std::ostringstream ss;
	ss << std::fixed << std::setprecision(2) << voltage;
	std::string voltageStr = ss.str();
	std::string command = "AO " + std::to_string(channel) + ", " + voltageStr + ";";

	char response[128] = {};
	GSize bytesReturned = 0;

	LastWriteStatus = Functions->GCommand(g, command.c_str(), response, sizeof(response), &bytesReturned);
	LastWriteResponse = response;
}

void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {
	if ((channel > 7) || (range < 1) || (range > 4)) return;
	
	std::string command = "AQ " + std::to_string(channel) + ", " + std::to_string(range) + ";";
	char response[128] = {};
	GSize bytesReturned = 0;

	Functions->GCommand(g, command.c_str(), response, sizeof(response), &bytesReturned);
}

void Galil::WriteEncoder() {
	std::string command = "WE 0;";
	char response[128] = {};
	GSize bytesReturned = 0;

	LastWriteStatus = Functions->GCommand(g, command.c_str(), response, sizeof(response), &bytesReturned);
	LastWriteResponse = response;
}

int Galil::ReadEncoder() {
	std::string command = "QE 0;";
	char response[128] = {};
	GSize bytesReturned = 0;

	GReturn status = Functions->GCommand(g, command.c_str(), response, sizeof(response), &bytesReturned);
	if (status != G_NO_ERROR) return 0;
	return std::stoi(response);
}

void Galil::setSetPoint(int s) {
	this->setPoint = s;
}

double Galil::getSetPoint() {
	return this->setPoint;
}

void Galil::setKp(double gain) {
	this->ControlParameters[0] = gain;
}

double Galil::getKp() {
	return this->ControlParameters[0];
}

void Galil::setKi(double gain) {
	this->ControlParameters[1] = gain;
}

double Galil::getKi() {
	return this->ControlParameters[1];
}

void Galil::setKd(double gain) {
	this->ControlParameters[2] = gain;
}

double Galil::getKd() {
	return this->ControlParameters[2];
}

std::ostream& operator<<(std::ostream& output, Galil& galil) {
	char info[128] = {};
	char ver[128] = {};

	galil.Functions->GInfo(galil.g, info, sizeof(info));
	galil.Functions->GVersion(ver, sizeof(ver));

	output << info << "\n\n" << ver << "\n\n";
	return output;
}

Galil& Galil::operator=(const Galil& other) {
	if (this == &other) return *this;
	if (g != 0) Functions->GClose(g);
	if (Owner) delete Functions;

	Functions = new EmbeddedFunctions();
	g = 0;
	ControlParameters[0] = other.ControlParameters[0];
	ControlParameters[1] = other.ControlParameters[1];
	ControlParameters[2] = other.ControlParameters[2];
	setPoint = other.setPoint;

	Owner = true;
	LastWriteResponse = other.LastWriteResponse;
	LastWriteStatus = other.LastWriteStatus;
	CurrentValue = other.CurrentValue;

	Functions->GOpen("192.168.0.120 -d", &g);
	return *this;
}