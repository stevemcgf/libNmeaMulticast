/*
 * MulticastUdp.h
 *
 *  Created on: Jul 9, 2016
 *      Author: steve
 */

#ifndef SRC_MULTICASTUDP_H_
#define SRC_MULTICASTUDP_H_

#include <string>
#include <memory>
#include <boost/asio/buffer.hpp>

class MulticastUdpListener;

class MulticastUdp {
public:
	MulticastUdp(const MulticastUdp& obj);
	MulticastUdp(const std::string& interfaceAddress,
			const std::string& multicastAddress, int multicastPort,
			int timeout);
	virtual ~MulticastUdp();

	bool open();
	bool close();
	bool isOpen();

	int send(boost::asio::const_buffer buffer);
	int recv(boost::asio::mutable_buffer buffer);

	void setListener(std::shared_ptr<MulticastUdpListener> listener);
	void unsetListener();

	void startListening();
	void stopListening();

private:
	class impl;
	std::unique_ptr<impl> pimpl;

	void runListener();

};

#endif /* SRC_MULTICASTUDP_H_ */
