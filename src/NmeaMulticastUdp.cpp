/**
 *	@file NmeaMulticastUdp.cpp
 *	@brief Implementation of the generic MulticastUdp class
 */

#include "NmeaMulticastUdp.h"

#include "NmeaMulticastUdpListener.h"

#include "MulticastUdp.h"

#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/thread.hpp>
#include <boost/log/trivial.hpp>

#ifdef NM_DEBUG
#define LOG_MESSAGE(lvl) BOOST_LOG_TRIVIAL(lvl)
#else
#define LOG_MESSAGE(lvl) if (false) BOOST_LOG_TRIVIAL(lvl)
#endif

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
		NmeaTransmissionGroup_PROP, { "239.192.0.8", 60008 } }, {
		NmeaTransmissionGroup_USR1, { "239.192.0.9", 60009 } }, {
		NmeaTransmissionGroup_USR2, { "239.192.0.10", 60010 } }, {
		NmeaTransmissionGroup_USR3, { "239.192.0.11", 60011 } }, {
		NmeaTransmissionGroup_USR4, { "239.192.0.12", 60012 } }, {
		NmeaTransmissionGroup_USR5, { "239.192.0.13", 60013 } }, {
		NmeaTransmissionGroup_USR6, { "239.192.0.14", 60014 } }, {
		NmeaTransmissionGroup_USR7, { "239.192.0.15", 60015 } }, {
		NmeaTransmissionGroup_USR8, { "239.192.0.16", 60016 } } };

using namespace boost;

const int defaultTimeout = 1000;
const int multicastBufferSize = 4096;
const int nmeaStringMaxSize = 2048;

char DatagramHeader[6] = { 'U', 'd', 'P', 'b', 'C', '\0' };

class NmeaMulticastUdp::impl {
public:
	bool active;

	std::shared_ptr<MulticastUdp> multicast;

	std::unordered_map<std::string, int> messageCounter;

	thread listenerThread;
	std::shared_ptr<NmeaMulticastUdpListener> listener;

	char readbuffer[multicastBufferSize];
	char writebuffer[multicastBufferSize];
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

void NmeaMulticastUdp::registerSystemId(const std::string& sourceId) {
	pimpl->messageCounter[sourceId] = 1;
}

bool NmeaMulticastUdp::sendString(const std::string& sourceId,
		const std::string& nmea) {
	int pointer = 0;
	memcpy(&pimpl->writebuffer[pointer], DatagramHeader,
			sizeof(DatagramHeader));
	pointer += sizeof(DatagramHeader);

	int& messageCounter = pimpl->messageCounter[sourceId];

	std::stringstream tagBlockss;
	tagBlockss << "s:" << sourceId;
	tagBlockss << ",n:" << messageCounter;

	++messageCounter;
	if (messageCounter == 1000) {
		messageCounter = 1;
	}

	int16_t checksum = calculateNmeaChecksum(tagBlockss.str());

	tagBlockss << "*" << std::hex << std::uppercase << std::setfill('0')
			<< std::setw(2) << checksum;

	std::string tagBlock = tagBlockss.str();

	pimpl->writebuffer[pointer] = '\\';
	++pointer;

	for (auto c : tagBlock) {
		pimpl->writebuffer[pointer] = c;
		++pointer;
	}

	pimpl->writebuffer[pointer] = '\\';
	++pointer;

	for (auto c : nmea) {
		pimpl->writebuffer[pointer] = c;
		++pointer;
	}

	pimpl->writebuffer[pointer] = '\r';
	++pointer;
	pimpl->writebuffer[pointer] = '\n';
	++pointer;

	return (pimpl->multicast->send(pimpl->writebuffer, pointer) > 0);
}

bool NmeaMulticastUdp::recvString(std::string& sourceId, std::string& nmea) {
	bool ret = false;

	int len = pimpl->multicast->recv(pimpl->readbuffer, multicastBufferSize);

	if (len > 0) {
		if (memcmp(pimpl->readbuffer, DatagramHeader, sizeof(DatagramHeader))
				== 0) {
			ret = true;
			std::string nmeaBlocks(&pimpl->readbuffer[sizeof(DatagramHeader)],
					len - sizeof(DatagramHeader));
			const boost::char_separator<char> sep("\\\r\n");
			const boost::tokenizer<boost::char_separator<char>> t(
					nmeaBlocks.begin(), nmeaBlocks.end(), sep);

			// FIXME: Se asume que la cadena es "s:ID0001,n:11*cc"
			boost::tokenizer<boost::char_separator<char>>::iterator itToken =
					t.begin();
			sourceId = (*itToken).substr(2, 6);
			++itToken;
			nmea = (*itToken);
		}
	}
	return ret;
}

void NmeaMulticastUdp::setListener(
		std::shared_ptr<NmeaMulticastUdpListener> listener) {
	pimpl->listener = listener;
}

void NmeaMulticastUdp::unsetListener() {
	pimpl->listener.reset();
}

bool NmeaMulticastUdp::startListening() {
	LOG_MESSAGE(trace)<< "NmeaMulticastUdp::startListening >>>>";
	bool ret = false;

	if (!pimpl->active && pimpl->listener) {
		if (pimpl->multicast->open()) {
			pimpl->active = true;
			ret = true;

			thread t(bind(&NmeaMulticastUdp::runListener, this));
			pimpl->listenerThread.swap(t);
			LOG_MESSAGE(debug) << "NmeaMulticastUdp::startListening se inicia hilo";
		}
	}
	LOG_MESSAGE(trace) << "NmeaMulticastUdp::startListening <<<<";
	return ret;
}

void NmeaMulticastUdp::stopListening() {
	LOG_MESSAGE(trace)<< "NmeaMulticastUdp::stopListening >>>>";
	if (pimpl->active) {
		pimpl->active = false;
		pimpl->listenerThread.join();
		pimpl->multicast->close();
		LOG_MESSAGE(debug) << "NmeaMulticastUdp::stopListening: se liberó hilo";
	}
	LOG_MESSAGE(trace) << "NmeaMulticastUdp::stopListening <<<<";
}

void NmeaMulticastUdp::runListener() {
	std::string sourceId;
	std::string nmeaStr;

	while (pimpl->active) {
		if (recvString(sourceId, nmeaStr)) {
			pimpl->listener->onStringAvailable(sourceId, nmeaStr);
		} else {
			pimpl->listener->onTimeout();
		}
	}

}

int16_t NmeaMulticastUdp::calculateNmeaChecksum(const std::string& nmeaStr) {
	int16_t checksum = 0;
	for (auto c : nmeaStr) {
		checksum ^= c;
	}
	return checksum;
}
