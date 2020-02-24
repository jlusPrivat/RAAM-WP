# Specs Version 1.0.0

# Server Commands
This guide describes the usage of the RAAM-WP tcp socket server.
It therefore helps implementing applications, that employs the network
functionalities provided by this program.

**Note, that any application using the RAAM server commands, partially or as a whole, must be licensed under GNU General Public License v3.0**

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
* TCP / BLE messages consist of Null terminated UTF-8 Character Strings. (Null termination before HMAC)
* Every TCP/BLE message is key-value based in form of: `key1="value1";key2="value2";` for every type of value. No key may be used more than once in a single message.
* Only error messages do not come as key-value based messages.
* Values containing a `"` character are prohibited and lead to the message being rejected.
* Every Message has to have the 32 bytes of the HMAC (using sha256) appended in raw binary form. The HMAC is generated from the entire message, excluding only the HMAC itself. If the HMAC is wrong, "WRONG_HMAC" will be returned. This is optional, when debug mode is enabled.
* Rejected messages will be fully ignored, unless a specific error message is returned.
* Further rules regarding the messages must be extracted from the following section.


## About Keys and Actions
### Overview of all actions
* **init**: Must be issued by the client within 10 seconds after establishing a transport layer connection using the servers IP adress and port and the additional required key `c`. The server has to answer using this action with the additional required key `s`.
* **info**: Can be issued by both server or client. The other party answers with "inforeturn" action.
* **inforeturn**: Server sends with additional required keys `s`, `c`, `sw`, `v`, `sv`. Client sends with additional required keys `c`, `sw`, `v`, `sv`.
* **changeServerId**: Only issued by server. Is sent, when the serverId changed. Additional required keys: `s`.
* **changeClientId**: Only issued by server. Is sent, when a new clientId was assigned to the paired client. Additional required keys: `c`.
* **enumDevices**: Sent by the client as a request to receive all devices or detailed information about one particular device. The server responds with one "dev" action per device. Additional optional keys: `di`.
* **enumDevices**: Sent by the server, implicating all devices may have changed and should be re enumerated by the client.
* **enumSessions**: Sent by the client as a request to receive all audio sessions or detailed information about one particular session. The server responds with one "sess" action per audio session. Additional optional keys: `si`. Additional required keys: `di`.
* **enumSessions**: When sent by the server, all sessions may have changed and should be re enumerated by the client. Additional required keys: `di`.
* **dev**: Sent by the client, when changes are supposed to be made. Additional required keys: `di`. Additional optional keys: `m`, `l`.
* **dev**: Sent by the server, when changes were made to a device, or when "enumDevices" was called, or when a new device was added, or when a device was removed. When a new device was added or when all devices are enumerated, all optional keys become required keys. Additional required keys: `di`, `dc`. Additional optional keys: `ddl`, `dds`, `dff`, `m`, `l`.
* **sess**: Sent by the client, when changes are supposed to be made. Additional required keys: `si`, `di`. Additional optional keys: `sic`, `m`, `l`.
* **sess**: Sent by the server, when changes were made to a session, or when "enumSessions" was called, or when a new session was added, or when a session was removed. When a new session was added or when all sessions are enumerated, all optional keys become required keys. Addtional required keys: `di`, `si`. Additional optional keys: `sc`, `sdn`, `sic`, `m`, `l`

### Overview of all possible keys
Key | Description
--- | -----------
`t` | "t" for "time". Must always be sent, except for devices in debug mode. The current unix timestamp in seconds. This is used to ensure "freshness" of the data being integrity checked with the hmac. If the received timestamp differs from the local timestamp by more than 30 seconds, "INVALID_TIMESTAMP" will be returned and the connection closed. Applications must keep track of the last sent timestamp and check, that no message with an earlier timestamp is allowed. Messages with earlier timestamps will be ignored.
`a` | "a" for "action". Must always be sent. Possible values see above.
`c` | "c" for "client". Must be Client ID as provided by the server upon pairing. If the client is unknown to the server, "CLIENT_UNKNOWN" will be returned and the connection closed. If the client is disabled or the user rejected a pairing request, "CLIENT_DISABLED" will be returned and the connection closed.
`s` | "s" for "server id". If the server is unknown to the client, "SERVER_UNKNOWN" will be returned and the connection closed.
`sw` | "sw" for "software". Is the name of the software sending this string. For example: "RAAM-WP".
`sv` | "sv" for "specs version". The version of this document.
`v` | "v" for "version". Is the RAAM software version as semver string of the sender.
`di` | "di" for "device id".
`dc` | "dc" for "device configuration". Can be either "i" (for input like microphone) or "o" (for output like speaker). Has a "d" appended, if it is the default device for that role. Has an "u" appended, if it is unplugged, otherwise has an "a" appended, if is active. This value is just "0", when the device was removed or not found.
`ddl` | "ddl" for "device description long".
`dds` | "dds" for "device description short".
`dff` | "dff" for "device form factor". Is one of these values: "RemoteNetworkDevice", "Speakers", "LineLevel", "Headphones", "Microphone", "Headset", "Handset", "UnknownDigitalPassthrough", "SPDIF", "DigitalAudioDisplayDevice", "UnknownFormFactor".
`si` | "si" for "session id".
`sc` | "sc" for "session configuration". Value contains "s", if this session is a system sound session. Value contains This value ist just "0", when the session was removed or not found.
`sdn` | "sdn" for "session display name".
`sic` | "sic" for "session icon". This is the base64 representation of the image in "PNG" format. This value is just empty, when no icon was provided.
`m` | "m" for "mute". Can be either `t` for true or `f` for false.
`l` | "l" for "soundlevel". Any value for the volume between 0 and 100.
