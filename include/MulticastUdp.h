/**
*	@file MulticastUdp.h
*	@brief Header for the generic MulticastUdp class
*/

#ifndef SRC_MULTICASTUDP_H_
#define SRC_MULTICASTUDP_H_

#include <string>
#include <memory>

class MulticastUdpListener;

/**
 * @brief MulticastUdp allows multicast UDP communication.
 *
 * This class allows to send and receive messages using Multicast UDP.
 * Can be use in two ways:
 * 1. With a Listener. The class creates a receiving thread and calls the Listener at each event.
 * 2. With recv() you call and wait for data. Recv will block at most the specified Timeout period.
 *
 */
class MulticastUdp {
public:
	/**
	 * @brief Copy constructor
	 *
	 * Copies class configuration. Connections are not copied, the object initial state is "closed".
	 *
	 * @param [in] obj Object to copy configuration.
	 */
	MulticastUdp(const MulticastUdp& obj);

	/**
	 * @brief Constructor
	 *
	 * Create a new Multicast communication object.
	 *
	 * @param [in] interfaceAddress Address assigned to the interface to be used to send and receive multicast messages. Can be default 0.0.0.0 to use the interface based on ip route.
	 * @param [in] multicastAddress Multicast address to send and receive messages.
	 * @param [in] multicastPort UDP port for communication.
	 * @param [in] timeout Communication timeout.
	 */
	MulticastUdp(const std::string& interfaceAddress,
			const std::string& multicastAddress, int multicastPort,
			int timeout);

	/**
	 * @brief Destructor
	 */
	virtual ~MulticastUdp();

	/**
	 * @brief Open UDP communication socket
	 *
	 * Opens the multicast UDP communication socket. In case of failure returns false.
	 *
	 * @return True on success, false on failure or if socket was already open.
	 */
	bool open();

	/**
	 * @brief Close UDP socket.
	 *
	 * Close the multicast UDP communication socket it also stops the listening thread.
	 *
	 * @return True on success, false if socket was already closed.
	 */
	bool close();

	/**
	 * @brief Verify if the socket is open.
	 *
	 * @return True if the socket is open. False if the socket is closed.
	 */
	bool isOpen();

	/**
	 * @brief Send data through the UDP Multicast socket
	 *
	 * @param [in] buffer Pointer to the binary message to send.
	 * @param [in] size Size of the pointed buffer.
	 *
	 * @return On success, returns the number of bytes sent. On error, -1 is returned.
	 */
	int send(const void* buffer, std::size_t size);

	/**
	 * @brief Receive data from the UDP Multicast socket
	 *
	 * @param [out] buffer Pointer to the binary buffer to receive the message.
	 * @param [in] size Size of the pointed buffer.
	 *
	 * @return On success, number of bytes received. On error, -1. On timeout, -2.
	 */
	int recv(void* buffer, std::size_t size);

	/**
	 * @brief Set listener object.
	 *
	 * Set the listener object to receive messages from the UDP socket using the listening thread.
	 *
	 * @param listener Smart pointer to the listener object.
	 */
	void setListener(std::shared_ptr<MulticastUdpListener> listener);

	/**
	 * @brief Unset listener object.
	 *
	 * Clear the listener pointer assignment.
	 *
	 */
	void unsetListener();

	/**
	 * @brief Starts the listening thread.
	 */
	void startListening();

	/**
	 * @brief Stops the listening thread.
	 */
	void stopListening();

private:
	class impl;
	std::unique_ptr<impl> pimpl;

	void runListener();

};

#endif /* SRC_MULTICASTUDP_H_ */
