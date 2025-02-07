//communication entre 1 server et 3 client (pas encore faite)

//[client1]-------communication p2p--------- [server] _______communication csma/cd_______ [client2]-------communication p2p-------- [client3]

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
/////////////////////////////////////////////////////////////
  NodeContainer nodes_p2pG;
  nodes_p2pG.Create (2);

  NodeContainer nodes_csma;
  nodes_csma.Add(nodes_p2pG.Get (1));
  nodes_csma.Create (1);
  

  NodeContainer nodes_p2pD;
  nodes_p2pD.Add(nodes_csma.Get (1));
  nodes_p2pD.Create (1);
////////////////////////////////////////////////////////////////
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

 NetDeviceContainer devices_p2pG;
  devices_p2pG = pointToPoint.Install (nodes_p2pG);

  NetDeviceContainer devices_csma;
  devices_csma = csma.Install (nodes_csma);

NetDeviceContainer devices_p2pD;
  devices_p2pD = pointToPoint.Install (nodes_p2pD);
/////////////////////////////////////////////////
  InternetStackHelper stack;
  stack.Install (nodes_p2pG.Get (0));
  stack.Install (nodes_p2pG.Get (1));
  
  stack.Install (nodes_csma.Get (1));

  stack.Install (nodes_p2pD.Get (1));

////////////////////////////////////////////
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces_p2pG = address.Assign (devices_p2pG);

address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces_csma = address.Assign (devices_csma);

address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces_p2pD = address.Assign (devices_p2pD);

//activer le routage
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

////creer un profil server
  UdpEchoServerHelper echoServer (9);//port du serveur est = 9
///creer des instances server
  ApplicationContainer serverApps = echoServer.Install (nodes_csma.Get (0));

  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (50.0));
  

////creer un profil client
  UdpEchoClientHelper echoClient (interfaces_p2pG.GetAddress (1), 9);//specification de ladresse du server et du port du server
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
///creer des instance clients
ApplicationContainer clientApps1 = echoClient.Install (nodes_p2pG.Get (0));
ApplicationContainer clientApps2 = echoClient.Install (nodes_csma.Get (1));
ApplicationContainer clientApps3 = echoClient.Install (nodes_p2pD.Get (1));

  clientApps1.Start (Seconds (5.0));
 clientApps1.Stop (Seconds (9.0));
 clientApps2.Start (Seconds (16.0));
  clientApps2.Stop (Seconds (18.0));
 clientApps3.Start (Seconds (21.0));
  clientApps3.Stop (Seconds (29.0));


  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
