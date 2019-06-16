
//this CSMA topology is composed of 3 nodes : 2 clients and 1 server with utilization of sockets for communication
//Scenario
//client 1 send message_1 to the server, the server respend by broadcasting REPLY_1 
//client 2  send message_2 to the server, the server respend by  broadcasting REPLY_2


//        10.15.1.3:12345                            10.15.1.2:123                               10.15.1.1:1234
//          Server--------------------------------------client2---------------------------------------client1
//                           CSMA link                                    CSMA link


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
void Recevoir_MIl (Ptr<Socket> socket);
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
  Ptr<Node> nMil = CreateObject<Node> ();
  Ptr<Node> nDst = CreateObject<Node> ();
  NodeContainer c = NodeContainer (nSrc, nMil, nDst);

CsmaHelper csma;
NetDeviceContainer devices;
devices = csma.Install (c);
InternetStackHelper stack;
stack.Install (c);
Ipv4AddressHelper address;
address.SetBase ("10.15.1.0","255.255.255.0");
Ipv4InterfaceContainer interfaces = address.Assign (devices);

//SOURCE SOCKET creation

Ptr<Socket> srcSocket = Socket::CreateSocket (nSrc, TypeId::LookupByName("ns3::UdpSocketFactory"));
uint16_t srcport = 1234;
Ipv4Address srcaddr (interfaces.GetAddress(0));
 InetSocketAddress src = InetSocketAddress (srcaddr, srcport);
 srcSocket->Bind (src);
 srcSocket->SetRecvCallback (MakeCallback (&Recevoir_source));

//MIL SOCKET creation

Ptr<Socket> milSocket = Socket::CreateSocket (nMil, TypeId::LookupByName ("ns3::UdpSocketFactory"));
 uint16_t milport = 123;
 Ipv4Address miladdr ("10.15.1.2");
 InetSocketAddress mil = InetSocketAddress (miladdr, milport);
 milSocket->Bind (mil);
 milSocket->SetRecvCallback (MakeCallback (&Recevoir_MIl));

//DESTINATION SOCKET creation

 Ptr<Socket> dstSocket = Socket::CreateSocket (nDst, TypeId::LookupByName ("ns3::UdpSocketFactory"));
 uint16_t dstport = 12345;
 Ipv4Address dstaddr ("10.15.1.3");
 InetSocketAddress dst = InetSocketAddress (dstaddr, dstport);
 dstSocket->Bind (dst);
 dstSocket->SetRecvCallback (MakeCallback (&Recevoir_destination));

//creation of packet that will be sent from socket source (client1) to socket destination
Ptr<Packet> p = Create<Packet> (reinterpret_cast<const uint8_t*> ("message_1"),10);
p->AddPaddingAtEnd(100);
NS_LOG_INFO (" CLIENT 1 > Sending message_1 to SERVER ");
srcSocket->SendTo (p,0,InetSocketAddress (dstaddr,dstport));

//creation of packet that will be sent from socket milieu (client2) to socket destination
p = Create<Packet> (reinterpret_cast<const uint8_t*> ("message_2"),10);
p->AddPaddingAtEnd(100);
NS_LOG_INFO (" CLIENT 2 > Sending message_2 to SERVER ");
milSocket->SendTo (p, 0, InetSocketAddress (dstaddr,dstport));


Simulator::Run();
Simulator::Destroy ();
return 0;
}


//recevoir_source function

void Recevoir_source (Ptr<Socket> socket)
{

 Address from;
 Ptr<Packet> packet = socket->RecvFrom (from);
  // packet->RemoveAllPacketTags (); Remove all packet tags. 
  // packet->RemoveAllByteTags (); // Remove all the tags stored in this packet. 

 uint8_t buf[packet->GetSize()];
 packet->CopyData(buf,packet->GetSize());
 NS_LOG_INFO ("CLIENT 1 > Client Received " << buf <<"  Size " <<packet->GetSize () << " bytes from " << InetSocketAddress::ConvertFrom (from).GetIpv4 ());
}

//recevoir_Mil function
void Recevoir_MIl (Ptr<Socket> socket)
{

 Address from;
 Ptr<Packet> packet = socket->RecvFrom (from);
 uint8_t buf[packet->GetSize()];
 packet->CopyData(buf,packet->GetSize());
 NS_LOG_INFO ("CLIENT 2 > client Received " << buf <<"  Size "<< packet->GetSize () << " bytes from " << InetSocketAddress::ConvertFrom (from).GetIpv4 ());

}

//recevoir_destination function

void Recevoir_destination (Ptr<Socket> socket)
{

Address from;
Ptr<Packet> packet = socket->RecvFrom (from);
uint8_t buf[packet->GetSize()];
packet->CopyData(buf,packet->GetSize());

NS_LOG_INFO ("SERVER > Server Received " << buf << " "<<packet->GetSize () << " bytes frrom " << InetSocketAddress::ConvertFrom (from).GetIpv4 ());


if(InetSocketAddress::ConvertFrom (from).GetIpv4 ()=="10.15.1.1"){

Ptr<Packet> p = Create<Packet> (reinterpret_cast<const uint8_t*> ("REPLY_1"),10);
p->AddPaddingAtEnd (100);
//broadcast message after receiving message_1
NS_LOG_INFO (" SERVER > Sending REPLY_1 to CLIENT 1 ");
socket->SendTo (p,0,InetSocketAddress ("10.15.1.1",1234));
NS_LOG_INFO (" SERVER > Sending REPLY_1 to CLIENT 2 ");
socket->SendTo (p,0,InetSocketAddress ("10.15.1.2",123));
}
else {

Ptr<Packet> p = Create<Packet> (reinterpret_cast<const uint8_t*> ("REPLY_2"),10);
p->AddPaddingAtEnd (100);
//broadcast message after receiving message_2
NS_LOG_INFO (" SERVER > Sending REPLY_2 to CLIENT 1 ");
socket->SendTo (p,0,InetSocketAddress ("10.15.1.1",1234));
NS_LOG_INFO (" SERVER > Sending REPLY_2 to CLIENT 2 ");
socket->SendTo (p,0,InetSocketAddress ("10.15.1.2",123));
}

}






















