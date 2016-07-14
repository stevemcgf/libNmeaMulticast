/*
 * test.cpp
 *
 *  Created on: Jul 14, 2016
 *      Author: steve
 */


#include <iostream>

#include "NmeaMulticastUdp.h"
#include "NmeaMulticastUdpListener.h"

class Helper : public NmeaMulticastUdpListener {
    virtual void onStringAvailable(const std::string& sourceId, const std::string& nmea)
    {
    	std::cout << "onStringAvailable : sourceId='" << sourceId << "' nmea= '" << nmea << "'" << std::endl;
    }
    virtual void onTimeout()
    {
    	std::cout << "onTimeout" << std::endl;
    }
    virtual void onConnectionError()
    {
    	std::cout << "onConnectionError" << std::endl;
    }
    virtual void onChecksumError()
    {
    	std::cout << "onChecksumError" << std::endl;
    }

};

int main(int argc, char* argv[]) {
	NmeaMulticastUdp multicast(NmeaTransmissionGroup_NAVD);

	multicast.setListener(std::make_shared<Helper>());

	multicast.startListening();

	char ch;
	std::cin >> ch;
}


