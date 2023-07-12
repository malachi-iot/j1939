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
    * Self-Configurable Address CA
* Arbitrary Address Capable CA

## 1. Common Behaviors

### 1.1. PGN: Address Claim

A CA emits this message indicating ownership or intent to own a specific SA.

A 250ms timeout is observed in which time other CAs may contend this claim with their
own `Address Claim` message.

DA field MUST be `global` [1.5] 4.4.2

#### 1.1.1. Contention Rules

"A CA should transmit an address claim if it receives an address claim with a source address that matches its own, and if its own NAME is of a lower value" [1.5] 4.4.3.3

If NAME is of a higher value, CA either:

* emits a `Cannot Claim Address`
* emits a `Claim Address` after selecting a new SA

### 1.2. PGN: Request (Address Claim)

Also known as `Request for Address Claim`.  Inquires one or all targets for their current SA.

SA field MUST be `null` unless CA has claimed an address [1.5] 4.4.2.1

#### 1.2.1. Network Modes

Operates in either BAM mode or specific DA mode:

##### 1.2.1.1. BAM mode

Indicates all CAs MUST issue an immediate `Address Claim` [0] 1.1 or `Cannot Claim` [0] 1.4
as per [1.5] 4.4.3.1

##### 1.2.1.2. Specific DA mode

Indicates specific CA MUST issue an immediate `Address Claim` [0] 1.1 sent to `global` DA (BAM)
as per [1.5] 4.4.3.2

#### 1.2.2. Contention Rules

[1.5] 4.4.3.3

### 1.3. NAME

Designates a unique 8-byte value appearing in payload for `Address Claim` [0] 1.1.

A "higher value" indicates a lower priority [1.5] 4.4.1

### 1.4. PGN: Commanded Address

### 1.5. PGN: Cannot Claim Address

## 2. Reserved

## 3. Modes

These modes are implicitly mutually inclusive.

### 3.1. Modes: Our Parlance

#### 3.1.1. Address Claim

#### 3.1.2. Request for Address Claim: w/ preferred address

#### 3.1.3. Request for Address Claim: w/o preferred address

### 3.2. Modes: Canonical Parlance

#### 3.2.1. Single Address Capable CA

In particular refers to [1.5] 3.3.1.
A read only or preset address may be altered by means external to J1939.
Initiates process via either `Address Claim` [0] 1.1. or `Request (Address Claim)` [0] 1.2.

MUST clear `Arbitrary Capable Address` field in NAME

##### 3.2.1.1. Non Configurable Address CA

SA is read only. 

##### 3.2.1.2. Service Configurable Address CA

Commanded Address Message assigns SA during "a 'service' mode" [1.5] 3.3.1.2.

##### 3.2.1.3. Command Configurable Address CA

Commanded Address Message assigns SA at any time.

##### 3.2.1.4. Self-Configurable Address CA

Deterministic factors such as sensor inputs factor into selection of a SA.
Associated NAME MUST reflect these factors.

#### 3.2.2. Arbitrary Address Capable CA

In particular refers to [1.5] 3.3.2.  Implies a null SA with no preference as to which SA is chosen.

MUST set `Arbitrary Capable Address` field in NAME

Initiates via `Request for Address Claim` to mitigate collisions [1.5] 4.2.1.  Request either:

* Solicits all CAs for their SAs
* Solicits a particular CA at an SA

A timeout period of 1250ms [1.5] Figure A5, A6, A7
