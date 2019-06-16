//communication between 1 server and 2 client

//       [client1]----------------communication p2p----------------- [server] _______________communication csma/cd______________ [client2]

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes_p2p;
  nodes_p2p.Create (2);

  NodeContainer nodes_csma;
  nodes_csma.Create (1);
  nodes_csma.Add(nodes_p2p.Get (0));

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));


  NetDeviceContainer devices_csma;
  devices_csma = csma.Install (nodes_csma);

  NetDeviceContainer devices_p2p;
  devices_p2p = pointToPoint.Install (nodes_p2p);

  InternetStackHelper stack;
  stack.Install (nodes_p2p.Get (0));
  stack.Install (nodes_p2p.Get (1));
  
  stack.Install (nodes_csma.Get (0));

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces_csma = address.Assign (devices_csma);
  Ipv4InterfaceContainer interfaces_p2p = address.Assign (devices_p2p);

  UdpEchoServerHelper echoServer (9);//port of serveur = 9

  ApplicationContainer serverApps = echoServer.Install (nodes_p2p.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
//client p2p
  UdpEchoClientHelper echoClient_p2p (interfaces_p2p.GetAddress (1), 9);//specification of ladresse of client and the port of server
  echoClient_p2p.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient_p2p.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient_p2p.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps_p2p = echoClient_p2p.Install (nodes_p2p.Get (1));
  clientApps_p2p.Start (Seconds (2.0));
  clientApps_p2p.Stop (Seconds (10.0));
//client csma/cd
  UdpEchoClientHelper echoClient_csma (interfaces_csma.GetAddress (1), 9);//specification of adresse of client and the port of server
  echoClient_csma.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient_csma.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient_csma.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps_csma = echoClient_csma.Install (nodes_csma.Get (0));
  clientApps_csma.Start (Seconds (5.0));
  clientApps_csma.Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
