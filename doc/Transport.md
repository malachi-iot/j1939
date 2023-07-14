# Overview

This document describes J1939 `Transport Protocol Functions`
accomodating > 8 byte data fields

Conforms to [Policy](Policy.md) v0.1

Document v0.1

# Detail

`Transport Protocol Functions` as outlined in [1.1] 5.10 overcomes CAN bus' innate
limitation of 8 byte payloads.

## 1. Core Behavior

### 1.1. PGN: Connection Management (TP.CM)

Used to "initiate and close connections and also to control flow" - [1.1] 5.10.3

A "connection" (also referred to as a "virtual connection") is "considered a temporary association of two nodes for the purpose of transferring a single large message" [1.1] 5.10.2

A "message" in this contexts refers to reassembly of multiple related CAN frames.

This one command breaks out into the 5 commands outlined in 1.1.2-1.1.6, herein referred
to in this section as "TP" commands.

In particular, 

#### 1.1.1. Shared Payload Structure

TP commands share the first byte as the `Control byte`

#### 1.1.2. Request to Send (TP.CM_RTS)

Initiates a connection

#### 1.1.3. Clear to Send (TP.CM_CTS)

#### 1.1.4. End of Message Acknowledgment (TP.CM_EndOfMsgACK)

#### 1.1.5. Connection Abort (TP.Conn_Abort)

#### 1.1.6. Broadcast Announce Message (TP.CM_BAM)

### 1.2. PGN: Data Transfer Message (TP.DT)

This carries message data 7 bytes at a time, with the remaining 1 byte serving as a sequence number for reassembly.

## 2. Reserved

## 3. Modes of Operation

### 3.1. Bulk Data Transfer (TBD)