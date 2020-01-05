# Server Command Guide
This guide describes the usage of the RAAM-WP tcp socket server.
It therefore helps implementing applications, that employs the network
functionalities provided by this program.


## Pairing Client and Server
When a client (like Smartphone) wants to connect to a server (like Windows PC),
it has to retrieve the information needed to connect as an authenticated client.
It is easiest to get those information through scanning the qr code. Alternatively
a client should provide an interface to manually input these data.
The client can then save those connection information and use them to connect at any time.

### Reading the QR Code
*!!! QR Code not finalized yet: Still pending*  
The QR Code contains these information in this exact order.  
* Server Name (Null terminated ascii character string; Max 100 characters; RegEx: [a-zA-Z][a-zA-Z0-9 _\-]{1,23}[a-zA-Z0-9])
* Server Software (Null terminated, ascii character string representing the name of the software e.g. "RAAM-WP")
* Server RAAM software version. (Null terminated, ascii character string; Semantic Versioning)
* Server IP adress (4x ascii bit integer in left-to-right order; Big Endian style)
* Server port (unsigned 16 bit integer, range 1024..65535)
* Client ID (Null terminated ascii character string; Max 100 characters; RegEx: [a-z][a-z0-9 _\\-]{1,23}[a-z0-9])
* Client specific hmac shared secret (64 Bytes; Big Endian)


## Establishing a connection
### About TCP messages
* Following specifications describe both ways of communication.
* TCP messages consist of Null terminated 2 byte Wide Character Strings.
* Every tcp message is key-value based in form of: `key1="value1";key2="value2"` for every type of value. Exceptions are error messages.
* Values containing a `"` character are prohibited and lead to the message being rejected.
* Every Message has to have the 32 bytes of the HMAC (using sha256) appended in raw binary form. The HMAC is generated from the entire message, excluding only the HMAC itself. If the HMAC is wrong, "WRONG_HAMAC" will be returned.
* Rejected messages will be fully ignored, unless a specific error message is returned.

### About connecting
1. Connect to the server using its IP adress and the correct port
2. *!!! No further specification yet*


## About Keys and Actions
### Overview of all actions
* **init**: *!!! No description yet*
* **close**: *!!! No description yet*
* **changeServerId**: *!!! No description yet*
* **changeClientId**: *!!! No description yet*
* **info**: *!!! No description yet*

### Overview of all possible keys
Key | Description
--- | -----------
`t` | "t" for "time". Must always be sent. The current unix timestamp in seconds. This is used to ensure "freshness" of the data being integrity checked with the hmac. If the received timestamp differs from the local timestamp by more than 30 seconds, "INVALID_TIMESTAMP" will be returned and the connection closed. Applications must keep track of the last sent timestamp and check, that no message with an earlier timestamp is allowed. Messages with earlier timestamps will be ignored.
`a` | "a" for "action". Must always be sent. Possible values see above.
`c` | "c" for "client". Must always be sent by client. Must be Client ID as provided by the server upon pairing. If the client is unknown to the server, "CLIENT_UNKNOWN" will be returned and the connection closed. If the client is disabled, "CLIENT_DISABLED" will be returned and the connection closed.
`s` | "s" for "server id". Must always be sent by server. If the server is unknown to the client, "SERVER_UNKNOWN" will be returned and the connection closed.
`sw` | "sw" for "software". Is the name of the software sending this string. For example: "RAAM-WP"
`v` | "v" for "version". Is the RAAM software version as semver string of the sender.
