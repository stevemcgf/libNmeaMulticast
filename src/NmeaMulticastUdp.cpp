/*
 * NmeaMulticastUdp.cpp
 *
 *  Created on: Jul 10, 2016
 *      Author: steve
 */

#include "NmeaMulticastUdp.h"

#include "MulticastUdp.h"

#include <unordered_map>
#include <boost/thread.hpp>

#include "debug.hpp"
#include "error.hpp"

struct EnumClassHash {
	template<typename T>
	std::size_t operator()(T t) const {
		return static_cast<std::size_t>(t);
	}
};

std::unordered_map<NmeaTrasmissionGroupEnum, std::pair<std::string, int>,
		EnumClassHash> NmeaTrasmissionGroupMap = { { NmeaTransmissionGroup_MISC,
		{ "239.192.0.1", 60001 } }, { NmeaTransmissionGroup_TGTD, {
		"239.192.0.2", 60002 } }, { NmeaTransmissionGroup_SATD, { "239.192.0.3",
		60003 } }, { NmeaTransmissionGroup_NAVD, { "239.192.0.4", 60004 } }, {
		NmeaTransmissionGroup_VDRD, { "239.192.0.5", 60005 } }, {
		NmeaTransmissionGroup_RCOM, { "239.192.0.6", 60006 } }, {
		NmeaTransmissionGroup_TIME, { "239.192.0.7", 60007 } }, {
		NmeaTransmissionGroup_PROP, { "239.192.0.8", 60008 } } };

using namespace boost;

const int defaultTimeout = 1000;
const int readBufferSize = 4096;
const int nmeaStringMaxSize = 2048;

char DatagramHeader[6] = {'U', 'd', 'P', 'b', 'C', '\0'};

class NmeaMulticastUdp::impl {
public:
	bool active;

	std::shared_ptr<MulticastUdp> multicast;

	thread listenerThread;
	std::shared_ptr<MulticastUdpListener> listener;

	char readbuffer[readBufferSize];
	char nmeaBuffer[nmeaStringMaxSize + 16];
	char writebuffer[nmeaStringMaxSize + 16];
};

NmeaMulticastUdp::NmeaMulticastUdp(const NmeaMulticastUdp& obj) :
		pimpl { new impl } {
	pimpl->active = false;
	pimpl->multicast = std::make_shared<MulticastUdp>(*obj.pimpl->multicast);
}

NmeaMulticastUdp::NmeaMulticastUdp(NmeaTrasmissionGroupEnum transmissionGroup) :
		pimpl { new impl } {
	pimpl->active = false;
	pimpl->multicast = std::make_shared<MulticastUdp>(std::string("0.0.0.0"),
			NmeaTrasmissionGroupMap[transmissionGroup].first,
			NmeaTrasmissionGroupMap[transmissionGroup].second, defaultTimeout);

}

NmeaMulticastUdp::~NmeaMulticastUdp() {
	stopListening();
}

bool NmeaMulticastUdp::open() {
	return pimpl->multicast->open();
}

bool NmeaMulticastUdp::close() {
	return pimpl->multicast->close();
}

bool NmeaMulticastUdp::isOpen() {
	return pimpl->multicast->isOpen();
}

bool NmeaMulticastUdp::sendString(const std::string& sourceId, const std::string& nmea)
{

}

bool NmeaMulticastUdp::recvString(std::string& sourceId, std::string& nmea)
{

}
