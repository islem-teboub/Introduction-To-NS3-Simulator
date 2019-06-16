///       client1 (App1)----------liaison Adhoc------Access Point (SERVER)----------liaison p2p---------- client2(App2)
///       192.168.1.2                              192.168.1.1 & 192.168.2.2                                192.168.2.1


#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodesP2P;
  nodesP2P.Create (2);

  NodeContainer wifiNodes;
  wifiNodes.Add(nodesP2P.Get (1));
  wifiNodes.Create (1);

///setting P2P link model
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devicesP2P;
  devicesP2P = pointToPoint.Install (nodesP2P);

/// Channel default configuration (modele d'atténuation et modele de délai de propagation)
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();

///physical layer model default configuration
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();

///associating channel with physical layer model
  phy.SetChannel (channel.Create ());

///mac layer model configuration
  QosWifiMacHelper A=QosWifiMacHelper::Default ();

///MAC layer attributes configuration

   Ssid ssid = Ssid ("ns-3-ssid");
   A.SetType ("ns3::AdhocWifiMac", "Ssid", SsidValue (ssid));

  WifiHelper wifi=WifiHelper::Default ();


  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, A, wifiNodes);


 ///mobility parameters
  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiNodes);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///pile protocolaire
  InternetStackHelper stack;
  stack.Install (nodesP2P.Get (0));
  stack.Install (wifiNodes);

///P2P adressing
  Ipv4AddressHelper address;
  address.SetBase ("192.168.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesP2P = address.Assign (devicesP2P);

///Adhoc adressing

  address.SetBase ("192.168.1.0", "255.255.255.0");
  address.Assign (staDevices);
  Ipv4InterfaceContainer interfaces= address.Assign (staDevices);

///routing enabling
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

///server profil
  UdpEchoServerHelper echoServer (9);//server port number = 9
///server instance
  ApplicationContainer serverApps = echoServer.Install (nodesP2P.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

///client profil
  UdpEchoClientHelper echoClient (interfacesP2P.GetAddress (1), 9);//server's adress and server's port specification
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

///client instance
  ApplicationContainer clientApps2 = echoClient.Install (nodesP2P.Get (0));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));
  ApplicationContainer clientApps1 = echoClient.Install (wifiNodes.Get (1));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
