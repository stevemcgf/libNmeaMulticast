/*
 * NmeaMulticastUdp.h
 *
 *  Created on: Jul 10, 2016
 *      Author: steve
 */

#ifndef SRC_NMEAMULTICASTUDP_H_
#define SRC_NMEAMULTICASTUDP_H_

#include <memory>
#include <string>

enum NmeaTrasmissionGroupEnum
{
	NmeaTransmissionGroup_MISC,
	NmeaTransmissionGroup_TGTD,
	NmeaTransmissionGroup_SATD,
	NmeaTransmissionGroup_NAVD,
	NmeaTransmissionGroup_VDRD,
	NmeaTransmissionGroup_RCOM,
	NmeaTransmissionGroup_TIME,
	NmeaTransmissionGroup_PROP
};

class NmeaMulticastUdpListener;

class NmeaMulticastUdp {
public:
	NmeaMulticastUdp(const NmeaMulticastUdp& obj);
	NmeaMulticastUdp(NmeaTrasmissionGroupEnum transmissionGroup);
	virtual ~NmeaMulticastUdp();

	bool open();
	bool close();
	bool isOpen();

	void registerSystemId(const std::string& sourceId);

	bool sendString(const std::string& sourceId, const std::string& nmea);
	bool recvString(std::string& sourceId, std::string& nmea);

    void setListener(std::shared_ptr<NmeaMulticastUdpListener> listener);
    void unsetListener();

    bool startListening();
    void stopListening();

private:
    class impl;
    std::unique_ptr<impl> pimpl;

    void runListener();

};

#endif /* SRC_NMEAMULTICASTUDP_H_ */
