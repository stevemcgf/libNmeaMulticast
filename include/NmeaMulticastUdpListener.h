/*
 * NmeaMulticastUdpListener.h
 *
 *  Created on: Jul 10, 2016
 *      Author: steve
 */

#ifndef SRC_NMEAMULTICASTUDPLISTENER_H_
#define SRC_NMEAMULTICASTUDPLISTENER_H_


class NmeaMulticastUdpListener {
public:
	virtual ~NmeaMulticastUdpListener();
    virtual void onStringAvailable(const std::string& sourceId, const std::string& nmea) = 0;
    virtual void onTimeout() = 0;
    virtual void onConnectionError() = 0;
    virtual void onChecksumError() = 0;
};

inline NmeaMulticastUdpListener::~NmeaMulticastUdpListener() { };

#endif /* SRC_NMEAMULTICASTUDPLISTENER_H_ */
