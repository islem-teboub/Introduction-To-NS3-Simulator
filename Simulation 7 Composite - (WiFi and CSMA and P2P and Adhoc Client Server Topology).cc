
// wifistation1(client)---------wifiAP(SERVER)-----------APP2 (client)------------APP3 (client)-------------APP4 (client)----------APP5(client)

//                    wifi link               P2P link             CSMA link                       P2P link             Adhoc link

//                   10.0.1.0/24                10.0.2.0/24            10.0.3.0/24             10.0.4.0/24            10.0.5.0/24


#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/config-store-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);


NodeContainer wifistation1;
wifistation1.Create (1);

NodeContainer wifiAp;
wifiAp.Create (1);

NodeContainer nodes_p2p1;
nodes_p2p1.Add(wifiAp.Get (0));
nodes_p2p1.Create (1);

NodeContainer nodes_csma;
nodes_csma.Add(nodes_p2p1.Get (1));
nodes_csma.Create (1);

NodeContainer nodes_p2p2;
nodes_p2p2.Add(nodes_csma.Get (1));
nodes_p2p2.Create (1);

NodeContainer wifistation2;
wifistation2.Add(nodes_p2p2.Get (1));
wifistation2.Create (1);

/////////////////////////////////////////////////////WIFI////////////////////////////////////

//default configuration of canal (atténuation model and propagation delay model)
YansWifiChannelHelper channel = YansWifiChannelHelper::Default();

//default configuration of physical layer model
YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();

//associating the canal to the physical layer model
wifiPhyHelper.SetChannel(channel.Create());

//MAC layer model configuration
QosWifiMacHelper wifiApMacHelper = QosWifiMacHelper::Default();


//MAC layer attributes configuration
Ssid ssid1 = Ssid ("myssid");
wifiApMacHelper.SetType("ns3::ApWifiMac","Ssid",SsidValue(ssid1));

QosWifiMacHelper wifiStationMacHelper = QosWifiMacHelper::Default();
wifiStationMacHelper.SetType("ns3::StaWifiMac","Ssid",SsidValue(ssid1),"ActiveProbing",BooleanValue(false));

WifiHelper wifiHelper = WifiHelper::Default();
NetDeviceContainer wifiApNetDevise = wifiHelper.Install (wifiPhyHelper,wifiApMacHelper,wifiAp);
NetDeviceContainer wifiStationNetDevise = wifiHelper.Install (wifiPhyHelper,wifiStationMacHelper,wifistation1);

//mobility parameter
MobilityHelper mobility;
mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
"MinX",DoubleValue(0.0),
"MinY",DoubleValue(0.0),
"DeltaX",DoubleValue(5.0),
"DeltaY",DoubleValue(10.0),
"GridWidth",UintegerValue(3),
"LayoutType",StringValue ("RowFirst"));

mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds",RectangleValue(Rectangle(-50,50,-50,50)));
mobility.Install (wifistation1);
mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
mobility.Install(wifiAp);
//////////////////////////////////////////////////////Adhoc////////////////////////////////////

//default configuration of canal (atténuation model and propagation delay model)
YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default();

//default configuration of physical layer model
YansWifiPhyHelper wifiPhyHelper1 = YansWifiPhyHelper::Default();

//associating the canal to the physical layer model
wifiPhyHelper1.SetChannel(channel1.Create());

//MAC layer model configuration
QosWifiMacHelper wifiApMacHelper1 = QosWifiMacHelper::Default();


//MAC layer attributes configuration
   Ssid ssid = Ssid ("ns-3-ssid");
   wifiApMacHelper1.SetType ("ns3::AdhocWifiMac", "Ssid", SsidValue(ssid));
  
  WifiHelper wifi = WifiHelper::Default ();
  
  
  NetDeviceContainer staDevices;
  staDevices = wifi.Install (wifiPhyHelper1, wifiApMacHelper1, wifistation2);

  
  //mobility parameters
  
  mobility.Install (wifistation2);

/////////////////////////////////////////////////////////////////////////////////////////////


//configuration of devices

CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  
PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

 NetDeviceContainer devices_p2p1;
  devices_p2p1 = pointToPoint.Install (nodes_p2p1);

  NetDeviceContainer devices_csma;
  devices_csma = csma.Install (nodes_csma);

NetDeviceContainer devices_p2p2;
  devices_p2p2 = pointToPoint.Install (nodes_p2p2);

/////enabling routing
InternetStackHelper stack;
stack.Install(wifiAp);
stack.Install(wifistation1);
stack.Install(nodes_p2p1.Get(1));
stack.Install(nodes_csma.Get(1));
stack.Install(nodes_p2p2.Get(1));
stack.Install(wifistation2.Get(1));
////adresses
Ipv4AddressHelper ipAddressHelpers;
ipAddressHelpers.SetBase("10.0.1.0","255.255.255.0");
Ipv4InterfaceContainer wifiApInterface = ipAddressHelpers.Assign (wifiApNetDevise);
Ipv4InterfaceContainer wifiStationInterface = ipAddressHelpers.Assign(wifiStationNetDevise);

ipAddressHelpers.SetBase ("10.0.2.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces_p2p1 = ipAddressHelpers.Assign (devices_p2p1);

ipAddressHelpers.SetBase ("10.0.3.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces_csma = ipAddressHelpers.Assign (devices_csma);

ipAddressHelpers.SetBase ("10.0.4.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces_p2p2 = ipAddressHelpers.Assign (devices_p2p2);

  ipAddressHelpers.SetBase ("10.0.5.0", "255.255.255.0");
  ipAddressHelpers.Assign (staDevices);
  Ipv4InterfaceContainer interfaces= ipAddressHelpers.Assign (staDevices);

//enabling routing
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

////server's profile
UdpEchoServerHelper echoServer(9);
////server's instance
ApplicationContainer serverApps = echoServer.Install(wifiAp.Get(0));
serverApps.Start(Seconds(1.0));
serverApps.Stop(Seconds(10.0));

////client profile
UdpEchoClientHelper echoClient (wifiApInterface.GetAddress(0),9);
echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
////client's instance
ApplicationContainer clientApps1 = echoClient.Install (wifistation1.Get(0));
clientApps1.Start(Seconds(2.0));
clientApps1.Stop(Seconds(10.0));
ApplicationContainer clientApps2 = echoClient.Install (nodes_p2p1.Get(1));
clientApps2.Start(Seconds(3.0));
clientApps2.Stop(Seconds(10.0));
ApplicationContainer clientApps3 = echoClient.Install (nodes_csma.Get(1));
clientApps3.Start(Seconds(4.0));
clientApps3.Stop(Seconds(10.0));
ApplicationContainer clientApps4 = echoClient.Install (nodes_p2p2.Get(1));
clientApps4.Start(Seconds(5.0));
clientApps4.Stop(Seconds(10.0));
ApplicationContainer clientApps5 = echoClient.Install (wifistation2.Get(1));
clientApps5.Start(Seconds(6.0));
clientApps5.Stop(Seconds(10.0));


Simulator::Run();
Simulator::Destroy();
return 0;
}

