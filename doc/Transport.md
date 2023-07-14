# Overview

This document describes J1939 `Transport Protocol Functions`
accomodating > 8 byte data fields

Conforms to [Policy](Policy.md) v0.1

Document v0.1

# Detail

`Transport Protocol Functions` as outlined in [1.1] 5.10 overcomes CAN bus' innate
limitation of 8 byte payloads

## 1. Core Behavior

A "message" in this contexts refers to reassembly of multiple related CAN (TP.DT) frames.  Being that it is an extended form of a PDU data field, a message associates to a PGN.

### 1.1. PGN: Connection Management (TP.CM)

Used to "initiate and close connections and also to control flow" - [1.1] 5.10.3

A "connection" (also referred to as a "virtual connection") is "considered a temporary association of two nodes for the purpose of transferring a single large message" [1.1] 5.10.2

This one command breaks out into the 5 commands outlined in 1.1.2-1.1.6, herein referred
to in this section as "TP" commands.

#### 1.1.1. Shared Payload Structure

TP commands share the first byte as the `Control byte`

#### 1.1.2. Request to Send (TP.CM_RTS)

Initiates a connection

Indicates total message size and PGN
Indicates maximum number TP.DT packets sendable at any given time

Maximum number of packets SHOULD NOT exceed 16 [1.1] 5.12.6

#### 1.1.2.1. Timing and Response

#### 1.1.3. Clear to Send (TP.CM_CTS)

Emitted in response to TP.CM_RTS when CA is ready to receive.
Indicated also is how many TP.DT packets are acceptable to receive.

#### 1.1.4. End of Message Acknowledgment (TP.CM_EndOfMsgACK)

Emitted when message is completely received without error.

#### 1.1.5. Connection Abort (TP.Conn_Abort)

#### 1.1.6. Broadcast Announce Message (TP.CM_BAM)

Informs all nodes a message is about to broadcast [1.1] 5.10.3.5

### 1.2. PGN: Data Transfer Message (TP.DT)

This carries message data 7 bytes at a time, with the remaining 1 byte serving as a sequence number for reassembly.

### 1.3. Timing

* Responses MUST occur wihin 200ms
* Response timeout MUST be 1250ms or greater before giving up or retrying
* Connection oriented TP.DT packets MUST not exceed 200ms interval between TP.DT packets
* BAM TP.DT packets MUST fall between 50ms-200ms interval between TP.DT packets
* Recipients of BAM TP.DT observe a 250ms timeout (presumably before giving up)

For more detail, see [1.1] 5.12.3

## 2. Reserved

## 3. Modes of Operation

### 3.1. Bulk Data Transfer (TBD)