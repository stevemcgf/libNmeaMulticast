/**
*	@file NmeaMulticastUdp.h
*	@brief Header file for NmeaMulticastUdp class
*/

#ifndef SRC_NMEAMULTICASTUDP_H_
#define SRC_NMEAMULTICASTUDP_H_

#include <memory>
#include <string>

/**
 * @brief NMEA transmission group indicator. Used in NmeaMulticastUdp constructor.
 *
 * NMEA transmission group define a pair of multicast address and port number.
 *
 */
enum NmeaTrasmissionGroupEnum
{
	NmeaTransmissionGroup_MISC, ///< SF not explicitly listed.
	NmeaTransmissionGroup_TGTD, ///< Target data (AIS), tracked target messages (Radar).
	NmeaTransmissionGroup_SATD, ///< High update rate, for example ship heading, attitude data.
	NmeaTransmissionGroup_NAVD, ///< Navigational output other than that of TGTD and SATD groups.
	NmeaTransmissionGroup_VDRD, ///< Data required for the VDR according to IEC 61996.
	NmeaTransmissionGroup_RCOM, ///< Radio communication equipment.
	NmeaTransmissionGroup_TIME, ///< Time transmitting equipment.
	NmeaTransmissionGroup_PROP, ///< Proprietary and user specified SFs.
	NmeaTransmissionGroup_USR1, ///< User defined transmission group 1.
	NmeaTransmissionGroup_USR2, ///< User defined transmission group 2.
	NmeaTransmissionGroup_USR3, ///< User defined transmission group 3.
	NmeaTransmissionGroup_USR4, ///< User defined transmission group 4.
	NmeaTransmissionGroup_USR5, ///< User defined transmission group 5.
	NmeaTransmissionGroup_USR6, ///< User defined transmission group 6.
	NmeaTransmissionGroup_USR7, ///< User defined transmission group 7.
	NmeaTransmissionGroup_USR8  ///< User defined transmission group 8.
};

class NmeaMulticastUdpListener;

/**
 * @brief NmeaMulticastUdp class implements Nmea Ethernet protocol.
 *
 * NmeaMulticastUdp class implements NMEA Ethernet interconnection as defined in EN 61162-450:2011.
 * "Maritime navigation and radiocommunication equipment and systems - Digital interfaces - Part 450: Multiple talkers and multiple listeners - Ethernet interconnection."
 *
 */
class NmeaMulticastUdp {
public:
	/**
	 * @brief Copy constructor
	 *
	 * Copies class configuration. Connections are not copied, the object initial state is "closed".
	 *
	 * @param [in] obj Object to copy configuration.
	 */
	NmeaMulticastUdp(const NmeaMulticastUdp& obj);

	/**
	 * @brief Constructor
	 *
	 * Create a new NmeaMulticastUdp object.
	 *
	 * @param [in] transmissionGroup Transmission group used to talk and/or listen. See enumeration NmeaTrasmissionGroupEnum.
	 */
	NmeaMulticastUdp(NmeaTrasmissionGroupEnum transmissionGroup);

	/**
	 * @brief Destructor
	 */
	virtual ~NmeaMulticastUdp();

	/**
	 * @brief Open UDP communication socket
	 *
	 * Opens the multicast UDP communication socket for the transmission group. In case of failure returns false.
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
	 * @brief Register a Source Id
	 *
	 * Register a System Id to be able to keep the messages counter when sending messages.
	 *
	 * @param sourceId
	 */
	void registerSystemId(const std::string& sourceId);

	/**
	 * @brief Send NMEA String to the transmission group
	 *
	 * @param [in] sourceId Source Id to wrap around the NMEA sentence.
	 * @param [in] nmea NMEA sentence to send.
	 *
	 * @return True on success, False on failure.
	 */
	bool sendString(const std::string& sourceId, const std::string& nmea);

	/**
	 * @brief Receive NMEA String from the transmission group
	 *
	 * @param [out] sourceId Source Id for the NMEA sentence.
	 * @param [out] nmea Received NMEA sentence
	 *
	 * @return True on success, False on failure.
	 */
	bool recvString(std::string& sourceId, std::string& nmea);

	/**
	 * @brief Set listener object.
	 *
	 * Set the listener object to receive messages from the transmission group using the listening thread.
	 *
	 * @param listener Smart pointer to the listener object.
	 */
    void setListener(std::shared_ptr<NmeaMulticastUdpListener> listener);

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
    bool startListening();

	/**
	 * @brief Stops the listening thread.
	 */
    void stopListening();

private:
    class impl;
    std::unique_ptr<impl> pimpl;

    void runListener();

    static int16_t calculateNmeaChecksum(const std::string& nmeaStr);

};

#endif /* SRC_NMEAMULTICASTUDP_H_ */
