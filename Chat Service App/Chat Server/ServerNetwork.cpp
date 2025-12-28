#include "ServerNetwork.h"
#include "Server.h"

Server * server;
ServerNetwork::ServerNetwork() {}

void ServerNetwork::Initialize(void * data) {
	server = (Server *)data;
}

void ServerNetwork::Listen(unsigned short port) {
	logl("Chat Server Started");
	if (listener.listen(port) != sf::Socket::Status::Done) logl("Could not listen");
}

void ServerNetwork::ConnectClients(std::vector<sf::TcpSocket *> * client_array) {
	while (true) {
		sf::TcpSocket * new_client = new sf::TcpSocket();
		if (listener.accept(*new_client) == sf::Socket::Status::Done) {
			new_client->setBlocking(false);
			client_array->push_back(new_client);
			{
				auto addrOpt = new_client->getRemoteAddress();
				std::string addrStr = addrOpt ? addrOpt->toString() : "unknown";
				logl("Added client " << addrStr << ":" << new_client->getRemotePort() << " on slot " << client_array->size());
			}
		}
		else {
			logl("Server listener error, restart the server");
			delete(new_client);
			break;
		}
	}
}

void ServerNetwork::DisconnectClient(sf::TcpSocket * socket_pointer, size_t position) {
	{
		auto addrOpt = socket_pointer->getRemoteAddress();
		std::string addrStr = addrOpt ? addrOpt->toString() : "unknown";
		logl("Client " << addrStr << ":" << socket_pointer->getRemotePort() << " disconnected, removing");
		if (addrOpt)
			server->SendNotification("User " + addrStr + " disconnected.", true, addrOpt.value(), socket_pointer->getRemotePort());
		else
			server->SendNotification("User unknown disconnected.", true, sf::IpAddress((std::uint32_t)0), socket_pointer->getRemotePort());
	}
	socket_pointer->disconnect();
	delete(socket_pointer);
	client_array.erase(client_array.begin() + position);
}

bool ServerNetwork::SendPacket(sf::TcpSocket * socket, std::int8_t type) {
	sf::Packet packet;
	packet << type;

	if (socket->send(packet) == sf::Socket::Status::Done) return true;
	else return false;
}

//Sends a packet to all clients excluding the one defined in exclude_address and port
void ServerNetwork::BroadcastPacket(sf::Packet & packet, sf::IpAddress exclude_address, unsigned short port) {
	if (packet.getDataSize() > 0) {
		for (size_t iterator = 0; iterator < client_array.size(); iterator++) {
			sf::TcpSocket * client = client_array[iterator];
			if (client->getRemoteAddress() != exclude_address || client->getRemotePort() != port) {
				if (client->send(packet) != sf::Socket::Status::Done) {
					logl("Could not send packet on broadcast");
				}
			}
		}
	}
}

//Sends a packet to every client
void ServerNetwork::BroadcastPacket(sf::Packet & packet) {
	if (packet.getDataSize() > 0) {
		for (size_t iterator = 0; iterator < client_array.size(); iterator++) {
			sf::TcpSocket * client = client_array[iterator];
			if (client->send(packet) != sf::Socket::Status::Done) {
				logl("Could not send packet on broadcast all");
			}
		}
	}
}

void ServerNetwork::ReceivePacket(sf::TcpSocket * client, size_t iterator) {
	sf::Packet packet;

	sf::Socket::Status status = client->receive(packet);
	if (status == sf::Socket::Status::Disconnected) {
		DisconnectClient(client, iterator);
	}
	else if (status == sf::Socket::Status::Done) {
		server->ProcessPacket(client, packet);
	}
}

//This is executed on a different thread, it will receive incoming packets
void ServerNetwork::ManagePackets() {
	while (true) {
		for (size_t iterator = 0; iterator < client_array.size(); iterator++) ReceivePacket(client_array[iterator], iterator);
		std::this_thread::sleep_for((std::chrono::milliseconds)10);
	}
}

void ServerNetwork::Run() {
	std::thread connetion_thread(&ServerNetwork::ConnectClients, this, &client_array);
	ManagePackets();
}