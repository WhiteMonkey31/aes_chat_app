#pragma once
#include <thread>
#include <cstdint>
#include <string>

#undef Status
#undef None
#undef BadRequest
#include "SFML/Network.hpp"
#include "Crypto.h"

class Network {
	std::string address;
	sf::TcpSocket local_socket;
	std::thread receiver_thread;
	std::string encryption_key;
public:
	bool isConnected = false;
	std::string username;

	Network(void *);
	void Connect(const char *, unsigned short);
	void DisconnectThread();
	void Disconnect();
	void SetEncryptionKey(const std::string& key) { encryption_key = key; }
	const std::string& GetEncryptionKey() const { return encryption_key; }

	bool Send(std::int8_t, const std::string &);
	bool Receive(sf::Packet &);

	void ReceivePackets(sf::TcpSocket *);
	void StartReceiving();
};
