/*
 * MulticastUdpListener.h
 *
 *  Created on: Jul 10, 2016
 *      Author: steve
 */

#ifndef SRC_MULTICASTUDPLISTENER_H_
#define SRC_MULTICASTUDPLISTENER_H_

#include <boost/asio/buffer.hpp>

class MulticastUdpListener {
public:
	virtual ~MulticastUdpListener();
    virtual void onDataAvailable(const char *data, size_t size) = 0;
    virtual void onTimeout() = 0;
    virtual void onConnectionError() = 0;
};

inline MulticastUdpListener::~MulticastUdpListener() { };

#endif /* SRC_MULTICASTUDPLISTENER_H_ */
