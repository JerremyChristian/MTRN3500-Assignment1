#include "EmbeddedFunctions.h"

EmbeddedFunctions::EmbeddedFunctions() : 
	client(nullptr), 
	stream(nullptr) 
{}

EmbeddedFunctions::~EmbeddedFunctions() {
	GClose();
}

void EmbeddedFunctions::GOpen(String^ address, const int port) {
	address = address->Replace(" -d", "")->Trim();

	client = gcnew TcpClient();
	client->Connect(address, port);
	
	stream = client->GetStream();
	stream->ReadTimeout = 2000;
	stream->WriteTimeout = 2000;
}

void EmbeddedFunctions::GClose() {
	if (stream != nullptr) {
		stream->Close();
		stream = nullptr;
	}
	if (client != nullptr) {
		client->Close();
		client = nullptr;
	}
}

String^ EmbeddedFunctions::GCommand(String^ command) {
	if (stream == nullptr) throw gcnew InvalidOperationException("Not connected to Galil");
	if (!command->EndsWith(";")) command += ";";

	command += "\r";
	array<Byte>^ sendData = Text::Encoding::ASCII->GetBytes(command);
	stream->Write(sendData, 0, sendData->Length);

	array<Byte>^ receiveData = gcnew array<Byte>(1024);
	String^ response = "";

	while (!response->Trim()->EndsWith(":") && !response->Trim()->EndsWith("?")) {
		int bytesRead = stream->Read(receiveData, 0, receiveData->Length);
		
		if (bytesRead == 0) {
			throw gcnew InvalidOperationException("Galil connection closed");
		}
		response += Text::Encoding::ASCII->GetString(receiveData, 0, bytesRead);
	}
	return response;
}