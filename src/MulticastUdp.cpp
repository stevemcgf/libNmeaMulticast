/*
 * MulticastUdp.cpp
 *
 *  Created on: Jul 9, 2016
 *      Author: steve
 */

#include "MulticastUdp.h"
#include "MulticastUdpListener.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <boost/thread.hpp>
#include <boost/log/trivial.hpp>

using namespace boost;

const std::string DEFAULT_ADDRESS = "0.0.0.0";
const std::string MULTICAST_MASK = "224.0.0.0";
const int MAX_BUFFER_SIZE = 32768;

class MulticastUdp::impl {
public:
	int fd;
	bool active;

	sockaddr_in interface;
	sockaddr_in multicast;
	timeval timeout;

	thread listenerThread;
	std::shared_ptr<MulticastUdpListener> listener;

	char readBuffer[MAX_BUFFER_SIZE];
};

MulticastUdp::MulticastUdp(const MulticastUdp& obj) :
		pimpl { new impl { -1, false, obj.pimpl->interface,
				obj.pimpl->multicast, obj.pimpl->timeout } } {

}

MulticastUdp::MulticastUdp(const std::string& interfaceAddress,
		const std::string& multicastAddress, int multicastPort, int timeout) :
		pimpl { new impl } {

	pimpl->fd = -1;
	pimpl->active = false;

	pimpl->interface.sin_family = AF_INET;
	pimpl->interface.sin_port = htons(multicastPort);
	if (interfaceAddress != DEFAULT_ADDRESS) {
		inet_aton(interfaceAddress.c_str(), &pimpl->interface.sin_addr);
	} else {
		pimpl->interface.sin_addr.s_addr = INADDR_ANY;
	}
	memset(pimpl->interface.sin_zero, '\0', sizeof(pimpl->interface.sin_zero));

	pimpl->multicast.sin_family = AF_INET;
	pimpl->multicast.sin_port = htons(multicastPort);
	inet_aton(multicastAddress.c_str(), &pimpl->multicast.sin_addr);
	memset(pimpl->multicast.sin_zero, '\0', sizeof(pimpl->multicast.sin_zero));

	struct in_addr multicastmask;
	inet_aton(MULTICAST_MASK.c_str(), &multicastmask);

	if (((multicastmask.s_addr & 0x000000ff)
			& (pimpl->multicast.sin_addr.s_addr & 0x000000ff))
			!= (multicastmask.s_addr & 0x000000ff)) {
		BOOST_LOG_TRIVIAL(error) << "Dirección Multicast no válida.";
		throw std::invalid_argument(
				"Se debe especificar una dirección Multicast");
	}

	pimpl->timeout.tv_sec = timeout / 1000;
	pimpl->timeout.tv_usec = (timeout % 1000) * 1000;

	BOOST_LOG_TRIVIAL(info) << "MulticastUdp InterfaceAddress = " << interfaceAddress << " MulticastAddress = " << multicastAddress << ":" << multicastPort;

}

MulticastUdp::~MulticastUdp() {
	if (this->isOpen()) {
		this->close();
	}
}

bool MulticastUdp::open() {
	bool ret = false;

	BOOST_LOG_TRIVIAL(trace) << "MulticastUdp open()";

	if (pimpl->fd < 0) {
		int socket_temp = socket(AF_INET, SOCK_DGRAM, 0);
		if (socket_temp >= 0) {
			pimpl->fd = socket_temp;
			int yes = 1;

			if (setsockopt(pimpl->fd, SOL_SOCKET, SO_REUSEADDR, &yes,
					sizeof(yes)) != 0) {
				BOOST_LOG_TRIVIAL(error) << ("No se pudo establecer REUSEADDR");
			}

			int bindret;

			// Dirección mutlticast
			bindret = ::bind(pimpl->fd, (struct sockaddr *) &pimpl->multicast,
					sizeof(pimpl->multicast));

			if (bindret == 0) {

				struct ip_mreqn group;
				group.imr_address = pimpl->interface.sin_addr;
				group.imr_multiaddr = pimpl->multicast.sin_addr;
				group.imr_ifindex = 0;

				BOOST_LOG_TRIVIAL(debug) << "Configurando IP_ADD_MEMBERSHIP";
				if (setsockopt(pimpl->fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group,
						sizeof(group)) != 0) {
					BOOST_LOG_TRIVIAL(error) << "No se pudo suscribir a la direccion multicast";
				}

				BOOST_LOG_TRIVIAL(debug) << "Habilita IP_MULTICAST_LOOP";
				char loop = 1;
				if (setsockopt(pimpl->fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop,
						sizeof(loop)) != 0) {
					BOOST_LOG_TRIVIAL(error) << "No se pudo Habilitar loop";
				}

				uint rcvbuffer = MAX_BUFFER_SIZE;
				while (rcvbuffer > 1) {
					if (setsockopt(pimpl->fd, SOL_SOCKET, SO_RCVBUF, &rcvbuffer,
							sizeof(rcvbuffer)) == 0) {
						break;
					}
					rcvbuffer = rcvbuffer >> 1;
				}

				uint sndbuffer = MAX_BUFFER_SIZE;
				while (sndbuffer > 1) {
					if (setsockopt(pimpl->fd, SOL_SOCKET, SO_SNDBUF, &sndbuffer,
							sizeof(sndbuffer)) == 0) {
						break;
					}
					sndbuffer = sndbuffer >> 1;
				}

				BOOST_LOG_TRIVIAL(debug) << "UdpSocket::open rcvbuffer = " << rcvbuffer << "b sndbuffer = " << sndbuffer << "b";

				ret = true;
			} else {
				BOOST_LOG_TRIVIAL(error) << "Error al hacer bind '" << strerror(errno) << "'";
			}
		} else {
			BOOST_LOG_TRIVIAL(error) << "No se pudo crear socket";
		}
	} else {
		BOOST_LOG_TRIVIAL(error) << "Socket ya abierto";
	}

	if (false == ret && pimpl->fd >= 0) {
		BOOST_LOG_TRIVIAL(error) << "Error al abrir socket. Cerrando";
		::close(pimpl->fd);
		pimpl->fd = -1;
	}

	return ret;
}

bool MulticastUdp::close() {
	bool ret = false;

	BOOST_LOG_TRIVIAL(trace) << "UdpSocket close()";

	if (isOpen()) {
		if (pimpl->active) {
			stopListening();
		}
		::close(pimpl->fd);
		pimpl->fd = -1;
		ret = true;
		BOOST_LOG_TRIVIAL(debug) << "Socket cerrado";
	}

	return ret;
}

bool MulticastUdp::isOpen() {
	return (pimpl->fd >= 0);
}

int MulticastUdp::send(boost::asio::const_buffer buffer) {
	return ::sendto(pimpl->fd, asio::buffer_cast<const void *>(buffer),
			asio::buffer_size(buffer), 0, (struct sockaddr *) &pimpl->multicast,
			sizeof(pimpl->multicast));
}

int MulticastUdp::recv(asio::mutable_buffer buffer) {
	static fd_set readset;
	FD_ZERO(&readset);
	FD_SET(pimpl->fd, &readset);

	timeval timeout = pimpl->timeout;

	int ret = select(pimpl->fd + 1, &readset, NULL, NULL, &timeout);

	if (ret > 0) {
		ret = ::recvfrom(pimpl->fd, asio::buffer_cast<void *>(buffer),
				asio::buffer_size(buffer), 0, NULL, NULL);
	} else {
		ret = -2;
	}

	return ret;
}

void MulticastUdp::setListener(std::shared_ptr<MulticastUdpListener> listener) {
	pimpl->listener = listener;
}

void MulticastUdp::unsetListener() {
	pimpl->listener.reset();
}

void MulticastUdp::startListening() {
	BOOST_LOG_TRIVIAL(trace) << "MulticastUdp startListening()";
	if (!pimpl->active && pimpl->listener) {
		if (!isOpen()) {
			this->open();
		}
		pimpl->active = true;
		thread t(bind(&MulticastUdp::runListener, this));
		pimpl->listenerThread.swap(t);
		BOOST_LOG_TRIVIAL(debug) << "MulticastUdp: inició hilo";
	}
}

void MulticastUdp::stopListening() {
	BOOST_LOG_TRIVIAL(trace) << "MulticastUdp stopListening()";
	if (pimpl->active) {
		pimpl->active = false;
		pimpl->listenerThread.join();
		BOOST_LOG_TRIVIAL(debug) << "MulticastUdp: se liberó hilo";
	}
}

void MulticastUdp::runListener() {
	asio::mutable_buffer b(pimpl->readBuffer, MAX_BUFFER_SIZE);

	while (pimpl->active) {
		int ret = recv(b);

		if (ret > 0) {
			pimpl->listener->onDataAvailable(asio::buffer_cast<const char *>(b),
					ret);
		} else if (ret == -2) {
			pimpl->listener->onTimeout();
		} else {
			pimpl->listener->onConnectionError();
		}
	}

}
