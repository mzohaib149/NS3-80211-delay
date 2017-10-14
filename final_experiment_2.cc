/*
 *ECE 6110 - Lab 3
 * Justin Eng
 * Muhammad Zohaib 
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <time.h>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-sink.h"
#include "ns3/log.h"

//Wireless
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("Final_Lab");

unsigned int roundNumber=0;
Ptr<Socket> nodeSocket[100];
uint16_t nodeport[100];
Ptr<Socket> nodeSocket1[100];
uint16_t nodeport1[100];
int burstSent = 0;
int untrackedRelays = 0;

//Buffer for tracking packets

  void SendStuff (Ptr<Socket> sock, Ipv4Address dstaddr, uint16_t port, unsigned int track)
  {
     Ptr<Packet> p;
     if (track) {
       //cout << "create tracked packet" << endl;
       p = Create<Packet> (reinterpret_cast<const uint8_t*>("sourcepacket"), 546);
     } else {
       p = Create<Packet> (546); //512 payload + 34 header bytes
     }
     sock->SendTo (p, 0, InetSocketAddress (dstaddr,port));
     return;
  }


void anySocketRecv(Ptr<Socket> socket)
{
	//cout<<"Inside any socket receive"<<endl;
	 Address from;
    	 Ptr<Packet> packet = socket->RecvFrom (from);
   	
	InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
   	
	int count = 0;
	for (unsigned int i=0; i<100; i++)
	{
		if (nodeSocket[i]==socket)
		{
			count = i;
			break;
		}
	}

	stringstream ss;
	ss << (count+2);
	string str = ss.str();
	
	string ipaddress = "10.0.0."+str;
	const char* ipchar = ipaddress.c_str();

    	//cout<<"Node Received " << packet->GetSize () << " bytes from " << address.GetIpv4 () <<"at time "<<Simulator::Now().GetSeconds()<<endl;
        uint8_t *buf = new uint8_t[packet->GetSize()];
        packet->CopyData(buf, packet->GetSize());
        std::string s = std::string((char*)buf);
        unsigned int send_tracked_pkt = (strlen(s.c_str()) > 0) ? 1 : 0;
        //cout << "socket received: " << strlen(s.c_str()) << endl;
        Simulator::Schedule (MicroSeconds(500),&SendStuff, socket, Ipv4Address (ipchar) , nodeport[count+1], send_tracked_pkt);
        //Simulator::Schedule (MicroSeconds(0),&SendStuff, socket, Ipv4Address (ipchar) , nodeport[count+1], send_tracked_pkt);

}

   void srcSocketRecv (Ptr<Socket> socket)
   {
     //cout<<"Inside src receive"<<endl;
     Address from;
     Ptr<Packet> packet = socket->RecvFrom (from);
   	
	InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
   
 
    if (socket->GetBoundNetDevice ())
       {
         NS_LOG_INFO ("Socket was bound");
       } 
     else
       {
         NS_LOG_INFO ("Socket was not bound");
       }

      //Determine if packet is from source or not 
      uint8_t *buf = new uint8_t[packet->GetSize()];
      packet->CopyData(buf, packet->GetSize());
      std::string s = std::string((char*)buf);
      int is_tracked = (strlen(s.c_str()) > 0) ? 1 : 0;
      if (is_tracked){
        cout << roundNumber << "," << Simulator::Now().GetSeconds()<<endl;
	untrackedRelays = 0;
        roundNumber++;
	if (roundNumber<100 )
	{
                //Create another tracked packed and forward to neighbor
		Simulator::Schedule (MicroSeconds(500),&SendStuff, socket, Ipv4Address ("10.0.0.2") , nodeport[1], 1);
		//Simulator::Schedule (MicroSeconds(0),&SendStuff, socket, Ipv4Address ("10.0.0.2") , nodeport[1], 1);
		unsigned int timetosend;

                if (!burstSent) {
                  //Only trigger the random packets to send ONCE
                  burstSent = 1;

                  //Queue up random noise packets
 		  Ptr<UniformRandomVariable> rnd = CreateObject<UniformRandomVariable>();
      		  rnd->SetAttribute("Min", DoubleValue(0));
    		  rnd->SetAttribute("Max", DoubleValue(10000));
    		  timetosend = rnd->GetValue();
		  Simulator::Schedule (MicroSeconds (timetosend),&SendStuff, nodeSocket[9], Ipv4Address ("10.0.0.11") , nodeport[10], 0);
		  Simulator::Schedule (MicroSeconds (timetosend),&SendStuff, nodeSocket[19], Ipv4Address ("10.0.0.21") , nodeport[20], 0);
		  Simulator::Schedule (MicroSeconds (timetosend),&SendStuff, nodeSocket[29], Ipv4Address ("10.0.0.31") , nodeport[30], 0);
		  Simulator::Schedule (MicroSeconds (timetosend),&SendStuff, nodeSocket[39], Ipv4Address ("10.0.0.41") , nodeport[40], 0);
		  Simulator::Schedule (MicroSeconds (timetosend),&SendStuff, nodeSocket[49], Ipv4Address ("10.0.0.51") , nodeport[50], 0);
		  Simulator::Schedule (MicroSeconds (timetosend),&SendStuff, nodeSocket[59], Ipv4Address ("10.0.0.61") , nodeport[60], 0);
		  Simulator::Schedule (MicroSeconds (timetosend),&SendStuff, nodeSocket[69], Ipv4Address ("10.0.0.71") , nodeport[70], 0);
		  Simulator::Schedule (MicroSeconds (timetosend),&SendStuff, nodeSocket[79], Ipv4Address ("10.0.0.81") , nodeport[80], 0);
		  Simulator::Schedule (MicroSeconds (timetosend),&SendStuff, nodeSocket[89], Ipv4Address ("10.0.0.91") , nodeport[90], 0);
                }
	}
    } else {
      //cout << "forwarding untracked packet at time: " << Simulator::Now().GetSeconds() << endl;
      untrackedRelays++;
      if (untrackedRelays > 30) { 
        cout << "Very likely source packet was dropped, stopping simulation at " << Simulator::Now().GetSeconds() << endl;
        Simulator::Stop();
        Simulator::Destroy();
      }
      Simulator::Schedule (MicroSeconds(500), &SendStuff, nodeSocket[0], Ipv4Address("10.0.0.2"), nodeport[1], 0);
      //Simulator::Schedule (MicroSeconds(0), &SendStuff, nodeSocket[0], Ipv4Address("10.0.0.2"), nodeport[1], 0);
    }  
   }

void dstSocketRecv (Ptr<Socket> socket)
  {
     //cout<<"Inside dst receive"<<endl;
     Address from;
     Ptr<Packet> packet = socket->RecvFrom (from);
     InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
     //cout<<"Destination Received " << packet->GetSize () << " bytes from " << address.GetIpv4 () <<"at time "<<Simulator::Now().GetSeconds()<<endl;
     //NS_LOG_INFO ("Destination Received " << packet->GetSize () << " bytes from " << address.GetIpv4 ());
     NS_LOG_INFO ("Triggering packet back to source node");
     
     uint8_t *buf = new uint8_t[packet->GetSize()];
     packet->CopyData(buf, packet->GetSize());
     std::string s = std::string((char*)buf);
     int is_tracked = (strlen(s.c_str()) > 0) ? 1 : 0;
     //cout << "dst received: " << s.c_str() << endl; 
     Simulator::Schedule (MicroSeconds(500),&SendStuff, socket, Ipv4Address ("10.0.0.1") , nodeport[0], is_tracked);
     //Simulator::Schedule (MicroSeconds(0),&SendStuff, socket, Ipv4Address ("10.0.0.1") , nodeport[0], is_tracked);
   }



int main(int argc, char *argv[]) {
  //Define parameters
  unsigned int nodeCount = 100;
  unsigned int lanArea = 1000;  
 
  //Set config options (frag, phymode, onoff packetsize)
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", 
                      StringValue("2200"));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", 
                      StringValue("0"));

  Config::SetDefault ("ns3::Ipv4L3Protocol::DefaultTtl", 
                      ns3::UintegerValue(128));
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue("DsssRate11Mbps"));
  
  //Parse cmd line
  CommandLine cmd;
  cmd.AddValue("nodeCount", "total # of nodes", nodeCount);
  cmd.AddValue("area", "size of area [in m^2]", lanArea);
  cmd.Parse(argc, argv);

  //*************************************
  //*  TOPOLOGY SETUP
  //************************************* 
  //Seed PRNG
  RngSeedManager::SetSeed(time(NULL));

  //Stringify area size
  string str_area = static_cast<ostringstream*>(&(ostringstream()<<lanArea))->str();

  //Randomly assign node positions using mobility
  NodeContainer nodes;
  nodes.Create(nodeCount);
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator>();
  
  
int v1[10] = {0,200,400,600,800,900,700,500,300,100};
int v2[10] = {0,100,200,300,400,500,600,700,800,900};
int v3[10] = {900,800,700,600,500,400,300,200,100,0};
//int nodeid=0;
for (int i=0; i<10; i++)
{
	for (int j=0;j<10;j++)
	{
		
		if (!(i%2))
		{
			positionAlloc ->Add(Vector(v2[j], v1[i], 0));
			//cout<<v2[j]<<" "<<v1[i]<<endl;
		}
		else
		{
			positionAlloc ->Add(Vector(v3[j], v1[i], 0));
			//cout<<v3[j]<<" "<<v1[i]<<endl;

		}
	}
}


  
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel"); //Not moving
 
  mobility.Install(nodes);

  //Configure Wireless PHY
  //double decibels = 10 * log10(txPower); //phy helper expects db not mW
  WifiHelper wifi;
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();   

  //Configure channel
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel",
                                  "MaxRange", DoubleValue (250.0));

  wifiPhy.SetChannel(wifiChannel.Create());

  //Configure MAC
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default();
  wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                               "DataMode", StringValue("DsssRate11Mbps"),
                               "ControlMode", StringValue("DsssRate11Mbps"));
  wifiMac.SetType("ns3::AdhocWifiMac"); 


  //Install all and create adhoc wireless network
  NetDeviceContainer wifiDevices = wifi.Install (wifiPhy, wifiMac, nodes);
Ipv4StaticRoutingHelper ipv4RoutingHelper;
Ipv4ListRoutingHelper list;

list.Add (ipv4RoutingHelper, 10);

InternetStackHelper stack;
stack.SetRoutingHelper(list);

stack.Install(nodes);
Ipv4AddressHelper address;
  address.SetBase("10.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer interfaces;
  interfaces = address.Assign(wifiDevices); 
 

  
 Ptr<Ipv4> ipv4Node[nodeCount];
 
 for (unsigned int i=0; i<nodeCount; i++)
  {
	Ptr<Node> nodeptr = nodes.Get(i);	
	ipv4Node[i] = nodeptr->GetObject<Ipv4> ();
	//Ipv4Address addri = ipv4Node[i]->GetAddress (1,0).GetLocal (); 
	//cout<<addri<<",";
  }	
  Ptr<Ipv4StaticRouting> staticRoutingNode[nodeCount];
 for (unsigned int i=0; i<nodeCount; i++)
  {
	staticRoutingNode[i] = ipv4RoutingHelper.GetStaticRouting (ipv4Node[i]);
  }	
  
  for (unsigned int i=0; i<nodeCount-1; i++)
  {
	unsigned int count = i+2;
	stringstream ss;
	ss << count;
	string countstr = ss.str();
	string temp = "10.0.0."+countstr;
	const char* nextHop = temp.c_str();
	//cout<<nextHop<<endl;
	staticRoutingNode[i]->AddHostRouteTo (Ipv4Address ("10.0.0.100"), Ipv4Address (nextHop), 1, 1 );
  }
 staticRoutingNode[99]->AddHostRouteTo (Ipv4Address ("10.0.0.1"), Ipv4Address ("10.0.0.1"), 1, 1);

  //Assign IPs to all nodes
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  //*****************************************
  //*  SIMULATION SETUP
  //*****************************************

for (unsigned int i=0; i<nodeCount; i++)
{
	Ptr<Node> Node = nodes.Get(i);
	nodeSocket[i] = Socket::CreateSocket (Node, TypeId::LookupByName ("ns3::UdpSocketFactory"));

	nodeport[i] = 11111+i;
        InetSocketAddress nodeaddr = InetSocketAddress (Ipv4Address::GetAny (), nodeport[i]);
	nodeSocket[i]->Bind (nodeaddr);

	if (i==0)
		nodeSocket[i]->SetRecvCallback (MakeCallback (&srcSocketRecv));
	else if (i==99)
		nodeSocket[i]->SetRecvCallback (MakeCallback (&dstSocketRecv));
	else 
		nodeSocket[i]->SetRecvCallback (MakeCallback (&anySocketRecv));



}

for (unsigned int i=0; i<nodeCount; i++)
{
	Ptr<Node> Node = nodes.Get(i);
	nodeSocket1[i] = Socket::CreateSocket (Node, TypeId::LookupByName ("ns3::UdpSocketFactory"));

	nodeport1[i] = 22222+i;
        InetSocketAddress nodeaddr1 = InetSocketAddress (Ipv4Address::GetAny (), nodeport1[i]);
	nodeSocket1[i]->Bind (nodeaddr1);

}


 Ipv4GlobalRoutingHelper g;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("final_project.routes", std::ios::out);
  g.PrintRoutingTableAllAt (Seconds (0), routingStream);

Simulator::Schedule (Seconds (0),&SendStuff, nodeSocket[0], Ipv4Address ("10.0.0.2") , nodeport[1], 1);

 //Run simulation
  Simulator::Run();
  Simulator::Destroy();

cout<<roundNumber<<endl;
 return 0;
}




