/**
*	@file NmeaMulticastUdpListener.h
*	@brief Header file for NmeaMulticastUdpListener class
*/

#ifndef SRC_NMEAMULTICASTUDPLISTENER_H_
#define SRC_NMEAMULTICASTUDPLISTENER_H_

/**
 * @brief Interface class for listening to NmeaMulticastUdp
 *
 * Interface class for listening to NmeaMulticastUdp events.
 */
class NmeaMulticastUdpListener {
public:
	/**
	 * Destructor
	 */
	virtual ~NmeaMulticastUdpListener();

	/**
	 * @brief On string available event.
	 *
	 * Called by NmeaMulticastUdp class when a valid string arrives.
	 *
	 * @param [in] sourceId Source Id from arriving message.
	 * @param [in] nmea NMEA string from arriving message.
	 */
    virtual void onStringAvailable(const std::string& sourceId, const std::string& nmea) = 0;

    /**
     * @brief Timeout event.
     *
     * Called by NmeaMulticastUdp class when a timeout occurs. The timeout value is set on MulticastUdp constructor.
     *
     */
    virtual void onTimeout() = 0;

    /**
     * @brief On connection error event.
     *
     * Called by NmeaMulticastUdp class when an error is reported by the socket.
     */
    virtual void onConnectionError() = 0;

    /**
     * @brief On Checksum error event.
     *
     * Called by NmeaMulticastUdp class when a string arrives but the checksum does not match.
     */
    virtual void onChecksumError() = 0;
};

inline NmeaMulticastUdpListener::~NmeaMulticastUdpListener() { };

#endif /* SRC_NMEAMULTICASTUDPLISTENER_H_ */
