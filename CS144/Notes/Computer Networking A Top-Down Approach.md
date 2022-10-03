# Computer Networking: A Top-Down Approach

## Chapter1:Computer Networks and the Internet

### Chapter goal:

- Get free, big picture, introduction to terminology

- More depth, detail later in course

### Overview/roadmap:

- What is the Internet? What is a protocol?
- **Network edge**: hosts, access network, physical media
- **Network core**: packet/ circuit switching, internet structure
- **Performance**: loss, delay, throughput
- Protocol layers, service models
- Security
- History

### 1.1 What Is the Internet?

The public Internet —— a specific computer network

#### 1.1.1 A Nuts-and-Bolts Description

Billions of connected computing **devices**

- **hosts** = end systems
- running network **apps** at Internet "edge"

**Packet switches**: forward packets(chunks of data)

- routers, switches

**Communication links**

- fiber, copper, radio, satellite
- Transmission rate: bandwidth

**Networks**

- collection of devices, routers, link managed by an organization 

**Internet: network of networks**

- Interconnected ISPs

**Protocols are everywhere**

- control sending, receiving of messages
- e.g. HTTP(Web), streaming video, Skype, TCP, IP, WiFi, 4G, Ethernet

**Internet standards**

- RFC: Request for Comments
- IETF: Internet Engineering Task Force

<img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-08-12 13.14.33.png" alt="截屏2022-08-12 13.14.33" style="zoom:50%;" />

#### 1.1.2 A Services Description

**Infrastructure that provides services to applications**

- Web, streaming video, multimedia, teleconferencing, email, games, e-commerce, social media, inter-connected appliances

**provides programming interface to distributed applications**

- "hooks" allowing sending/receiving apps to connect to, use Internet transport service
- provides service options, analogous to postal service

#### 1.1.3 What Is a Protocol?

**Human protocols**

Rules for:

- specific messages sent
- specific actions taken when messages received, or other events

**Network protocols**

- All communication activity in Internet governed by protocols

### 1.2 The Network Edge

**Network edge**:

- hosts: clients and servers
- Severs often in data centers

**Access networks, physical media:**

- wired, wireless communication links 

**Network core:**

- interconnected routers
- network of networks

**Q: How to connect end systems to edge routers**

- residential access nets
- institutional access networks
- mobile access networks(WiFi, 4G/5G)

**What to look for:**

- transmission rate(bits/sec) of access network
- Shared or dedicated access among users

#### 1.2.1 Access Networks

**Home Access: DSL, Cable, FTTH, and 5G Fixed Wireless**

<img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-08-12 14.04.42.png" alt="截屏2022-08-12 14.04.42" style="zoom:50%;" />

The two most prevalent of broadband residential access are DSL(digital subscriber line) and cable.

- Cable-based Access

  <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-08-12 13.46.30.png" alt="截屏2022-08-12 13.46.30" style="zoom:50%;" />

  - **Frequency Division Multiplexing(FDM)**: different channels transmitted in different frequency bands
  - **HFC**: hybrid fiber coax
    - asymmetric: up to 40Mbps to 1.2Gbs downstream transmission rate, 30-100 Mbps upstream transmission rate
    - Network of cable, fiber attaches homes to ISP router
      - homes share access network to cabel headend

- DSL

  <img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-08-12 13.52.50.png" alt="截屏2022-08-12 13.52.50" style="zoom:50%;" />

  - use existing telephone line to central office DSLAM
    - data over DSL phone line goes to Internet
    - voice over DSL phone line goes to telephone net
  - 24-52 Mbps dedicated downstream transmission rate
  - 3.5-16 Mbps dedicated upstream transmission rate

**Access in the Enterprise (and the Home):Ethernet and WiFi**

**Wireless access networks**(Chapter 7)

Shared wireless access network connects end system to router

- Via base station aka "access point"

Wireless local area networks(WLANs) ====> WifFi

Wide-area cellular access networks ====> 3G/4G/5G

#### 1.2.2 Physical Media

**Host**: sends packets of data

- takes application message
- breaks into smaller chunks, known as packets, if length of **L** bits
- transmits packet into access network at transmission rate **R**
  - link transmission rate, aka link capacity, aka link bandwidth

packet transmission delay = time needed to transmit L-bit packet into link = $\frac{L_{(bits)}}{R_{{(bits/sec)}}}$

<img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-08-15 16.26.34.png" alt="截屏2022-08-15 16.26.34" style="zoom:50%;" />

**Links**: physical media

- **bit**: propagates between transmitter/ receiver pairs
- **physical links**: what lies between transmitter and receiver
- **guided media**:signals propagate in solid media: copper, fiber, coax
- **unguided media**:signals propagate freely e.g. radio
- **Twisted Pair(TP)**
- **Coaxial cable**
- **Fiber optic cable**
- **Wireless radio**
  - **Radio link types:**
    - Wireless LAN(WiFi)
    - Wide-area(e.g. 4G celluar)
    - Bluetooth: cable replacement
    - terrestrial microwave ===> point to point
    - satellite

### 1.3 The Network Core

<img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-08-15 16.39.37.png" alt="截屏2022-08-15 16.39.37" style="zoom:50%;" />

#### 1.3.1 Packet Switching

In a network application, end systems exchange **messages** with each other.

Between source and destination, each packet travels through communication links and **packet switches**

- **routers switches**  
- **link-layer switches**

If a source end system or a packet switch is sending a packet of *L* bits over a link with transmission rate *R* bits/sec, then the time to transmit the packet is *L/R* seconds.

Transmission Time $\frac{L}{R}(sec)$

**Store-and-Forward Transmission**
