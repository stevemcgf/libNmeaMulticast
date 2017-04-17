/**
*	@file MulticastUdpListener.h
*	@brief Header for the MulticastUdpListener class
*/

#ifndef SRC_MULTICASTUDPLISTENER_H_
#define SRC_MULTICASTUDPLISTENER_H_

/**
 * @brief Interface class for listening to MulticastUdp
 *
 * Interface class for listening to MulticastUdp events.
 */
class MulticastUdpListener {
public:
	/**
	 * Destructor
	 */
	virtual ~MulticastUdpListener();

	/**
	 * @brief On data available event.
	 *
	 * Called by MulticastUdp class when data arrives.
	 *
	 * @param [in] data Buffer pointing to the received data.
	 * @param [in] size Number of bytes received.
	 */
    virtual void onDataAvailable(const char *data, size_t size) = 0;

    /**
     * @brief Timeout event.
     *
     * Called by MulticastUdp class when a timeout occurs. The timeout value is set on MulticastUdp constructor.
     *
     */
    virtual void onTimeout() = 0;

    /**
     * @brief On connection error event.
     *
     * Called by MulticastUdp class when an error is reported by the socket.
     */
    virtual void onConnectionError() = 0;
};

inline MulticastUdpListener::~MulticastUdpListener() { };

#endif /* SRC_MULTICASTUDPLISTENER_H_ */
