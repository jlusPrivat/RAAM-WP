# Server Commands
This guide describes the usage of the RAAM-WP tcp socket server.
It therefore helps implementing applications, that employs the network
functionalities provided by this program.

**Note, that any application using the RAAM server commands, partially or as a whole, must be licensed under GNU General Public License v3.0**

**Note, that all specs descriped here are still pending and should not be used for development. This line will be removed, as soon, as the specs are ready to use.**

## Pairing Client and Server
When a client (like Smartphone) wants to connect to a server (like Windows PC),
it has to retrieve the information needed to connect as an authenticated client.
It is easiest to get those information through scanning the qr code. Alternatively
a client should provide an interface to manually input these data.
The client can then save those connection information and use them to connect at any time.

### Reading the QR Code
Any suitable error correction level is allowed.  
The QR Code contains these information in this exact order.  
* Server ID (Null terminated ascii character string; Max 25 characters; RegEx: [a-zA-Z][a-zA-Z0-9 _\-]{1,23}[a-zA-Z0-9])
* Server Software (Null terminated, ascii character string representing the name of the software e.g. "RAAM-WP")
* Server software version. (Null terminated, ascii character string; Semantic Versioning)
* Server IP adress (4x 8 bit unsigned integer in left-to-right order; Big Endian style; Only IPv4)
* Server port (unsigned 16 bit integer, range 1024..65535)
* Client ID (Null terminated ascii character string; Max 25 characters; RegEx: [a-z][a-z0-9 _\\-]{1,23}[a-z0-9])
* Client specific hmac shared secret (64 Bytes)


## Establishing a connection
### About TCP/BLE messages
* Following specifications describe both ways of communication.
* TCP / BLE messages consist of Null terminated 2 byte Wide Character Strings. (Null termination before HMAC)
* Every TCP/BLE message is key-value based in form of: `key1="value1";key2="value2"` for every type of value. Exceptions are error messages.
* Values containing a `"` character are prohibited and lead to the message being rejected.
* Every Message has to have the 32 bytes of the HMAC (using sha256) appended in raw binary form. The HMAC is generated from the entire message, excluding only the HMAC itself. If the HMAC is wrong, "WRONG_HMAC" will be returned. This is optional, when debug mode is enabled.
* Rejected messages will be fully ignored, unless a specific error message is returned.
* Furhter rules regarding the messages must be extracted from the following section.


## About Keys and Actions
### Overview of all actions
* **init**: Must be issued by the client within 10 seconds after establishing a transport layer connection using the servers IP adress and port. *!!! No further description yet*
* **unknown**: This action is sent, when any correspondent does 
* **close**: *!!! No description yet*
* **changeServerId**: *!!! No description yet*
* **changeClientId**: *!!! No description yet*
* **info**: *!!! No description yet*
* **devices**: *!!! No description yet*
* **sessions**: *!!! No description yet*

### Overview of all possible keys
Key | Description
--- | -----------
`t` | "t" for "time". Must always be sent. The current unix timestamp in seconds. This is used to ensure "freshness" of the data being integrity checked with the hmac. If the received timestamp differs from the local timestamp by more than 30 seconds, "INVALID_TIMESTAMP" will be returned and the connection closed. Applications must keep track of the last sent timestamp and check, that no message with an earlier timestamp is allowed. Messages with earlier timestamps will be ignored.
`a` | "a" for "action". Must always be sent. Possible values see above.
`c` | "c" for "client". Must always be sent by client. Must be Client ID as provided by the server upon pairing. If the client is unknown to the server, "CLIENT_UNKNOWN" will be returned and the connection closed. If the client is disabled, "CLIENT_DISABLED" will be returned and the connection closed.
`s` | "s" for "server id". Must always be sent by server. If the server is unknown to the client, "SERVER_UNKNOWN" will be returned and the connection closed.
`sw` | "sw" for "software". Is the name of the software sending this string. For example: "RAAM-WP"
`v` | "v" for "version". Is the RAAM software version as semver string of the sender.
