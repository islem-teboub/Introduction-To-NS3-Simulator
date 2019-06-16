///         client1 (App1)----------wifi link--------Access Point (SERVER)-------------p2p link------- client2(App2)
///           192.168.1.2                           192.168.1.1 & 192.168.2.1                            192.168.2.2

#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
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

  NodeContainer nodesP2P;
  nodesP2P.Create (2);

  NodeContainer wifiAp;
  wifiAp.Add(nodesP2P.Get (1));

  NodeContainer wifistation;
  wifistation.Create (1);


///setting P2P link
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devicesP2P;
  devicesP2P = pointToPoint.Install (nodesP2P);
///////////////////////////////////////////////////////////////////////////////////////////////////
///default configuration of channel (modele d'atténuation et modele de délai de propagation)
YansWifiChannelHelper channel = YansWifiChannelHelper::Default();

///default configuration of physical layer's model
YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();

///channel tp physical layer model association
wifiPhyHelper.SetChannel(channel.Create());

///MAC layer model configuration
QosWifiMacHelper wifiApMacHelper = QosWifiMacHelper::Default();

///MAC layer attributes configuration
Ssid ssid = Ssid ("myssid");
wifiApMacHelper.SetType("ns3::ApWifiMac","Ssid",SsidValue(ssid));

QosWifiMacHelper wifiStationMacHelper = QosWifiMacHelper::Default();
wifiStationMacHelper.SetType("ns3::StaWifiMac","Ssid",SsidValue(ssid),"ActiveProbing",BooleanValue(false));

WifiHelper wifiHelper = WifiHelper::Default();
NetDeviceContainer wifiApNetDevise = wifiHelper.Install (wifiPhyHelper,wifiApMacHelper,wifiAp);
NetDeviceContainer wifiStationNetDevise = wifiHelper.Install (wifiPhyHelper,wifiStationMacHelper,wifistation);

///Mobility parameters
MobilityHelper mobility;
mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
"MinX",DoubleValue(0.0),
"MinY",DoubleValue(0.0),
"DeltaX",DoubleValue(5.0),
"DeltaY",DoubleValue(10.0),
"GridWidth",UintegerValue(3),
"LayoutType",StringValue ("RowFirst"));

mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds",RectangleValue(Rectangle(-50,50,-50,50)));
mobility.Install (wifistation);
mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
mobility.Install(wifiAp);
////////////////////////////////////////////////////////////////////////////////////////////////////////////

///pile protocolaire
  InternetStackHelper stack;
  stack.Install (nodesP2P.Get (0));
  stack.Install (wifiAp);
  stack.Install (wifistation);

///P2P addressing
  Ipv4AddressHelper address;
  address.SetBase ("1192.168.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesP2P = address.Assign (devicesP2P);

///WiFi addressing
Ipv4AddressHelper ipAddressHelper;
ipAddressHelper.SetBase("192.168.1.0","255.255.255.0");
Ipv4InterfaceContainer wifiApInterface = ipAddressHelper.Assign (wifiApNetDevise);
Ipv4InterfaceContainer wifiStationInterface = ipAddressHelper.Assign(wifiStationNetDevise);

/// Enabling routing
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

///Server's profil
  UdpEchoServerHelper echoServer (9);//port du serveur est = 9
///Server's instance
  ApplicationContainer serverApps = echoServer.Install (nodesP2P.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

///Clients profils
  UdpEchoClientHelper echoClient (interfacesP2P.GetAddress (1), 9);//Specification of server's address and server's port
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
///Clients instances
  ApplicationContainer clientApps2 = echoClient.Install (nodesP2P.Get (0));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));
  ApplicationContainer clientApps1 = echoClient.Install (wifistation.Get (0));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
