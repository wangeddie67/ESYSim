
# User Manual

## Overview

ESYNet is a cycle-accuracy network-on-chip simulator. It is integrated into 
ESYSim as the interconnection architecture. This branch provides the basic 
version of ESYNet. 

ESYNet provides many options so that ESYNet can be used to simulate a wide of 
situations. Moreover, ESYNet provides the flexibility to be modified for 
irregular or custom situations.

- **Topology**. ESYNet supports regular topologies, including switch, ring, 
2D-mesh/torus and diamension mesh/torus. The network can be defined by options. 
ESYNet can also accept irregular topologies defined by network configuration 
file. The irregular topology can be drawn in VisualNoC.

- **Routing algorithm**. ESYNet provides built-in routing algorithms for 
different topologies. Moreover, ESYNet provides table-based routing algorithm 
so that user can implement custom deterministic routing algorithm without 
modify the simulator. At last, ESYNet provides the ability to add new routing 
algorithm within the simulator, which is useful for dynamic routing algorithm.

- **Switch Methods**. ESYNet provides wormhole switch for all topologies. Flits 
can be delivered only if there is any free slot in the neighbor. To avoid 
deadlock, ESYNet also provides virtual-cut switch for ring topology only. 
Packets cannot be injected into the ring unless there is enough continuous free 
slots.

- **Flow control**. ESYNet support credit-based flow control methods. A router 
know the number of free slots in the input buffers of its neighbors

- **Arbiter**. ESYNet implements three different arbiters, which are random 
arbiter, round-robin arbiter and matrix arbiter.

- **Event trace**. ESYNet can record the simulation procedure into a event 
trace file, which can be used by VisualNoC to replay the simulation. Also, 
ESYNet support on-line simulation replay.

### Install and Compile

Download the code from github. There are two ways to get the environment. First
way is to download full ESYSim project.

```shell
git clone https://github.com/wangeddie67/ESYSim.git
```

The other way is to download the `esynet` branch only.

```shell
git clone -b esynet https://github.com/wangeddie67/ESYSim.git
```

After downloading, enter `esynet` folder and run Makefile.

```
cd ESYSim/esynet
make
```

After compiling, the execuate file `esynet` can be found in the folder 
`esynet/bin`. Run `esynet` directly to verfiy installation.

```
cd ESYSim/esynet/bin
./esynet
```

## Options

ESYNet defines the option list based on `EsyArgumentList`. The default options 
are as follow:

Option `-h` or `-help` prints optional list.

Option `-xml` prints argument XML file used by VisualNoC. 
```sh
-xml <file path> -xmlexec <command for executing> -xmlcomp <command for compiling> -xmlname <name of this tool>
```

The options of ESYNet can be categorized into several group as follow.

## Network Configuration Options

These options are used to specify the topology, routing algorithm, and switch 
method of the network as well as the architecture of the router and the network 
interface.

```sh
-topology [Switch|Ring|2DMesh|2DTorus|DiaMesh|DiaTorus] 
-network_size <integar list>
-phy_number <integar>
-vc_number <integar>
-in_buffer_size <integar>
-out_buffer_size <integar>
-data_path_width <integar>
-link_length <double>
-routing_alg [SingleRing|DoubleRing|XY|TXY|DyXY|Table|DiaMesh|DiaTorus]
-routing_table <file path with extension>
-arbiter [Random|RR|Matrix]
-switch [Wormhole|Ring]
-ni_buffer_size <integar>
-ni_read_ready <integar>
-network_cfg_file_enable
-network_cfg_out_file_enable
-network_cfg_file_name <file path without extension>
```

Option `-topology` specifies network topology. `DiaMesh` and `DiaTorus` mean 
cube topology.

Option `-network_size` specifies network size. This option accepts a list of 
integar. Each number specifies the number of router in one diamension. For 
switch topology, it must be 1; for ring topology, it only accepts one number; 
for mesh topology, it must accepts two numbers. 

Option `-phy_number` specifies the number of physical channels in one router.

Option `-vc_number` specifies the number of virtual channels in each physical 
channel.

Option `-in_buffer_size` specifies the number of slots of the input buffer for 
one virtual channel of one physical channel.

Option `-out_buffer_size` specifies the number of slots of the output buffer 
for one virtual channel of one physical channel.

Option `-data_path_width` specifies the width of data path, in bit.

Option `-link_length` specifies the physical length of link between routers, in 
um.

Option `-routing_alg` specifies the routing algorithm. `SingleRing` and 
`DoubleRing` are valid for Ring topology only. `XY` and `DyXY` are valid for 2D 
Mesh topology only. `TXY` are valid for 2D Torus topology only. `DiaMesh` and 
`DiaTorus` are valid for diamension cube topology only. `Table` enables 
table-based routing algorithm.

Option `-routing_table` specifies the routing table file used for table-based 
routing algorithm.

Option `-arbiter` specifies the type of arbiter.

Option `-switch` specifies the switch method. `Ring` is only valid for ring 
topology.

Option `-ni_buffer_size` specifies the number of slots of the NI.

Option `-ni_read_ready` specifies the time before the received packet is 
removed from NI.

Option `-network_cfg_file_enable` means the network is configured by network 
configuration file. If this option appears, other options are disabled.

Option `-network_cfg_out_file_enable` means the network topology is stored in 
network configuration file. This option is valid only when
`-network_cfg_file_enable` does not appear.

Option `-network_cfg_file_name` specifies the input or output network 
configuration file.

## Simulation Control Options

These options are used to control the simulation procedure. 

```sh
-random_seed <integar>
-simulation_period <double>
-injected_packet <integar>
-warmup_packet <integar>
-latency_measure_packet <integar>
-throughput_measure_packet <integar>
-sim_length <double>
```

Option `-random_seed` specifies the seed for the random generator. 

Option `-simulation period` specifies period of simulation, in cycle. Default 
is 1.0.

Option `-injected_packet` specifies the total number of injected packets. 
Packet generator stops injecting new packets after the number of injected 
packets reaches specified value and the simulation stops after all injected 
packets are accepted.

Option `-warmup_packet` specifies the number of ignored packets before 
measuring latency and throughput.

Option `-latency_measure_packet` specifies the number of packets to measure 
latency. A simulation stops if latency and throughput measurement finishes.

Option `-throughput_measure_packet` specifies the number of packets to measure 
throughput. A simulation stops if latency and throughput measurement finishes.

Option `-sim_length` specifies the simulation length, in cycle.

## Traffic Injection Options

These options are used to specify the injected traffc during the simulation. 
The necessary options include the traffic pattern, the packet injection rate, 
and so on.

```sh
-traffic_injection_disable
-input_trace_enable
-input_trace_file_text_enable
-input_trace_buffer_size <integar>
-input_trace_file_name <file path without extension>
-traffic_rule [TP_UNIFORM|TP_TRANSPOSE1|TP_TRANSPOSE2|TP_BITREVERSAL|TP_BUTTERFLY|TP_SHUFFLE]
-traffic_pir <double range (0,1)>
-packet_size <integar>
-output_trace_enable
-output_trace_file_text_enable
-output_trace_buffer_size <integar>
-output_trace_file_name <file path without extension>
```

Option `-traffic_injection_disable` means traffic generator is disabled. No 
packet is injected into the network by traffic generator. If 
`-traffic_injection_disable` appears, other options does not work except 
the options to generate benchmark trace file (`-output_trace_enable`, 
`-output_trace_file_text_enable`, `-output_trace_buffer_size`, and
`-output_trace_file_name`).

Option `-input_trace_enable` means traffic generator injects packets according 
to a benchmark trace file, which records the time, source and destination of 
each packets. If `-input_trace_enable` appears, option `-traffic_rule`, 
`-traffic_pir`, and `-packet_size` do not work. Otherwise, Option 
`-input_trace_file_text_enable`, `-input_trace_buffer_size`, and 
`-input_trace_file_name` do not work.

Option `-input_trace_file_text_enable` means the input benchmark trace file is 
text format.

Option `-input_trace_buffer_size` specifies the size of input stream buffer to 
input benchmark trace file.

Option `-input_trace_file_name` specifies the path of input benchmark trace 
file. File extension should not appear.

Option `-traffic_rule` specifies the traffic profiles to generate packets. 
Traffic profiles specifies the relationship between destination and source 
address.

Option `-traffic_pir` specifies the packet injection rate (PIR) to generate 
packets. The unit of PIR is packet/cycle/NI. The valid value ranges from 0.0 to 
1.0, which means one NI can inject at most 1 packets at one cycle.

Option `-packet_size` specifies the size of flit in packet injection rate. 

Option `-output_trace_enable` means the simulator records all packets into a 
benchmark trace file. It is only possible if the traffic generator injects 
according to traffic profile and packet injection rate.

Option `-output_trace_file_text_enable` means the output stream prints the 
benchmark trace into a text format file as well a binary format file.

Option `-output_trace_buffer_size` specifies the size of output stream buffer to 
output benchmark trace file.

Option `-output_trace_file_name` specifies the path of output benchmark trace 
file. File extension should not appear.

## Event Trace Options

These options are used to specify the event trace file and the output stream to 
event trace file.

```sh
-event_trace_enable
-event_trace_file_text_enable
-event_trace_buffer_size <integar>
-event_trace_file_name <file path without extension>
-event_trace_cout_enable
```

Option `-event_trace_enable` means the simulator records the simulation into a 
event trace file. 

Option `-output_trace_file_text_enable` means the output stream prints the 
event trace into a text format file as well a binary format file.

Option `-output_trace_buffer_size` specifies the size of output stream buffer to 
output event trace file.

Option `-output_trace_file_name` specifies the path of output event trace file. 
File extension should not appear.

## Simulation Engine

ESYNet utilizes an event-driven simulation engine to perform the cycle-accurate 
simulation. 

### Event-driven Simulation Engine

The event-driven simulator consists of a simulation engine and 
simulation components. Simulation engine drives the simulation process forward 
while simulation components store the status and do the operations. In ESYNet, 
simulation components include routers and network interfaces.

The kernel of the simulation engine is the event queue which stores in the 
event in the order of increasing time. The event is an abstraction of 
simulation operations. The time determines when the simulation engine should 
respond to the specified event.

During the simulation, the simulation engine takes out the first event in the 
event queue and calls suitable simulation components to respond to the event. 
Simulation components receive the event and change their status and generate 
new events if necessary. The new events are inserted back to the event queue.

The simulation engine continues the iteration until all the events in the event 
queue have been responded. Thus, if the event queue is empty, the simulation is 
finished. When the simulation starts, the first event should be inserted to the 
event queue so that the simulation can start. If the event queue is empty, the 
simulation is finished.

The simulation time depends on the number of events. For example, if the NoC is 
not under heavy traffic, the simulation time can be very fast because there is 
not much event. On the other hand, the simulation accuracy depends on the 
fine-grain of event definitions.

ESYNet provides `EsynetEventQueue` as the event queue. Event queue inherts from 
STL `multiset` so that it can automatically sort items within the queue. The 
key is the time when to response the event. 

ESYNet provides `EsynetSimBaseUnit` as the base-class for simulation 
components. The simulation components must inhert from `EsynetSimBaseUnit`. 
Otherwise, they cannot be called by event queue. Currently, ESYNet provides 
three simulation components:
- `EsynetFoundation`: Network platform, includes all routers and NIs.
- `EsynetRouter`: Router structure.
- `EsynetNI`: NI structure.

During the simulation, `EsynetEventQueue` pops the first event from the event 
queue. Then, `EsynetEventQueue` calls `EsynetFoundation` to response this 
event. `EsynetFoundation` distinguishes the type of the event and calls 
coresponding router and NI to response the event. Further, `EsynetRouter` and 
`EsynetNI` response the event. When responsing event, simulation components may 
generate new events. These new events are collected and injected into event 
queue as well. 

### Events in Esynet

ESYNet provides `EsynetEvent` to store the information for different events.

#### Packet generation event

Packet generation event (EVG) means a new packet should be injected into the 
network. EVG events are generated by the traffic generator or integration APIs.

`EsynetEvent` provides `EsynetEvent::generateEvgEvent` to generate a EVG event. 
EVG event should provides the following information.

| Fields | Data type | Note |
| ---- | ---- | ---- |
| `EsynetEvent::m_event_time`     | `double`     | The injection time of the packet. |
| `EsynetEvent::m_event_type`     | `long`       | `EsynetEvent::EVG`. |
| `EsynetEvent::m_src_id`         | `long`       | Source router id. |
| `EsynetEvent::m_des_id`         | `long`       | Destination router id. |
| `EsynetEvent::m_flit`           | `EsynetFlit` | The head flit of new packet. |

`EsynetEvent::m_flit` should provides the following information.

| Fields | Data type | Note |
| ---- | ---- | ---- |
| `EsynetFlit::m_id`              | `long`       | Packet id. |
| `EsynetFlit::m_pac_size`        | `long`       | Packet size in flit. |
| `EsynetFlit::m_type`            | `FlitType`   | `EsynetFlit::FLIT_HEAD`. |
| `EsynetFlit::m_src_ni`          | `long`       | Source router id. |
| `EsynetFlit::m_des_ni`          | `long`       | Destination router id. |
| `EsynetFlit::m_flag`            | `uint16_t`   | Flit flags. |
| `EsynetFlit::m_flit_start_time` | `double`     | The injection time of the packet. |

EVG 

`EsynetFoundation` provides `EsynetFoundation::receiveEvgMessage` method to 
handle EVG event. The network platform calls the NI specified by 
`EsynetEvent::m_des_id` to respond EVG event. NI generates all the flits for 
the packets and stores flits in the injection queue. See also Traffic 
generation for more information.

#### Router pipeline

Router pipeline event (ROUTER) means one clock cycle for the pipeline in the 
router. ROUTER event provides the ability for ESYNet to perform the 
cycle-accuracy simulation. ROUTER events are generated by the simulation engine 
itself.

`EsynetEvent` provides `EsynetEvent::generateRouterEvent` to generate a ROUTER 
event. ROUTER event should provide the following information.

| Fields | Data type | Note |
| ---- | ---- | ---- |
| `EsynetEvent::m_event_time` | `double` | The simulation cycle. |
| `EsynetEvent::m_pipe_time`  | `double` | The period of the pipeline in cycle. |
| `EsynetEvent::m_event_type` | `long`   | `EsynetEvent::ROUTER`. |
| `EsynetEvent::m_src_id`     | `long`   | Specified router to respond this event. Default value is -1. |

ROUTER event is the first event in the event queue. When the event queue is 
constructed, the event queue injects the first ROUTER event.

`EsynetFoundation` provides `EsynetFoundation::receiveRouterMessage` method to 
handle ROUTER event. During the simulation, the network platform generates the 
next ROUTER event when it responds to one ROUTER event. The simulation cycle 
of the next ROUTER is calculated by added `EsynetEvent::m_pipe_time` to 
`EsynetEvent::m_event_time`. By default, the network platform calls all the 
routers and NIs to respond ROUTER event. Each router and NI does the pipeline 
once when they receive ROUTER event. See also Router pipeline and NI pipeline 
for more information.

ESYNet can support multi-clock-domains in NoC which means simulation components 
may not respond ROUTER event at the same time. The basic clock period is 1.0 
cycle. One ROUTER event with `EsynetEvent::m_pipe_time` as 1.0 can trigger all 
routers and NIs within the base clock domain. Another clock period is measured 
by the basic clock period. One ROUTER event with `EsynetEvent::m_pipe_time` not 
as 1.0 can only trigger the router and NI specified by `EsynetEvent::m_src_id`.

The network platform also calls traffic generator in ROUTER events. The traffic 
generator is only called by the ROUTER event with `EsynetEvent::m_pipe_time` as 
1.0. See also Traffic generation for more information.

#### Link transmission event

Link transmission event (WIRE) means a flit transmits through one link. WIRE 
events are generated by the link transmission stage in router pipeline.

`EsynetEvent` provides `EsynetEvent::generateWireEvent` to generate a WIRE 
event. One WIRE event means `EsynetEvent::m_flit` transmits from 
`EsynetEvent::m_src_vc` of `EsynetEvent::m_src_pc` of `EsynetEvent::m_src_id` 
to `EsynetEvent::m_des_vc` of `EsynetEvent::m_des_pc` of 
`EsynetEvent::m_des_id` at `EsynetEvent::m_event_time`. WIRE event should 
provides the following information. 

| Fields | Data type | Note |
| ---- | ---- | ---- |
| `EsynetEvent::m_event_time` | `double`     | The time when the flit should be received by the destination router. |
| `EsynetEvent::m_event_type` | `long`       | `EsynetEvent::WIRE`. |
| `EsynetEvent::m_src_id`     | `long`       | Source router. |
| `EsynetEvent::m_src_pc`     | `long`       | Output physical port of source router. |
| `EsynetEvent::m_src_vc`     | `long`       | Output virtual channel of source router. |
| `EsynetEvent::m_des_id`     | `long`       | Destination router. |
| `EsynetEvent::m_des_pc`     | `long`       | Input physical port of destination router. |
| `EsynetEvent::m_des_vc`     | `long`       | Input virtual channel of destination router. |
| `EsynetEvent::m_flit`       | `EsynetFlit` | The transmitted packet. |

`EsynetFoundation` provides `EsynetFoundation::receiveWireMessage` method to 
handle WIRE event. The network platform calls the router or NI specified by 
`EsynetEvent::m_des_id` to respond WIRE event. See also Receive flit in router 
and [Receive flit in NI] for more information. The network platform also calls 
the router or NI specified by `EsynetEvent::m_src_id` to clear the status of 
link.

The source and destination id encodes the routers and NIs in the same field. 
The lower part of the value is used to specified routers; the higher part of 
the value is used to specified NI. For example, for a 8x8 regular mesh 
topology, 0-63 is used to specified router; 64-127 is used to specified NI.

#### Credit transmission event

Credit transmission event (CREDIT) means a credit information transmits through 
one link. The credit means the number of free-slots in the virtual channels of 
the input channels connected with the virtual channels of the output channels. 
In credit-based flow control, the credit is transmitted by the wires between 
routers as well. The CREDIT events are generated by the router pipeline as well. 
See [flow control] for more information.

`EsynetEvent` provides `EsynetEvent::generateCreditEvent` to generate a CREDIT 
event. CREDIT event should provides the following information.

| Fields | Data type | Note |
| ---- | ---- | ---- |
| `EsynetEvent::m_event_time` | `double` | The time when the flit should be received by destination router. |
| `EsynetEvent::m_event_type` | `long`   | `EsynetEvent::CREDIT`. |
| `EsynetEvent::m_src_id`     | `long`   | Source router. |
| `EsynetEvent::m_src_pc`     | `long`   | Input physical port of source router. |
| `EsynetEvent::m_src_vc`     | `long`   | Input virtual channel of source router. |
| `EsynetEvent::m_des_id`     | `long`   | Destination router. |
| `EsynetEvent::m_des_pc`     | `long`   | Output physical port of destination router. |
| `EsynetEvent::m_des_vc`     | `long`   | Output virtual channel of destination router. |

`EsynetEvent::m_flit` should provides the following information.

| Fields | Data type | Note |
| ---- | ---- | ---- |
| `EsynetEvent::m_flit.m_id`  | `long`   | Credit value. |

`EsynetFoundation` provides `EsynetFoundation::receiveCreditMessage` method to 
handle CREDIT event. The network platform calls the router or NI specified by 
`EsynetEvent::m_des_id` to respond CREDIT event. See also [flow control] for 
more information.

#### NI read event

NI read event (NIREAD) means a packet should be ejected from the network. 
NIREAD events are generated by the integration APIs.

`EsynetEvent` provides `EsynetEvent::generateNIReadEvent` to generate a NIREAD 
event. NIREAD event should provides the following information.

| Fields | Data type | Note |
| ---- | ---- | ---- |
| `EsynetEvent::m_event_time` | `double` | The ejection time of the packet. |
| `EsynetEvent::m_event_type` | `long`   | `EsynetEvent::NIREAD`. |
| `EsynetEvent::m_src_id`     | `long`   | The NI id. |

`EsynetFoundation` provides `EsynetFoundation::receiveNireadMessage` method to 
handle NIREAD event. The network platform calls the NI specified by 
`EsynetEvent::m_des_id` to respond NIREAD event. NI ejects one packet out from 
the eject queue. See also NI pipeline for more information.

## Network Generator

ESYNet supports two methods to configure the network. One method is to specify 
a regular topology by options. The other method is to specify a custom topology 
by network configuration file. ESYNet can also store the regular topology 
configured by options into a network configuration file.

ESYNet provides `EsyNetworkCfg` as the network configuration structure. It can 
generate network configuration from a file as well as store network 
configuration to a file. Moreover, `EsyNetworkCfg` can generate network 
configuration from options. See network configuration for details.

### Regular Topology

ESYNet supports regular topolgies generated by options, including switch, ring, 
mesh/torus and diamension mesh/torus.

#### Switch

Switch topology always appears in micro-controllers (MCU) to connect 
peripherals together with the core. If option `-topology` is `Switch`, ESYNet 
generates a network with only one router. Hense, option `-network_size` does 
not work in this case. 

The physical channels and virtual channels are 
determined by option `-phy_number` and `-vc_number`. All physical channels 
connect with network interfaces. Hense the number of network interface is as 
same as the number of physical channels. 

Switch topology only works with table-based routing algorithm 
(`-routing_alg Table`) and wormhole switch (`-switch Wormhole`).

#### Ring

Ring topology is very common in multi-core processors. Each node connects a 
cluster of several cores, part of memories or interface to faster peripherals. 
If option `-topology` is `Ring`, ESYNet generates a ring network. The number of 
nodes is determined by option `-network_size`. Hence, option `-network_size` 
accepts only one integar number. If there is more than one number provided, 
only the first one is accepted. 

The physical channels and virtual channels are determined by option 
`-phy_number` and `-vc_number`. The physical channels of one router must be not 
lower than 3 (`-phy_number >= 3`). If option `-phy_number` is lower than 3, it 
is forced to the lower bound. Physical channels are connected according to 
following rules:
- Physical channel 0 is assigned to the network interface. Hence, the number of 
NIs is as same as the number of routers.
- Physical channel `2n+1` (e.g. 1, 3, etc.) of router `A` connects with the 
physical channel `2n+2` (e.g. 2, 4, etc.) of the router `A-1 mod router#`. For 
example, physical channel 1 of router 1 connects with physical channel 2 of 
router 0. Such channel for router 0 connects with router with largest id.
- Physical channel `2n+2` (e.g. 2, 4, etc.) of router `A` connects with the 
physical channel `2n+1` (e.g. 1, 3, etc.) of the router `A+1 mod router#`. For 
example, physical channel 2 of router 0 connects with physical channel 1 of 
router 1. Such channel for router with largest id connects with router 0.

Ring topology supports double ring but it can simulate single ring topology by 
routing algorithm. Ring topology works with two routing algorithms to simulate 
single ring and double ring respectively 
(`-routing_alg [SingleRing|DoubleRing]`). `SingleRing` routing algorithm only 
uses the physical channel 2 to forward packets. Thus, `SingleRing` routing 
algorithm can simulate single ring network. `DoubleRing` routing algorithm uses 
both physical channel 1 and 2. The packets select the direction with shorter 
distance. Thus `DoubleRing` routing algorithm can simulate double ring network.

Ring topology works with wormhole switch (`-switch Wormhole`) if the packets 
only have one flit. However, deadlock occurs if the packets have more than one 
flit. To avalid deadlock, ring topology works with another special switch 
(`-switch Ring`) which is a combination of wormhole switch and virtual-cut 
switch. The packets cannot be injected in the ring unless there is enough free 
slots in the next-stage router (like virtual-cut switch) while the packets 
within the ring can be forward in the ring or ejected from the ring if there is 
any free slots in the next-stage router (like wormhole). 

#### 2D Mesh/Torus

2D mesh topology became more and more popular in many-core processors because 
the high bandwidth. Each node connects a cluster of process elements, includes 
cores, memories or peripherals. The nodes are arranged in a rectangle. Each 
node has a router with connection to its north, south, west and east neighbor 
routers as well as local network interface. 

Coordinate can easily describe a router in a matrix. *Axis X* increases from 
west to east, which is also labeled as axis 0. *Axis Y* increases from north to 
south, which is also labeled as axis 1.

Except coordinate, the routers are labeled by unique id from the north-west to 
the south-east. Take 8x8 mesh/torus network as example. The routers on the 
northest row is labeled as router 0 to router 7 from west to east; the routers 
on the second north row is labeled as router 8 to router 15 from west to east; 
and so on. The routers on the southest row is labeled as router 56 to router 
63.

If option `-topology` is `2DMesh`, ESYNet generates a 2D mesh network. If 
option `-topology` is `2DTorus`, ESYNet generates a 2D torus network. The 
number of nodes is determined by option `-network_size`. Hence, option 
`-network_size` must have two integar numbers. First number specifies the 
number of router on X axis; second number specifies the number of router on Y 
axis. If there is more than two numbers provided, only the first two are 
accepted. If there is only one number provides, ESYNet assume the network forms 
square. The number of routers on X axis is as same as the number of routers on 
Y axis.

The physical channels and virtual channels are determined by option 
`-phy_number` and `-vc_number`. The physical channels of one router must be not 
lower than 5 (`-phy_number >= 5`). If option `-phy_number` is lower than 5, it 
is forced to the lower bound. Physical channels are connected according to 
following rules:
- Physical channel 0 is assigned to the network interface. Hence, the number of 
NIs is as same as the number of routers.
- Physical channel `4n+1` and `4n+2` (e.g. 1/2, 5/6, etc.) of one router are 
assigned to X axis. Physical channel `4n+3` and `4n+4` (e.g. 3/4, 7/8, etc.) of 
one router are assigned to Y axis.
- Physical channel `2n+1` (e.g. 1, 3, 5, etc.) connects to the down router on 
the assigned axis; physical channel `2n+2` (e.g. 2, 4, 6, etc.) of router A 
connects to the up router on the assigned axis.
- The difference between mesh and torus lies on that mesh topology do not have 
connection between the router on boundary while torus topology provides the 
connection between the routers on boundary to reduce distance between routers.

2D Mesh topology supports XY and DyXY routing algorithm 
(`-routing_alg [XY|DyXY]`). DyXY routing algorithm need two virtual channels on 
physical channel 1 and 2. 2D Torus topology supports TXY routing algorithm 
(`-routing_alg [TXY]`).

Mesh topology works with wormhole switch (`-switch Wormhole`). Meanwhile torus 
topology also works with wormhole switch if the packets only have one flit. 
However, deadlock occurs if the packets have more than one flit for torus 
topology. To avalid deadlock, torus topology only support the packet with 1 
flit. If option `-packet_size` is larger than 1, the packet injection rate is 
multiplied with the size of packet and the size of packet is forced to 1. As a 
result, the total injected flit is same.

#### Diamension Mesh/Torus

Diamension mesh and diamension torus is an abstraction of pipeline bus, ring, 
mesh, torus and cube topology. It is majorly used to simulate 3D topology, 
which is very popular in researches since 3D stack technology becomes more and 
more realiable. 

If option `-topology` is `DiaMesh` or `DiaTorus`, ESYNet generates a mesh or 
torus network with multiple diamensions. The reason to name with diamension 
lies that the routers are labeled by coordinate. The number of integars 
provided for option `-network_size` determines the number of diamension in the 
topology. Diamension is labeled by number since 0 as well.

Except coordinate, the routers are labeled by unique id. Router id is also 
labeled along from lower diamension to higher diamension. Diamension 1 is 
higher than diamension 0 but lower than diemaneion 2. Taken 4x4x4 mesh/torus 
network as example. The router 0 to router 3 has the same location on 
diamension 1 and 2; the router 0 to router 15 has the same location on 
diamension 2. 

The physical channels and virtual channels are determined by option 
`-phy_number` and `-vc_number`. The physical channels of one router must be not 
lower than `2 * dia# + 1`, so that each diamension has at least 2 ports for up 
and lower direction. Physical channels are connected according to following 
rules:
- Physical channel 0 is assigned to the network interface. Hence, the number of 
NIs is as same as the number of routers.
- Left physical channels are assigned to axis circularity. Every time, one axis 
are assigned with 2 continuous ports. To compatibility with 2D mesh/torus, 
ports are assigned from highest diamension to lowest diamension. Take 4x4x4 
mesh/torus network as example. Such network must has not less than 7 physical 
channels. Physical channel `6n+1` and `6n+2` are assigned to axis 2; physical 
channel `6n+3` and `6n+4` are assigned to axis 1; and physical channel `6n+4` 
and `6n+5` are assigned to axis 0.
- Physical channel `2n+1` (e.g. 1, 3, 5, etc.) connects to the down router on 
the assigned axis; physical channel `2n+2` (e.g. 2, 4, 6, etc.) of router A 
connects to the up router on the assigned axis.
- The difference between mesh and torus lies on that mesh topology do not have 
connection between the router on boundary while torus topology provides the 
connection between the routers on boundary to reduce distance between routers.

ESYSim provides a general routing algorithm for diamension mesn and torus 
(`-routing_alg [DiaMesh|DiaTorus]`).

Like 2D mesh and 2D torus, diamension mesh topology works with wormhole switch 
(`-switch Wormhole`). Meanwhile diamension torus topology works with wormhole 
switch but packets can only have 1 flit.

### Custom Topology

ESYNet supports custom topology specified by network configuration file. The 
network configuration file is organized in XML format. Network configuration 
file can a tree topology. Moreover, such file can also describe regular 
topology with irregular routers.

To load network configuration from file, following options should used:
```sh
-network_cfg_file_enable -network_cfg_file_name <file path without extension>
```
Some configuration options does not store in network configuration file. 
```sh
-data_path_width <integar> -link_length <double> -routing_alg [SingleRing|DoubleRing|XY|TXY|DyXY|Table|DiaMesh|DiaTorus] -routing_table <file path with extension> -arbiter [Random|RR|Matrix] -switch [Wormhole|Ring] -ni_buffer_size <integar> -ni_read_ready <integar>
```
Other configuration options do not work.

Custom topology does not work with all routing algorithms and switch methods. 
But no limitation is set on options. If the custom topology describes a tree 
network, table-based routing algorithm should be selected. If the custom 
topology is a regular topology with irregular routers, it is possible to use 
other built-in routing algorithms.

### Store Network Configuration

For the network specified by options, ESYNet can store the configuration to 
network configuration file.
```sh
-network_cfg_out_file_enable -network_cfg_file_name <file path without extension>
```

## Simulation Control

There are several ways to control the simulation procedure. 

### Simulation Length

Option `-sim_length` determines the total simulation cycle. Simulation length 
is the base method to control the simulaton. Without other setup, the 
simulation lasts as long as option `-sim_length`. 
```sh
-sim_length <double>
```

### Injected Packets

The second way is injected packets. Option `-injected_packet` determines the 
total number of packets during the simulation. Network platform counts the 
injected packets and accepted packets every cycle. If the number of injected 
packets reaches `-injected_packet`, the traffic generator stops. If all 
injected packets have been accepted, the simulation stops. 
```sh
-injected_packet <integar>
```

If the specified value is `-1`, the simulation is not controlled by injected 
packets.

### Latency and Throughput Measurement

The third way to control the simulation is latency and throughput measurement. 

By default, the simulation provides the average latency and average throughput. 
Average latency equals to the ratio of the sum of latency of accepted packets 
and the number of accepted packets. Average throughput equals to the ratio of 
the number of accepted packets and the simulation cycle. However, the average 
latency and average throughput are not very accurate because the latency of 
packets and throughput of network increases since the flits accumulate at the 
begin of simulation. The duration is called *warming-up phase*. Then, latency 
and throughput maintains stable, which can be called *stable phase*. At last, 
after all packets has been injected from the network, the latency and 
throughput decreases. The duration of last phase call be called 
*retired phase*.

The standard procedure to measure latency and throughput should pick a duration 
from the stable phase to get the accurate value. Option `-warmup_packet` 
specifies the number of packets to skip during warming-up phase. Option 
`-latency_measure_packet` specifies the number of packets marked for latency 
measurement. Option `-throughput_measure_packet` specifies the number of 
packets counted for throughput measurement. If option `-latency_measure_packet` 
is -1, the latency measure is disabled. If option `-throughput_measure_packet` 
is -1, the throughput measure is disabled.
```sh
-warmup_packet <integar> -latency_measure_packet <integar> -throughput_measure_packet <integar>
```

After the number of injected packet reaches the option `-warmup_packet`, 
standard measurement procedure for latency starts. Later injected packets are 
marked for latency measurement. The number of marked packets is determined by 
option `-latency_measure_packet`. After all marked packets have been accepted, 
the measurement procedure finishes. The network latency equals to the totally 
latency of all marked packets divides the number of marked packets. 

After the number of accepted packet reaches the option `-warmup_packet`, 
standard measurement procedure for throughput starts. The start time is 
recorded. Then, when the number of accepted packet beyonds the sum of option 
`-warmup_packet` and `-throughput_measure_packet`, the measurement procedure 
stops. The stop time is recorded as well. The difference between the recorded 
start time and stop time illustrates how long the network can accept enough 
packets specified by `-throughput_measure_packet`. The network throughput 
equals to the accepted packets during the recorded time divides the recorded 
time.

Because the Network platform updates the status of measurement every cycle. 
Thus, the number of actual marked packets may be larger than option 
`-latency_measure_packet` while the number of actual recorded packets for 
throughput measurement may also be larger than option 
`-throughput_measure_packet`.

If latency measure and throughput measure are both enabled 
(`-latency_measure_packet >= 0` and `-throughput_measure_packet >= 0`), the 
simulation is controlled by the measurement procedure. If both measurement 
procedure of latency and throughput finish, the simulation stops because the 
important results have provided. If either latency measure or throughput 
measure is enabled, the simulation does not controlled by the measurement 
procedure.

## Traffic Generator

ESYNet provides `EsynetPacketGenerator` to inject packets into the network in 
two ways. First, ESYNet can inject packets according to traffic profiles. 
Options select the traffic profile and specifies the packet injection rate. 
Second, ESYNet can inject packets based on benchmark trace file which is 
specified by options as well. 

### Traffic Profile

ESYNet can generate packets according to selected traffic profile if neither 
option `-traffic_injection_disable` nor option `-input_trace_enable` appears. 
The traffic profile is selected by option `-traffic_rule`, the traffic 
packet injection rate is determined by option `-traffic_pir`, and the size of 
packets is determined by option `-packet_size`.
```sh
-traffic_rule [Uniform|Transpose1|Transpose2|Bitreversal|Butterfly|Shuffle] -traffic_pir <double> -packet_size <integar>
```
Traffic profiles specifies certain rules to generate the source and destination 
address for packets. ESYNet implements six traffic profiles as below.

| Profile | Note |
| ---- | ---- |
| Uniform     | Select source id and destination id randomly. |
| Transpose1  | From NI (x,y) to NI (network size - 1 - y, network size - 1 -x). |
| Transpose2  | From NI (x,y) to NI (y,x). |
| Bitreversal | Destination NI id is the bit reversal of source NI id. |
| Butterfly   | Get destination id by swaping the LSDB and MSB of source id. |
| Shuffle     | Destination id is the right loop shift of source id. |

Uniform rules works for all topologies because it does not depends on the 
geometry transform. Other rules work on 2D mesh and torus topology which one 
router only connects with one NI. The coordinate of NI in the table is as same 
as the coordinate of connected router.

The source NI are selected uniformly among the router. The destination address 
are determined from the source address except `Uniform` profile. The traffic 
generated by `Uniform` profiles covers all possible pathes and the traffic on 
these paths are also unform. The traffic generated by other profiles only 
covers a part of possible pathes. For example, the traffic of Transpose1 and 
Transpose2 only covers the pathes from top-left to bottom-right or from 
top-right to bottom-left. The traffic profiles except `Uniform` are used to 
study the network performance under certain kind of applications.

ESYNet generates packets following a binomial distribution with constant 
probility. The probaility dominates the number of packet injected per cycle per 
NI, which is called packet injection rate (PIR). Option `-traffic_pir` 
determines the PIR for simulation. ESYNet cannot generate more than one packets 
for one NI in one cycle. Thus, the value of `-traffic_pir` ranges from 0.0 to 
1.0. If PIR is 0.0 packet/cycle/NI, no packet is injected actually. If PIR is 
1.0 packet/cycle/NI, each NI injects one packet every cycle. If option 
`-traffic_pir` is higher than 1.0, the actual PIR is 1.0. 

Option `-packet_size` determines the number of flits in one packet. Therefore, 
the flit injection rate (FIR) of entire network equals to the product of PIR of 
one NI, the total number of NI, and the size of packet. 

`EsynetPacketGenerator::generatePacket` are called at every cycle to generate 
packets for each NI. It is called when Network platform handle `ROUTER` events 
with pipeline time as 1.0. One uniform distributed random number is generated 
and compared with PIR. If the random value is lower than PIR, one new packet is 
generated. Otherwise, no packet is generated.

`EsynetPacketGenerator` is not a simulation components. Thus, 
`EsynetPacketGenerator::generatePacket` returns the list of injected packets 
rather than injects them into the event queue.

### Input Benchmark Trace

`EsynetPacketGenerator` can generate packets according to benchmark trace if 
option `-input_trace_enable` appears. 

Benchmark trace file (`.bencht` or `.benchb`) records the source NI, 
destination NI and size of the packets in the order of injecion time. As all 
the trace in ESYSim, benchmark trace can be in text format (`.bencht`) as well 
as binary format (`.benchb`). `EsyDataFileIStream< EsyDataItemBenchmark >` can 
parse the benchmark trace file (See also [Trace file interface]). Each record 
in benchmark trace has following fields:

| Field | Type | Note |
| ---- | ---- | ---- |
| `EsyDataItemBenchmark::m_time` | `double` | Injection time in cycle. The time when packets are injected into NI. |
| `EsyDataItemBenchmark::m_src`  | `short`  | Source NI. |
| `EsyDataItemBenchmark::m_dst`  | `short`  | Destiniation NI. |
| `EsyDataItemBenchmark::m_size` | `short`  | Packet size in flit. |

The benchmark trace file is specified by option `-input_trace_file_name`. It is 
not necessary to provide file extension name because input stream adds the file 
extension name according format. The benchmark trace file is in binary format 
by default. The file is in text format if option 
`-input_trace_file_text_enable` appears. option `-input_trace_buffer_size` 
specifies the buffer size for the input stream to reduce the file syscall and 
the execution time.
```sh
-input_trace_enable -input_trace_buffer_size <integar> -input_trace_file_name <file path without extension>
```
or
```sh
-input_trace_enable -input_trace_file_text_enable -input_trace_buffer_size <integar> -input_trace_file_name <file path without extension>
```

Becnhmark trace file can make sure that each simulation uses exactly same 
traffic so that the effect of random algorithm can be avoid. Moreover, 
benchmark trace can record the traffic of real application, e.g. PARSEC 
benchmarks, so that the simulation can be close to the real situation. 
Moreover, benchmark trace of real application can have variation in 
time-domain. For example, the packet injection rate can be higher in 
some period than other period. Traffic profile cannot perform such variation. 
At last, benchmark trace can be used generate special traffic for special case. 
For example, add high traffic stream on specified paths to create deadlock.

Benchmark trace file can be dumpped from a full system simulator, like ESYSim. 
ESYSim can dump the benchmark trace when real applications run. Also, benchmark 
trace in text format can be edit by hand to create special traffic.

`EsynetPacketGenerator` provides the pointer 
(`EsynetPacketGenerator::m_tracein`) to input stream 
`EsyDataFileIStream< EsyDataItemBenchmark >`. In the function 
`EsynetPacketGenerator::generatePacket`, the traffic generator reads records 
from benchmark trace file and returns the packets which should be injected not 
later than current simulation cycle.

## Network Platform

ESYNet provides `EsynetFoundation` as the network platform. The network 
platform collects routers and NIs in the network. In the construction function 
`EsynetFoundation::EsynetFoundation`, routers and NIs are built according the 
network configuration `EsynetConfig`. Configuration can be specified by either 
options or a network configuration file.

Routers are stored in STL vector `EsynetFoundation::m_router_list` and NIs are 
stored in STL vector `EsynetFoundation::m_ni_list`. The index in the vector is 
the id of the router or the NI. If one variable presents the id of both router 
and NI, the NIs are labeled after routers. For example, for 4x4 network, 0-15 
present routers and 16-31 present NIs.

The event queue calles `EsynetFoundation` to handle events. Event queue 
provides different founds to handle different events.
- `EsynetFoundation::receiveEvgMessage` handles `EVG` events. This function 
calls the NI specified by `EsynetEvent::srcId` to inject one packet. If the 
`EVG` event does not specified packet id, this function assignes one to the new 
packets. Moreover, the packet generator only specifies the source and 
destination NI but routers forward packets according to the source and 
destination router. This function does such transform. 
- `EsynetFoundation::receiveRouterMessage` handles `ROUTER` events. This 
function calls the routers and NIs to perform pipeline operation. If the 
pipeline period specified by `ROUTER` event is 1.0, all NIs are called to 
perform pipeline. The routers with the pipeline period of 1.0 are called as 
well. Meanwhile, packet generator, statistics unit, and information propagation 
are called. If the pipeline period specified by `ROUTER` event is not 1.0, only 
the rouers with the same pipeline period as `ROUTER` event are called.
- `EsynetFoundation::receiveWireMessage` handles `WIRE` events. This function 
calls the router or NI specified by `EsynetEvent::desId` to inject the flit in 
`EsynetEvent::flit`. Meanwhile, this function calls the router or NI specified 
by `EsynetEvent::srcId` to clear the status of link. `EsynetEvent::srcId` and 
`EsynetEvent::desId` encodes the id of routers and NIs in one variable.
- `EsynetFoundation::receiveCreditMessage` handles `CREDIT` events. This 
function calls the router or NI specified by `EsynetEvent::desId` to set the 
credit specified by `EsynetFlit::flitId` of `EsynetEvent::flit`.
`EsynetEvent::srcId` and `EsynetEvent::desId` encodes the id of routers and NIs 
in one variable.
- `EsynetFoundation::generateNIReadEvent` handles `NIREAD` events. This 
function calls the NI specified by `EsynetEvent::srcId` to accept one packet.

Network platform also provides the traffic generator 
`EsynetFoundation::m_packet_generator` to generate new packets according 
traffic profiles specifed by options or the benchmark trace file. The traffic 
generator is called only once every cycle so that packets can be injected in 
the correct time. 

Network platform also provides the statistics unit 
`EsynetFoundation::m_statistic` to control latency and throughput measure 
procedure. The statistics unit counts the injected and accepted packets every 
cycle and updates the measurement status. See [](). 

Esynet emplies several hierarchy statistics unit. Each router and NI has their 
own statistics unit. Power module for each router also has statistics 
variables. When the simulation finish, the statistics unit of network platform 
collects the statistics variables from routers, NIs and power modules to get 
the statistics result for the entire network. 
`EsynetFoundation::simulationResults` collects statistics results and print 
results to standard output. 

Because Esynet does not provides global pointers of routers and NIs, which 
means one router or NI cannot access another router or NI directly. The 
information propagation between routers and NIs must be handled by network 
platform. `EsynetFoundation::informationPropagate` is used to copy variables 
from one router or NI to another router or NI. Currently, this function is 
empty. The major concern of such design lies that users should have a intuitive 
way to find out how much information are delivered between routers and NIs and 
how much hardware overhead to propagate such information.

## Router Module

ESYNet provides `EsynetRouter` to model a traditional 5-stage pipeline router.

### Router Architecture

`EsynetRouter` has several input and output physical channels. The number of 
input ports and output ports are same. The number of physical ports can be 
specified by option `-phy_number` or by a network configuration file. 
`EsynetRouter` provides two STL vector to store input and output ports, 
`EsynetRouter::m_input_port` and `EsynetRouter::m_output_port`. 

Input ports have several virtual channels. The number of virtual channels for 
one physical channel can be specified by option `-vc_number` or a network 
configuration file. By default, each physical channel has one virtual channel.
Input virtual channel holds a input buffer. The size of input buffer can also 
be specified by option `-in_buffer_size` or a network configuration file. 

Output ports does not have virtual channels. The output port can also have a 
buffer specified by `-out_buffer_size` or a network configuration files. But, 
the output buffer are not used currently. Output ports also hold the status of 
the connected link.

Each router has one crossbar to deliver packets between input ports and output 
ports. The size of the crossbar is determined by the physical channel. Thus, 
only one virtual channel from one physical channel can occupy the crossbar.

`EsynetRouter` provides a statistics unit which is empty currently. 

### Pipeline

`EsynetRouter` describes a tranditional 5-stage pipeline router. When one flit 
is accepted by the router, the flit is stored into the input buffer. When the 
flit reaches the head of input buffer, it needs 5-step before the flit leaves 
the network. The status of pipeline is presented by the status of input virtual 
channel `esynet::EsynetVCStatus`.

The initializattion status of one virtual channel is `esynet::VCS_INIT`. When 
the head flit arrives at the top of input buffer, the status changes to 
`esynet::VCS_ROUTING`. 7

`EsynetRouter::routingDecision` calculates the forward direction (next-stage 
router and virtual channel) for all virtual channel with status of 
`esynet::VCS_INIT`. This stage is called *Routing Calculation* (RC). Then the 
status changes to `esynet::VCS_VC_AB`.

`esynet::VCS_VC_AB`

`esynet::VCS_SW_AB`

`esynet::VCS_SW_TR`

`EsynetRouter::routerSimPipeline` call the function of each stage to perform 
the pipeline. 

### Routing Algorithm

`EsynetRouter` provides the pointer to the function of routing algorithm. These 
functions must follow the same declartion format as a member of `EsynetRouter`.
```c++
void (EsynetRouter::*m_curr_algorithm)(long des, long src, long s_ph, long s_vc);
```
The routing algorithm determines the output port and the virtual channel in 
next router based on the source, destination and current router. Moreover, 
the status of routers, the input virtual channel and physical channel can be 
used in the routing algorithm. For eaxmple, DyXY routing algorithm uses credit 
to select the direction with less congestion. 

The routing decision are stored by `EsynetRouter::addRouting`. If the routing 
decision are not registered by `EsynetRouter::addRouting`, the packet cannot be 
forward.

#### Single Ring

Single ring routing algorithm (option `-routing_alg` is `SingleRing`) only uses 
one direction in the ring. Except the packets which have arrived at destination 
router, other packets are all forward to up direction (router with larger id). 
Packets can be assigned to all virtual channels. 
See `EsynetRouter::algorithmSingleRing`.

#### Double Ring

Doule ring routing algorithm (option `-routing_alg` is `DoubleRing`) uses both 
up direction (routers with larger id) and down direction (routers with smaller 
id). Packets select direction according to the distance between the source and 
destination on different directions. Packets can be assigned to all virtual 
channels.
See `EsynetRouter::algorithmDoubleRing`.

#### XY 

XY routing algorithm (option `-routing_alg` is `XY`) is most basic 
deterministic routing algorithm for 2D mesh topology. Packets go along the X 
axis first until the router with same row of the source router and same column 
of the destination router. Then packets turn to Y axis and forward until the 
destination router. XY is a deadlock-free routing algorithm. Packets can be 
assigned to all virtual channels.
See `EsynetRouter::algorithmXY`.

#### TXY 

TXY routing algorithm (option `-routing_alg` is `TXY`) is very similar to XY. 
Packets go along the X axis first until the router with same row of the source 
router and same column of the destination router. Then packets turn to Y axis 
and forward until the destination router. The difference lies in that axis, 
packets can select the direction on X or Y axis with shorter distance because 
torus topology provides the links between the routers on both boundary. Packets 
can be assigned to virtual channel 0.
See `EsynetRouter::algorithmTXY`.

#### DyXY

DyXY routing algorithm (option `-routing_alg` is `DyXY`) is a dynamic 
congestion-aware routing algorithm. If the destination router is on the north, 
south, west, or east of current router, packets forward to the destination 
router directly. If the destination router is on the north-west, north-east, 
south-west, or south-east of current router, packets have two possible 
directions. Routing algorithm selects the direction with less congestion which 
means higher credit value. 
See `EsynetRouter::algorithmDyXY`.

DyXY is not a deadlock-free routing algorithm natually. To avoid deadlock, 
the north port and south port must have two virtual channels. Eastern packets 
(the destination router is on the east, north-east and south-east of the source 
router) are assigned to virtual channel 0 on north and south direction. Western 
packets (the destination router is on the west, north-west and south-west of 
the source router) are assigned to virtual channel 1 on north and south 
direction. 

#### Diamension Mesh 

Diamension mesh routing algorithm (option `-routing_alg` is `DiaMesh`) is the 
extension of XY routing algorithm. Packets are move along each axis until the 
router with same index on the axis.  Packets can be assigned to all virtual 
channels.
See `EsynetRouter::algorithmDiaMesh`.

Take the packet from router 0 to router 27 in 4x4x4 mesh network as example. 
The coordinate of the source and destination router is (0,0,0) and (3,2,1) 
respectively. The first number in the turple is the index on axis 0, and so on. 
The packet starts from router (0,0,0), and move along the axis 0 to router 
(3,0,0). Then, the packet turns to axis 1, and move to router (3,2,0). At last, 
the packet turns to axis 2, and move to router (3,2,1).

#### Diamension Torus

Diamension torus routing algorithm (option `-routing_alg` is `DiaTorus`) is the 
extension of TXY routing algorithm. Similarly with diamension mesh routing 
algorithm, Packets are move along each axis until the router with same index on 
the axis. The difference lies in that axis, packets can select the direction on
axis with shorter distance because torus topology provides the links between 
the routers on both boundary.  Packets can be assigned to all virtual channels.
See `EsynetRouter::algorithmDiaTorus`.

#### Table-based

Table-based routing algorithm (option `-routing_alg` is `Table`) is used to 
define a deterministic routing algorithm for any topology. It presents the 
routing algorithm in a look-up table (`EsynetRouter::m_routing_table`), which 
is indexed by the destination router and stores the output ports and virtual 
channels. 

```C++
std::vector< std::vector< std::pair< long, esynet::EsynetVC > > >
```
The routing table has two label of index. The first index is the destination 
router. The second index is different possible forward direction based on 
source address. The first field of items in the table is the source router and 
the second field of items in the table is forward direction. If the direction 
is not related to the source router, the first field should fill with `-1`. 
Otherwise, the first field should fill with the source router. 
See `EsynetRouter::algorithmTable`.

The routing table is specified by option `-routing_table`. The routing table 
file is a text file. Each line in the text file presents one item in the 
routing table. Each line has following fields:

- The router id. Specifies the router of this item. It must be a valid router 
id.
- The destination router. It must be a valid router id.
- The source router. If the forward direction are not related to the source 
router, this field can be `-1`. Otherwise, it must be a valid router id.
- The output physical channel. 
- The input virtual channel in next router.

It must be addressed that the source address of items with the same destination 
router must cover all the source router. Therefore, there is either one item 
with source router as `-1` to specify the same forward address for all source 
routers, or there are items as same as the number of routers to specify the 
forward address for all source routers individually. 

### Arbiter

Arbiters are implemented in the routers for virtual channel allocation and 
switch arbitration. There are several popular arbiters. See "Principles and 
Practices of Interconection Networks" for defail introduction.

ESYNet provides `EsynetArbiter` to implement an arbiter. Each instance of 
`EsynetArbiter` corresponding to an abiter.

`EsynetArbiter` provides three kinds of arbiters, including random arbiter, 
Round-Robin arbiter, and matrix arbiter (ARBITER_MATRIX). The type of arbiter 
is determined by option `-arbiter`. The arbitration algorithm and the size of 
arbiter is determined by the construction function.

The arbiter makes the grant according to the request and the status of the 
arbiter. The status of the arbiter is stored in `EsynetArbiter` and the request 
is set by `EsynetArbiter::setRequest` function. `EsynetArbiter::setRequest` can 
set the request by a vector of boolean signals reg or set the request of the 
signal specified by a. `EsynetArbiter` gives out the index of granted signals 
by `EsynetArbiter::grant` function.

Grant function also updates the status of the arbiter. Even with the same 
request, the output of two time of grant functions can be different. In the 
view of one arbiter, it only recognizes the request signals. But in the view of 
the router, the arbiter makes decision among virtual channels or physical 
channel. Thus, `EsynetArbiter` provides the ability to make connection between 
virtual channels and the request signals. According to the map between virtual 
channels and request signals, `EsynetArbiter` can set the request from one 
virtual channel as well as give out the virtual channel which gets grant 
directly.

`EsynetRouter` provides four groups of arbiters. 
`EsynetRouter::m_vc_input_arbiter` and `EsynetRouter::m_vc_output_arbiter` are 
used in virtual channel allocation.
- `EsynetRouter::m_vc_input_arbiter` finds out the grant for the virtual 
channels require the same virtual channel in the neighbor router. The 
requirements are provided by routing algorithm.
- `EsynetRouter::m_vc_output_arbiter` finds out the virtual channel in the 
neighbor router which should be assigned with the virtual channel. The 
requirements are provided by the result of `EsynetRouter::m_vc_output_arbiter`. 
If a packet can be forward to multiple directions (different virtual channels 
or physical channels), it is possible that the packet gets multiple grants from 
virtual channel in the neighbor routers. Thus, it is necessary to select one 
from these grant.

`EsynetRouter::m_port_input_arbiter` and `EsynetRouter::m_port_output_arbiter` 
are used in switch arbitration.
- `EsynetRouter::m_port_input_arbiter` finds out which input virtual channel 
can be delivered through crossbar.
- `EsynetRouter::m_port_output_arbiter` finds out which physical channel can 
use link.

#### Random arbiter

Random arbiter chooses one request randomly. The choose is made by random 
generator `EsynetSRGenFlatLong`. Random arbiter does not store any status. 
Grant clears all request signals. If there is no valid request signal, grant 
return `-1`.

#### Round-Robin arbiter

Round-Robin arbiter loops all the request signals in a certain order until a 
valid request signal. For the next time, it loops from the grant signal of last 
time. In this way, all the request signals have their chance to be assigned 
highest priority. The fairness of Round-Robin arbiter is weaker than matrix 
arbiter, but it sill be reasonable acceptable. Moverover, Round-robin arbiter 
is simpler.

`EsynetArbiter` stores the last grant signal in 
`EsynetArbiter::m_state_vector`, which is a 1-D vector of boolean. The item for 
last grant signal is set by True.

Grant function loops all the request signals from the last grant signal stored 
in `EsynetArbiter::m_state_vector`. The first valid request signal is assigned 
as grant signal. If there is no valid request signal, grant return `-1`.

`EsynetArbiter` provides the ability to assign highest or lowest priority to a 
port by changing the value of D-FF. If one signal is set with highest priority, 
the loop for next arbitration starts from the special signal; if one signal is 
set with lowest priority, the loop for next arbitration starts from the next 
signal to the special signal. It is very critical that the assigned priority 
only valid for next time of arbitration because the granted request signals 
must be assigned with lowest priority after arbitration. Thus, the user-defined 
priority should be set again after each arbitration.

#### Matrix arbiter

Matrix arbiter is named after the matrix array of D-FF to store the arbiter 
register. The D-FF in row `i` and column `j` indicates that request `i` takes 
priority over request `j`. The main idea behinds the allgorithm is that the 
last granted signals should be assigned to the lowest priority while the 
priority of other request signals keep the same. The details and example of 
matrix arbiter can be found in book "Principles and Practices of Interconection 
Networks".

`EsynetArbiter` stores the status of D-FF in matrix array in 
`EsynetArbiter::m_state_matrix`, which is a 2-D matrix of boolean. 

Grant function loops all the request signals to find the grant signals meet 
following condition: no valid request signal has higher priority over the grant 
signal. In other word, if the grant signal is `a`, 
`m_state_matrix[i][a]` for valid request `i` must be all False. If no request 
signal meets such a condition, grant funcion returns `-1`.

After finding the grant signals, grant function updates D-FF. Grant function 
clears all the D-FF in the same row as the grant signals and set the D-FF in 
the same column as the grant signals. In this way, the priority of the grant 
signal drops the lowest.

`EsynetArbiter` provides the ability to assign highest or lowest priority to a 
port by changing the value of D-FF. It is very critical that the assigned 
priority only valid for next time of arbitration because the granted request 
signals must be assigned with lowest priority after arbitration. Thus, the 
user-defined priority should be set again after each arbitration.

### Switch

ESYNet supports a typical wormhole switch methods. Different from the 
store-and-forward and virtual-cut method, packets can be forward to next router 
as long as the packets arrive at the top of input buffer. One packet can be 
distributed into several routers. The key points to implement wormhole switch 
methods as follow.

- Head flit builds up a channel between the input virtual channel and the 
input virtual channel in next router. Other flits follow the channel. The 
channel is released after the tail flits have been delivered. 
- One link between routers can be shared by several virtual channels. If one 
virtual channel is blocked due to flow control, other virtual channel can 
occupy the link.

Wormhole switch methods work effectively for the topology without ring (torus 
topology can be seen as a collection of rings). Moreover, wormhole switch 
methods work for all topology if packets only have 1 flit. However, if packets 
have more than 1 flit, wormhole switch leads to deadlock in the topology with 
ring. 

ESYNet provides another switch methods which is called `Ring` for the topolgy 
with ring. The packets on the ring are switched as wormhole switch method. If 
they arrive the destination, they can be forwarded to network interface as 
wormhole switch method as well. However, the packets from network interface 
must wait until there is enough space to accept the packets on the ring. For 
eaxmple, a new packet from a network interface has 5 flits. Such a packet is 
blocked in the input buffer of physical port 0 until the input buffer in the 
next router has 5 free entries. Then, the new packet can get grant and the 
router forwards the packet into the ring. The router gets the number of free 
entires in the input buffer of neighbor router by credit, as seen 
[Flow Control](#Flow Control).

### Flow Control

ESYNet provides credit-based flow control. Credit means the free slots in the 
input buffers of neighbor routers. The credit value is hold in the output 
ports. Each virtual channel has their own credit value. 

The initialization value of credit is as same as the size of input buffer. 
When the router sends out one flit to its neighbor, credit decreases by 1 
because the delivered flit wil occupy one entry in the buffer. When the 
neighbor router moves one flit from the input port to the output port, the 
neighbor router generates one `CREDIT` event with the new credit value because 
the delivered flit will free one entry in the buffer.

Credit is used in several places in the pipeline. First, credit is used by 
congestion-awared routing algorithm, like DyXY. In DyXY, packets goes to NE, 
NW, SE and SW have two possible directions. The direction with larger credit is 
select because the direction has less congestion.

Second, during switch arbitration, credit is used to check whether there is 
enough free slot in the neighbor router to accept a new flit. If there is only
1 virtual channel in the input port in the neighbor router, flits are blocked 
if the credit is 0. If there are more than 1 virtual channels in input ports in 
the neighbor router, the flits assigned to the virtual channel with credit 
value of 0 are skipped during switch arbitration and flits assigned to other 
virtual channel can still deliver through the crossbar and the link.

Third, during virtual channel allocation, credit is used to block the packets 
from network interface if there is not enough free slots in the ring. To avoid 
deadlock, packets cannot be injected into the ring unless there is enough space 
to accept the whole packets. Only if the credit value is larger than or equal 
to the size of a new packet from network interface, the packet can get grant. 

### Power Module

`EsynetRouter` also provides a power unit based on Orion 2.0. The power unit 
accumlates the energy consumption during the simulation. At the end of 
simulation, the power consumption is calculated by diving the energy 
consumption with simulation time.

ESYNet accumulates five different events, including writing input buffer, 
reading input buffer, crossbar traversal, link traversal and arbitration. The 
power consumption is calculated by the formula `$E = \alpha C V^2$`. 
`C` and `V` is constant value related to technology and circuit architecture. 
It is not suppest to change the `C` and `V` defined in `esynet_global.h`. 
`\alpha` is calculated by compare the signal with previous time. One changed 
signal leads to a piece of power consumption, otherwise, signal consumpts no 
energy. 

## Network Interface Module



## Benchmark Trace and Event Trace

### Output Event Trace

Event queeu can generate the event trace to record all flit movement and status 
updated during simulation. The event queue does not only contains the events 
which drive the simulation (`EVG`, `ROUTER`, `NIREAD`, `WIRE`, and `CREDIT`), 
but also other events to replay the simulation. The function to dump event 
trace is only available if option `-event_trace_enable` is enabled. 

Option `-event_trace_file_name` specifies the output event file. It is not 
necessary to provide file extension name because output stream will add the 
file extension name. Event queue dumps the event trace in binary format. If 
option `-event_trace_file_text_enable` appears, ESYNet generates two files 
with same file name but different file extension name. Option 
`-event_trace_buffer_size` specifies the buffer size for output stream.
```sh
-event_trace_enable -event_trace_buffer_size <integar> -event_trace_file_name <file path without extension>
```
or
```sh
-event_trace_enable -event_trace_file_text_enable -event_trace_buffer_size <integar> -event_trace_file_name <file path without extension>
```

### Output Benchmark Trace
Event queue can generate the benchmark trace for other simulation. The function 
to dump benchmark trace is only available if option `-input_trace_enable` is 
disabled. Thus, event queue can dump the benchmark trace of traffic generated 
by traffic profiles or injected by integration API.

Option `-output_trace_file_name` specifies the output benchmark file. It is not 
necessary to provide file extension name because output stream will add the 
file extension name. Event queue dumps the benchmark trace in binary format. If 
option `-output_trace_file_text_enable` appears, ESYNet generates two files 
with same file name but different file extension name. Option 
`-output_trace_buffer_size` specifies the buffer size for output stream.
```sh
-output_trace_enable -output_trace_buffer_size <integar> -output_trace_file_name <file path without extension>
```
or
```sh
-output_trace_enable -output_trace_file_text_enable -output_trace_buffer_size <integar> -output_trace_file_name <file path without extension>
```

`EsynetEventQueue` provides the pointer (`EsynetEventQueue::mp_outputtrace`) to 
output stream `EsyDataFileOStream< EsyDataItemBenchmark >`. During the 
simulation, event queue dumps the traffic by writing `ET_PACKET_INJECT` events 
to output stream. The function to create and open output stream can be found in 
construction function `EsynetEventQueue::EsynetEventQueue`; the function 
to dump traffic trace can be found in simulation function 
`EsynetEventQueue::simulator`; the function to close and delete output stream 
can be found in destruction function `EsynetEventQueue::~EsynetEventQueue`.

## Examples
