#include "EmbeddedFunctions.h"
#include "iostream"

int main(void) {

	EmbeddedFunctions^ Galil;
	String^ IPAddress = "192.168.0.120";
	int Port = 23;
	String^ Command;
	String^ Response;

	try {
		Galil = gcnew EmbeddedFunctions();
		Galil->GOpen(IPAddress, Port);
		Console::WriteLine("Connected to " + IPAddress);

		Command = "AO 0,0.0";                   //Turn on outputs 0-7 (no ';', GCommand should add it).
		Response = Galil->GCommand(Command);    //Sends the command, waits for the reply.
		Console::WriteLine(Command + " -> Expected: (empty) | Got: \"" + Response + "\"");

	}
	catch (Exception^ e) {
		Console::WriteLine("ERROR: " + e->Message);   //Prints any error (connection, timeout, rejected command).
	}

	if (Galil != nullptr) {
		Galil->GClose();                        //Disconnects cleanly.
		Console::WriteLine("Connection closed.");
	}
	std::cin.get();
	return 0;
}