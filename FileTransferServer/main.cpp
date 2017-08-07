#include "Network.hpp"

void main()
{
	Network network;
	network.bind();
	network.acceptWithReceive();
}