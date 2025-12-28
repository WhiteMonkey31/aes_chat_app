#include "Network.h"
#include "Client.h"

namespace net {
	Client * client;
}

Network::Network(void * data) {
	net::client = (Client *)data;
}

void Network::Connect(const char * address, unsigned short port) {
	this->address = address;
	if (!isConnected) {
		auto ip = sf::IpAddress::resolve(address);
		if (ip && local_socket.connect(ip.value(), port) == sf::Socket::Status::Done) {
			isConnected = true;
		}
		else {
			isConnected = false;
		}
	}
}

void Network::DisconnectThread() {
	isConnected = false;
	local_socket.disconnect();
	receiver_thread.join();
	receiver_thread.~thread();
	logl("Disconnected and stopped the thread!");
}

void Network::Disconnect() {
	isConnected = false;
	local_socket.disconnect();
	logl("Disconnected!");
}


bool Network::Send(std::int8_t type, const std::string & data) {
	sf::Packet packet;
	
	// Encrypt only the data, keep type and username plaintext for server routing
	std::string encrypted_data;
	if (!encryption_key.empty()) {
		try {
			encrypted_data = Crypto::encrypt(data, encryption_key);
		} catch (const std::exception& e) {
			logl("Encryption error: " + std::string(e.what()));
			return false;
		}
	} else {
		encrypted_data = data;  // No encryption if key not set
	}
	
	packet << type << username << encrypted_data;

	if (packet.getDataSize() > 0) {
		if (local_socket.send(packet) == sf::Socket::Status::Done) return true;
		else {
			isConnected = false;
			return false;
		}
	}
	return false;
}

bool Network::Receive(sf::Packet & packet) {
	if (local_socket.receive(packet) == sf::Socket::Status::Done) return true;
	else {
		isConnected = false;
		return false;
	}
}

void Network::ReceivePackets(sf::TcpSocket * socket) {
	while (isConnected) {
		sf::Packet packet;
		if (socket->receive(packet) == sf::Socket::Status::Disconnected) {
			isConnected = false;
		}
		else {
			net::client->ProcessPacket(packet);
		}
	}
}

void Network::StartReceiving() {
	receiver_thread = std::thread(&Network::ReceivePackets, this, &local_socket);
}