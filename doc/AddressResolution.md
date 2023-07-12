# Overview

Document v0.1

Uses RFC 2119, RFC 8174 terminology

All references found in REFERENCES.md

# Detail

This document describes CAs which have configurable source addresses (SA)

There are various ways a CA can determine its SA:

* Address Claim (implies preferred address)
* Request for Address Claim: w/ preferred address)
* Request for Address Claim: w/o preferred address)

In canonical parlance, these are:

* Single Address Capable CA
    * Non-Configurable Address CA
    * Service Configurable Address CA
    * Command Configurable Address CA
    * Self Configurable Address CA
* Arbitrary Address Capable CA

## 1. Common Behaviors

### 1.1. PGN: Address Claim

A CA emits this message to indicate ownership or intent to own a specific SA.
A 250ms timeout is observed in which time other CAs may contend this claim with their
own `PGN: Address Claim` message.

### 1.2. PGN: Request (Address Claim)

Operates in either BAM mode or specific DA mode:

#### 1.2.1. BAM mode

Indicates all CAs SHOULD (TBD) issue an immediate address claim [0] 2.1

#### 1.2.2. Specific DA mode

Indicates specific CA SHOULD issue an immediate address claim [0] 2.1

### 1.3. NAME

### 1.4. PGN: Commanded Address

Designates a unique 8-byte value appearing in payload for 2.1.

## 2. Reserved

## 3. Modes

### 3.1. Modes: Our Parlance

#### 3.1.1. Address Claim

#### 3.1.2. Request for Address Claim: w/ preferred address

#### 3.1.3. Request for Address Claim: w/o preferred address

### 3.2. Modes: Canonical Parlance

#### 3.2.1. Single Address Capable CA

In particular refers to [1.5] 3.3.1.
A read only or preset address may be altered by means external to J1939.

##### 3.2.1.1. Non Configurable Address CA

SA is read only. 

##### 3.2.1.2. Service Configurable Address CA

SA is configured by the Commanded Address Message
