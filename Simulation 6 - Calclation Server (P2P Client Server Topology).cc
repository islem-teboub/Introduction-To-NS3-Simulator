
// the topology is composed if 3 nodes : 2 clients et 1 server with socket utilization for communcation
//Scenario
//client 1 send a number x to the server,
//client 2  send a number y to the server, 
//the server calculates s = x+y and send s to the two clients


//        10.15.2.2:12345                      10.15.2.1       10.15.1.2:123                     10.15.1.1:1234
//          Client2--------------------------------------SERVER---------------------------------------client1
//                            P2P link                                        P2P link


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
/////////////////////////// INITIALISATION/////////
void Envoyer (Ptr<Socket> sock, Ipv4Address dstaddr, uint16_t port,Ptr<Packet> p);
void Recevoir_client1 (Ptr<Socket> socket);
void Recevoir_client2 (Ptr<Socket> socket);
void Recevoir_server (Ptr<Socket> socket);
int i =0;
int somme = 0;

////////////////////////////////////////////////////
int main (int argc, char *argv[])
{



 LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
 LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
 LogComponentEnableAll (LOG_PREFIX_TIME);
 LogComponentEnable ("FirstScriptExample", LOG_LEVEL_INFO);

//Nodes creation
  Ptr<Node> client1 = CreateObject<Node> ();
  Ptr<Node> server = CreateObject<Node> ();
  Ptr<Node> client2 = CreateObject<Node> ();
  NodeContainer c1 = NodeContainer (client1, server);
  NodeContainer c2 = NodeContainer (server, client2);


PointToPointHelper p2p;
 p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

NetDeviceContainer devices1;
devices1 = p2p.Install (c1);

NetDeviceContainer devices2;
devices2 = p2p.Install (c2);

//enabling routing
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

InternetStackHelper stack;
stack.Install (c1);
stack.Install (c2.Get(1));

Ipv4AddressHelper address;
address.SetBase ("10.15.1.0","255.255.255.0");
Ipv4InterfaceContainer interfaces1 = address.Assign (devices1);

address.SetBase ("10.15.2.0","255.255.255.0");
Ipv4InterfaceContainer interfaces2 = address.Assign (devices2);


//client 1 socket creation
Ptr<Socket> Socket_client1 = Socket::CreateSocket (client1, TypeId::LookupByName("ns3::UdpSocketFactory"));
 InetSocketAddress sock_client1 = InetSocketAddress ("10.15.1.1", 1234);
 Socket_client1->Bind (sock_client1);
 Socket_client1->SetRecvCallback (MakeCallback (&Recevoir_client1));

//server socket creation
Ptr<Socket> Socket_server = Socket::CreateSocket (server, TypeId::LookupByName("ns3::UdpSocketFactory"));
 InetSocketAddress sock_server = InetSocketAddress ("10.15.1.2", 123);
 Socket_server->Bind (sock_server);
 Socket_server->SetRecvCallback (MakeCallback (&Recevoir_server));

//client 2 socket creation
Ptr<Socket> Socket_client2 = Socket::CreateSocket (client2, TypeId::LookupByName("ns3::UdpSocketFactory"));
 InetSocketAddress sock_client2 = InetSocketAddress ("10.15.2.2", 12345);
 Socket_client2->Bind (sock_client2);
 Socket_client2->SetRecvCallback (MakeCallback (&Recevoir_client2));

char x[20];
//creation of packet that will be sent from client1 to the server
printf("ClIENT 1 > Entrer la valeur de x : ");
scanf("%s",x);
Ptr<Packet> p = Create<Packet> (reinterpret_cast<const uint8_t*> (x),10);
NS_LOG_INFO (" CLIENT 1 > Sending x to SERVER ");
Simulator::Schedule (Seconds (5.0),&Envoyer, Socket_client1, "10.15.1.2",123,p);


char y[20];

//creation of the packet that willbe sent from client2 to the server
printf("ClIENT 2 > Entrer la valeur de y : ");
scanf("%s",y);
Ptr<Packet> q = Create<Packet> (reinterpret_cast<const uint8_t*> (y),10);
NS_LOG_INFO (" CLIENT 2 > Sending y to SERVER ");
Simulator::Schedule (Seconds (10.0),&Envoyer, Socket_client2, "10.15.1.2",123,q);

/*
//convertir la somme (entier) en chaine de caraceter et laffecter dans s
char som[10];
sprintf(som,"%d",somme);

//la creation des packet qui sera envoyé depuis la socket (server) vers la socket client2
Ptr<Packet> qq = Create<Packet> (reinterpret_cast<const uint8_t*> (som),10);
NS_LOG_INFO (" SERVER > Sending somme to client2 ");
Simulator::Schedule (Seconds (10.0),&Envoyer, Socket_server, "10.15.2.2",12345,qq);
*/

//enable routing
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

Simulator::Run();
Simulator::Destroy ();
return 0;
}


//la fonction envoyer (function of sending)
void Envoyer (Ptr<Socket> sock, Ipv4Address dstaddr, uint16_t port,Ptr<Packet> p )
{
 p->AddPaddingAtEnd (100);
 sock->SendTo (p, 0, InetSocketAddress (dstaddr,port));
 }



//recevoir_client1 function

void Recevoir_client1 (Ptr<Socket> socket)
{
 Address from;
 Ptr<Packet> packet = socket->RecvFrom (from);
  // packet->RemoveAllPacketTags (); Remove all packet tags. 
  // packet->RemoveAllByteTags (); // Remove all the tags stored in this packet. 

 uint8_t buf[packet->GetSize()];
 packet->CopyData(buf,packet->GetSize());
 NS_LOG_INFO ("CLIENT 1 > Client Received s = " << buf <<"  Size " <<packet->GetSize () << " bytes from " << InetSocketAddress::ConvertFrom (from).GetIpv4 ());
}

//recevoir_client2 function
void Recevoir_client2 (Ptr<Socket> socket)
{
 Address from;
 Ptr<Packet> packet = socket->RecvFrom (from);
 uint8_t buf[packet->GetSize()];
 packet->CopyData(buf,packet->GetSize());
 NS_LOG_INFO ("CLIENT 2 > client Received s = " << buf <<"  Size "<< packet->GetSize () << " bytes from " << InetSocketAddress::ConvertFrom (from).GetIpv4 ());

}

//recevoir_server function

void Recevoir_server (Ptr<Socket> socket)
{

//routing enabling
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
i++;

Address from;
Ptr<Packet> packet = socket->RecvFrom (from);
uint8_t buf[packet->GetSize()];
packet->CopyData(buf,packet->GetSize());


if( InetSocketAddress::ConvertFrom (from).GetIpv4 () == "10.15.1.1"){

//recover buffer content, and convert it to carachters, then convert it to number bebore affecting it to x
int x=std::atoi((char*)buf);
NS_LOG_INFO ("SERVER > Server Received x = " << buf << " "<<packet->GetSize () << " bytes frrom " << InetSocketAddress::ConvertFrom (from).GetIpv4 ());

somme = somme + x;
}
else {
//recover buffer content, and convert it to carachters, then convert it to number bebore affecting it to y
int y=std::atoi((char*)buf);
NS_LOG_INFO ("SERVER > Server Received y = " << buf << " "<<packet->GetSize () << " bytes frrom " << InetSocketAddress::ConvertFrom (from).GetIpv4 ());

somme = somme + y;
}

if(i==2){

//converting the addition to caractere and affecting it to s
char s[10];
sprintf(s,"%d",somme);

Ptr<Packet> p = Create<Packet> (reinterpret_cast<const uint8_t*> (s),10);
p->AddPaddingAtEnd (100);
//broadcast message after the calculation of the somme
NS_LOG_INFO (" SERVER > Sending s to CLIENT 1 ");
socket->SendTo (p,0,InetSocketAddress ("10.15.1.1",1234));
NS_LOG_INFO (" SERVER > Sending s to CLIENT 2 ");

socket->SendTo (p,0,InetSocketAddress ("10.15.2.2",12345));
//enabling routing
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

}


}






















