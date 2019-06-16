
///                   client---------------------------------------------server
///                 10.15.1.1:1234                                     10.15.1.2:12345

//the client send a paquet named paquet1 to server, the server respend to the client by sending a packet named paquet2




#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"



using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

void Envoyer (Ptr<Socket> sock, Ipv4Address dstaddr, uint16_t port);
void Recevoir_source (Ptr<Socket> socket);
void Recevoir_destination (Ptr<Socket> socket);

int
main (int argc, char *argv[])
{

 LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
 LogComponentEnableAll (LOG_PREFIX_TIME);
 LogComponentEnable ("FirstScriptExample", LOG_LEVEL_INFO);

//Nodes creation
  Ptr<Node> nSrc = CreateObject<Node> ();
  Ptr<Node> nDst = CreateObject<Node> ();
  NodeContainer c = NodeContainer (nSrc, nDst);

CsmaHelper csma;
NetDeviceContainer devices;
devices = csma.Install (c);
InternetStackHelper stack;
stack.Install (c);
Ipv4AddressHelper address;
address.SetBase ("10.15.1.0","255.255.255.0");
Ipv4InterfaceContainer interfaces = address.Assign (devices);

//Server SOCKET creation

Ptr<Socket> srcSocket = Socket::CreateSocket (nSrc, TypeId::LookupByName("ns3::UdpSocketFactory"));
uint16_t srcport = 1234;
Ipv4Address srcaddr ("10.15.1.1");
 InetSocketAddress src = InetSocketAddress (srcaddr, srcport);
 srcSocket->Bind (src);
 srcSocket->SetRecvCallback (MakeCallback (&Recevoir_source));

//Client SOCKET creation

 Ptr<Socket> dstSocket = Socket::CreateSocket (nDst, TypeId::LookupByName ("ns3::UdpSocketFactory"));
 uint16_t dstport = 12345;
 Ipv4Address dstaddr ("10.15.1.2");
 InetSocketAddress dst = InetSocketAddress (dstaddr, dstport);
 dstSocket->Bind (dst);
 dstSocket->SetRecvCallback (MakeCallback (&Recevoir_destination));

LogComponentEnableAll (LOG_PREFIX_TIME);
 LogComponentEnable ("FirstScriptExample", LOG_LEVEL_INFO);

//REQUEST packet creation, this packet will be sent by the client to the server socket
Ptr<Packet> packet1 = Create<Packet> (reinterpret_cast<const uint8_t*> ("REQUEST"),10);
packet1->AddPaddingAtEnd(100);
srcSocket->SendTo (packet1,0,InetSocketAddress (dstaddr,dstport));
NS_LOG_INFO ("CLIENT > Source Send a Packet To Destination ");



Simulator::Run();
Simulator::Destroy ();
return 0;
}


//the function that able client to receive
void Recevoir_source (Ptr<Socket> socket)
{
 Address from;
 Ptr<Packet> packet = socket->RecvFrom (from);
  // packet->RemoveAllPacketTags (); Remove all packet tags.
  // packet->RemoveAllByteTags (); // Remove all the tags stored in this packet.

 uint8_t buf[packet->GetSize()];
 packet->CopyData(buf,packet->GetSize());
 NS_LOG_INFO ("CLEINT > Source Received " << buf <<"  Size " <<packet->GetSize () << " bytes from " << InetSocketAddress::ConvertFrom (from).GetIpv4 ());
}


//the function that able server to receive and resent to the client

void Recevoir_destination (Ptr<Socket> socket)
{

 Address from;
 Ptr<Packet> packet = socket->RecvFrom (from);
  // packet->RemoveAllPacketTags (); Remove all packet tags.
  // packet->RemoveAllByteTags (); // Remove all the tags stored in this packet.

 uint8_t buf[packet->GetSize()];
 packet->CopyData(buf,packet->GetSize());
 NS_LOG_INFO ("SERVER > Server Received " << buf <<"  Size " <<packet->GetSize () << " bytes from " << InetSocketAddress::ConvertFrom (from).GetIpv4 ());


//REPLY packet creation, this packet will be sent from server socket to the client socket
NS_LOG_INFO ("SERVER >  Packet back to source node's ");
Ptr<Packet> packet2 = Create<Packet> (reinterpret_cast<const uint8_t*> ("REPLY"),10);
packet2->AddPaddingAtEnd (100);
socket->SendTo (packet2,0,InetSocketAddress ("10.15.1.1",1234));

}






















