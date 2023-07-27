# Overview

This document describes network address resolution across J1939 Controlling Applications

Conforms to [Policy](Policy.md) v0.1

Document v0.1

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

## 1. Core Behaviors

### 1.1. PGN: Address Claim

A CA emits this message indicating ownership or intent to own a specific SA.

* SA field MUST contain intended source address of CA
* DA field MUST be `global` [1.5] 4.4.2
* Payload MUST contain [NAME](#13-name)

This message has [special retransmit rules](#114-transport-rules)

#### 1.1.1. Contention Rules

If incoming `Address Claim` SA collides with receiving SA's address, this rule is observed.

"A CA should transmit an address claim if it receives an address claim with a source address that matches its own, and if its own NAME is of a lower value" [1.5] 4.4.3.3.  This specifically pertains to emitting a claim with the same SA.

Therefore,

If [NAME](#13-name) is of a lower value, CA emits a `Claim Address` with its unchanged SA

If [NAME](#13-name) is of a higher value, CA either:

* emits a [Cannot Claim](#15-pgn-cannot-claim-address)
* emits a `Claim Address` after selecting a new SA

If [NAME](#13-name) is of equal value, it is [implied](#1112-implied-behaviors-detecting-ones-own-address-claim) message is ignored

#### 1.1.1.1. Implied Behaviors: Request for Address Claim

J1939 [1.5] *implies* that [Request for Address Claim](#12-pgn-request-address-claim) SHOULD be used to select a new SA at this point for [Arbitrary Address Capable CAs](#322-arbitrary-address-capable-ca)

"An ECU that cannot claim an address shall not send any message other than the Cannot Claim Address message or a Request for Address Claim." [1.5] 4.2.2.3

As of yet J1939-81 [1.5] only indicates a vague notion of when one MAY elect to use aforementioned, with notable exception of Figures A5, A6 and A7 which explicitly indicate its usage after POST.  

This is in possible contradiction to 4.2.2
which somewhat inspecifically claims "CAs must issue it [Address Claimed] during initialization of a network".  It is further contradicted by Figure D1 _never_ emitting a [Request for Address Claim](#12-pgn-request-address-claim).

"These CAs should not send a Cannot Claim Address message or any other message until an Address Claim has been attempted." 4.2.1

We are left with confusion as to when one is truly expected to emit this request.

It is this author's opinion that [1.5] the usage of `Request for Address Claim` was recently expanded and verbiage was not fully aligned.

#### 1.1.1.2. Implied Behaviors: Detecting ones' own address claim

"A CA that is configured to receive messages that it transmits should be able to differentiate between Address Claimed messages received from itself and those received from other CAs" [1.5] 4.4.2

This strongly implies that NAME is used to determine this equality, as there is no other means discussed in [1.5] to do so.

#### 1.1.2. Timing Scenarios

A 250ms timeout is sometimes observed in which time other CAs may contend this claim with their own `Address Claim` message.

##### 1.1.2.1. Scenario 1: Immediate Communication

A Single Address Capable CA SHOULD begin normal communication immediately if
its preferred SA is 0-127 or 248-253. [1.5] 4.4

##### 1.1.2.2. Scenario 2: Delayed Communication

If prerequisite from 1.1.2.1 is not met, a 250ms timeout is REQUIRED to allow other CAs to condend the claim. [1.5] 4.4

#### 1.1.3. Response Rules

If address is contended, see [Contention Rules](#111-contention-rules)

> Otherwise, behavior is TBD

#### 1.1.4. Transport Rules

For just this PGN, one SHOULD turn off physical transport retry mechanisms.  The goal is to reduce colliding claim traffic.

While a bus error is present, repeat the following steps:

1. wait 0-153ms pseudo random delay after EOF
2. retransmit address claim [1.5] 4.4.4.3

##### 1.1.4.1. Implied Behaviors

"The transmit delay will be added to the normal idle period before the 
next claim message is transmitted" [1.5] 4.4.4.3

"idle period" is unclear here, it may refer to:

* [the 250ms timeout](#1122-scenario-2-delayed-communication)
* CAN bus idle state

"After transmitting any claim message [...if] a bus error has occurred, [...] retransmission attempts by the CAN peripheral should be canceled" [1.5] 4.4.4.3

Though the verbiage says "after transmitting", it is equally viable they mean JUST BEFORE transmitting to enable any kind of one-shot mode, since zero automatic retransmission is desired.

Furthermore, no rule indicates to cease retransmissionn attempts, implying they SHOULD be infinite for the duration of a bus error condition.

### 1.2. PGN: Request (Address Claim)

Also known as `Request for Address Claim`.  Inquires one or all targets for their current SA.

SA field MUST be `null` unless CA has claimed an address [1.5] 4.4.2.1
DA field MUST be `global` or specific DA as per 1.2.1.1 and 1.2.1.2

#### 1.2.1. Network Modes & Expected Responses

Emitting CA waits 1250ms to gather CA responses as per [1.5] 4.2.2 and [1.1] 5.12.3.

No response is required if CA has not yet attempted to claim an address.  Otherwise, factoring in
implied behaviors as per 1.2.3., a response MUST emit.

Operates in either broadcast mode or specific DA mode:

##### 1.2.1.1. Broadcast mode

Indicates all CAs MUST issue an immediate [Address Claim](#11-pgn-address-claim) or [Cannot Claim](#15-pgn-cannot-claim-address) as per [1.5] 4.4.3.1.  This includes us, requesting CA [1.5] 4.2.1

It is discouraged to use this mode since it generates a lot of traffic.

##### 1.2.1.2. Specific DA mode

Indicates specific CA MUST issue an immediate [Address Claim](#11-pgn-address-claim) sent to `global` DA (BAM)
as per [1.5] 4.4.3.2

#### 1.2.2. Contention Rules

[1.5] 4.4.3.3

#### 1.2.3. Ambiguities / Implied Behaviors

[1.5] 4.2.1 is unclear what to do if a CA receives this request while in process of claiming an address.  It is
implied a response MUST emit.

There is no indication if this restarts the claim process as per 3.3.3.1.  Specifically, to reset the 250ms timeout.

For simplicity, we elect to:

* NOT reset the timeout.
* Emit address claim as implied by the spec, despite implying we have truly claimed it when we haven't 

### 1.3. NAME

Designates a unique 8-byte value appearing in payload for [Address Claim](#11-pgn-address-claim)

A "higher value" indicates a lower priority [1.5] 4.4.1

### 1.4. PGN: Commanded Address

This "contains 9 bytes of data" and MUST use TP.CM_BAM [1.5] 4.2.3, [1.1] 5.10.3.5

### 1.5. PGN: Cannot Claim Address

This is actually the same PGN as [Address Claim](#11-pgn-address-claim) with distinction that SA MUST contain `null` (254)

Transmitted when:

* CA is Single Address and preferred address is contended
* CA is Arbitrary Address and all addresses are used

This message has [special transmit rules](#154-transport-rules)

#### 1.5.1. Reserved
#### 1.5.2. Reserved

#### 1.5.3. Response Rules

"A pseudo-random delay of between 0 and 153 ms should be inserted between the reception of a message triggering the response and the Cannot Claim Address response" [1.5] 4.2.2.3

This is similar to [address claim rules](#114-transport-rules) but differs in that:

* prng delay only happens in context to a response
* transport level retry is normal, not disabled

##### 1.5.3.1. Implied Behaviors & Ambiguities

[1.5] 4.4.4.4 indicates that response rules are actually identical to (#114-transport-rules) which among other things means transport level retry is disabled.

This would actually make sense since cannot claim is subject to colliding claim traffic as per 1.1.4.

#### 1.5.4. Transport Rules

No special physical transport behavior is required, as [stated above](#153-response-rules)

## 2. Reserved

## 3. Modes of Operation

### 3.1. Specific Modes: Our Parlance

#### 3.1.1. Address Claim

#### 3.1.2. Request for Address Claim: w/ preferred address

#### 3.1.3. Request for Address Claim: w/o preferred address

### 3.2. Specific Modes: Canonical Parlance

These modes are implicitly mutually inclusive.

#### 3.2.1. Single Address Capable CA

In particular refers to [1.5] 3.3.1.
A read only or preset address may be altered by means external to J1939.
Initiates process via either [Address Claim](#11-pgn-address-claim). or `Request (Address Claim)` [0] 1.2.

MUST clear `Arbitrary Capable Address` field in NAME

##### 3.2.1.1. Non Configurable Address CA

SA is read only. 

##### 3.2.1.2. Service Configurable Address CA

Commanded Address Message assigns SA during "a 'service' mode" [1.5] 3.3.1.2.

##### 3.2.1.3. Command Configurable Address CA

Commanded Address Message assigns SA at any time.  Another CA MUST exist to emit such a command.

##### 3.2.1.4. Self-Configurable Address CA

Deterministic factors such as sensor inputs factor into selection of a SA.
Associated NAME MUST reflect these factors.

#### 3.2.2. Arbitrary Address Capable CA

In particular refers to [1.5] 3.3.2.  Implies a null SA with no preference as to which SA is chosen.  However, it appears a preferred SA is REQUIRED and SHOULD be in the range of 128-247 [1.5] 3.3.2

MUST set `Arbitrary Capable Address` field in NAME

Initiates via [Request for Address Claim](#12-pgn-request-address-claim) to mitigate collisions [1.5] 4.2.1.  Solicits a particular or all CAs for their SA. 

### 3.3. Startup state

This section outlines a step by step walk through of address arbitration.
Steps are REQUIRED unless denoted otherwise.

#### 3.3.1. Prerequisites

* CA SHOULD finish a POST
* CA MUST have a preferred SA

#### 3.3.2. Initial Address Claim

CA MUST emit [Address Claim](#11-pgn-address-claim) of preferred SA [1.5] 4.2.2.
Proceed immediately to 3.3.3.

#### 3.3.3. Claiming / Address Contention

This step happens over the course of 250ms

If existing CA contends with newcomer (this) CA, observe [Contention Rules](#111-contention-rules).
Otherwise, startup procedure is complete and operation enters [Nominal](#34-nominal) mode.

#### 3.3.3.1. Scenario 1: Followup Address Claim

If addres is contested and [Contention Rules](#111-contention-rules) permit, one SHOULD attempt a new claim.
This ultimately is quite similar to 3.3.2:

1. CA MUST emit [Address Claim](#11-pgn-address-claim) of newly selected SA
2. Loop back to 3.3.3

#### 3.3.3.2. Scenario 2: Unable to proceed

If one is unable to reattempt, then as per contention rules a [Cannot Claim](#15-pgn-cannot-claim-address) is emitted.
[Degraded](#35-degraded-cannot-claim) mode is entered.

### 3.4. Nominal state

This sections outlines a step by step walk through of scenarios which occur after
startup procedure (3.3) has completed and CA address is claimed

#### 3.4.1. Scenario 1: Receipt of Address Claimed

If incoming SA matches CA's claimed address, observe [contention rules](#111-contention-rules)  This may end up placing CA into [Degraded](#35-degraded-cannot-claim) state.

If no contention, message garners no response.

#### 3.4.2. Scenario 2: Receipt of Request for Address Claimed

As per 1.2.1., CA MUST respond with [Address Claim](#11-pgn-address-claim)

### 3.5. Degraded (Cannot Claim) state

This sections outlines a step by step walk through of scenarios which occur after
startup procedure (3.3.3.2) has ended and no address could be claimed.

#### 3.5.1. Scenario 1: Receipt of Address Claimed

> TBD - evaluate against incoming NAME

#### 3.5.2. Scenario 2: Receipt of Request for Address Claimed

> TBD - evaluate against incoming NAME

As per 1.2.1., CA MUST respond with [Cannot Claim](#15-pgn-cannot-claim-address)

