#include "Client.h"
#include "Crypto.h"

int main(int agrc, char **argv)
{
	// Initialize crypto library
	if (!Crypto::init()) {
		std::cerr << "Failed to initialize crypto library" << std::endl;
		return 1;
	}
	
	Client client;
	client.Run();
	return Fl::run();
}
