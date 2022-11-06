/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <inttypes.h>
#include "ns3/address.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/netanim-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/csma-helper.h"
#include "ns3/inet-socket-address.h"
// Network topology (default)


// 
//        n1
//        /
//     n2-n0 ------ n4  n5  n6 --- n7  n8  n9
//        \          |  |  |     |   |  | 
//        n3        ========     ========
// 

// Constants

#define n1 0
#define n2 1
#define n3 2
#define n4 3

#define n6 2
#define n5 1

#define n7 0
#define n8 1
#define n9 2

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Task_1_Team_56");

void printNodesIP(PointToPointStarHelper star, Ipv4InterfaceContainer c1Addresses, Ipv4InterfaceContainer c2Addresses, NodeContainer csmaNodes1, NodeContainer csmaNodes2 );

int main (int argc, char *argv[])
{

  //
  // Default number of nodes in the star.  Overridable by command line argument.
  //
  uint32_t nSpokes = 4;
  uint32_t nCsma1 = 2;
  uint32_t nCsma2 = 3;
  int configuration = 0;


  CommandLine cmd (__FILE__);
  cmd.AddValue ("nSpokes", "Number of nodes to place in the star", nSpokes);
  cmd.AddValue ("configuration", "Configuration to apply", configuration);
  cmd.Parse (argc, argv);

  printf("configuration is: %d", configuration);

  // LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  // LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
  // LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
  // LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
// * ############################################# * 
  
  NS_LOG_INFO ("Build star topology.");

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("80Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("10us"));
  PointToPointStarHelper star (nSpokes, pointToPoint);


// * ############################################# * 

  NS_LOG_INFO ("Building Csma topology #1 .");
  
  NodeContainer csmaNodes1;
  csmaNodes1.Add(star.GetSpokeNode(n4));
  csmaNodes1.Create(nCsma1);
  
  CsmaHelper csma1;
  
  csma1.SetChannelAttribute ("DataRate", StringValue ("25Mbps"));
  csma1.SetChannelAttribute ("Delay", TimeValue (MicroSeconds(10)));
  


  NetDeviceContainer csma1Devices;
  csma1Devices = csma1.Install(csmaNodes1);

// * ############################################# * 

  NS_LOG_INFO ("Building Csma topology #2 .");

  NodeContainer csmaNodes2;
  csmaNodes2.Create(nCsma2);

  CsmaHelper csma2;
  csma2.SetChannelAttribute ("DataRate", StringValue ("80Mbps"));
  csma2.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (20)));


  NetDeviceContainer csma2Devices;
  csma2Devices = csma2.Install(csmaNodes2);
  
// * ############################################# * 

  NS_LOG_INFO ("Building PointToPointCsma topology");

  PointToPointHelper p2pCsma;
  p2pCsma.SetDeviceAttribute ("DataRate", StringValue ("30Mbps"));
  p2pCsma.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (10)));


  NetDeviceContainer csmaP2PDevices;
  csmaP2PDevices = p2pCsma.Install(csmaNodes1.Get(n6), csmaNodes2.Get(n7));
  
/*
  !! ## variables definitions ##
  ? netDevices
    
    * star
    * csma1Devices 
    * csma2Devices
    * csmaP2PDevices 
  
  ? Helpers
    * star - PointToPoint
    * csma1 - csma2
    * p2pcsma
*/

  

// ! Internet Stack Here

  NS_LOG_INFO ("Install internet stack on all nodes.");
  InternetStackHelper internet;
  star.InstallStack (internet);
  internet.Install(csmaNodes1.Get(n5));
  internet.Install(csmaNodes1.Get(n6));
  internet.Install(csmaNodes2);


// ! Ip Address Assignment

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper Address;
  Address.SetBase("10.0.1.0", "255.255.255.240");
  star.AssignIpv4Addresses (Address);

  

  Address.SetBase("192.118.1.0", "255.255.255.0");
  Ipv4InterfaceContainer c1Addresses;
  c1Addresses = Address.Assign(csma1Devices);

  

  Address.SetBase("192.118.2.0", "255.255.255.0");
  Ipv4InterfaceContainer c2Addresses;
  c2Addresses = Address.Assign(csma2Devices);

  

  Address.SetBase("10.0.2.0", "255.255.255.252");
  Ipv4InterfaceContainer p2pAddresses;
  p2pAddresses = Address.Assign(csmaP2PDevices);
  
  // printNodesIP( star,  c1Addresses,  c2Addresses, csmaNodes1, csmaNodes2 );
  
// ! ####################### TASKS ################################

  if(configuration == 0){

    // * TCP sink
    
    uint16_t port = 2600;

    PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sink.Install(star.GetSpokeNode(n1));
    sinkApp.Start (Seconds (0));
    sinkApp.Stop (Seconds (20.0));

    // * TCP OnOff Client

    OnOffHelper onOffHelper ("ns3::TcpSocketFactory", InetSocketAddress(star.GetSpokeIpv4Address(n1), port));
    onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onOffHelper.SetAttribute ("PacketSize", UintegerValue(1500));

    

    ApplicationContainer client;
    
    AddressValue remoteAddress (InetSocketAddress (star.GetSpokeIpv4Address(n1), port));
    onOffHelper.SetAttribute ("Remote", remoteAddress);

    client = onOffHelper.Install(csmaNodes2.Get(n9));

    client.Start (Seconds (3.0));
    client.Stop (Seconds (15.0));

    char f[255];

    sprintf(f, "task1-%d-n%" PRIu32 ".tr", configuration, star.GetSpokeNode(n1)->GetId());
    pointToPoint.EnableAscii(f,star.GetSpokeNode(n1)->GetDevice(0),true);

    sprintf(f, "task1-%d-n%" PRIu32 ".tr", configuration, csmaNodes2.Get(n9)->GetId());
    csma2.EnableAscii(f,csma2Devices.Get(n9),true);


  }else if (configuration == 1){
    // * TCP SINK n1

    uint16_t port1 = 2600;
    PacketSinkHelper sink1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port1));
    ApplicationContainer sinkApp1= sink1.Install(star.GetSpokeNode(n1));
    sinkApp1.Start (Seconds (0));
    sinkApp1.Stop (Seconds (20.0));

    //* TCP SINK n2

    uint16_t port2 = 7777;
    PacketSinkHelper sink2 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port2));
    ApplicationContainer sinkApp2 = sink2.Install(star.GetSpokeNode(n2));
    sinkApp2.Start (Seconds (0));
    sinkApp2.Stop (Seconds (20.0));

    //* TCP onOff client n9 to n1

    OnOffHelper onOffHelper1 ("ns3::TcpSocketFactory", InetSocketAddress(star.GetSpokeIpv4Address(n1), port1));
    onOffHelper1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onOffHelper1.SetAttribute ("PacketSize", UintegerValue(2500));

    ApplicationContainer client1;
    
    AddressValue remoteAddress1 (InetSocketAddress (star.GetSpokeIpv4Address(n1), port1));
    onOffHelper1.SetAttribute ("Remote", remoteAddress1);

    client1 = onOffHelper1.Install(csmaNodes2.Get(n9));

    client1.Start (Seconds (5.0));
    client1.Stop (Seconds (15.0));

    //* TCP OnOff client n8 to n2


    OnOffHelper onOffHelper2 ("ns3::TcpSocketFactory", InetSocketAddress(star.GetSpokeIpv4Address(n2), port2));
    onOffHelper2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onOffHelper2.SetAttribute ("PacketSize", UintegerValue(5000));

    ApplicationContainer client2;
    
    AddressValue remoteAddress2 (InetSocketAddress (star.GetSpokeIpv4Address(n2), port2));
    onOffHelper2.SetAttribute ("Remote", remoteAddress2);

    client2 = onOffHelper2.Install(csmaNodes2.Get(n8));

    client2.Start (Seconds (2.0));
    client2.Stop (Seconds (9.0));

    char f[255];

    sprintf(f, "task1-%d-n%" PRIu32 ".tr", configuration, star.GetSpokeNode(n1)->GetId());
    pointToPoint.EnableAscii(f,star.GetSpokeNode(n1)->GetDevice(0),true);

    sprintf(f, "task1-%d-n%" PRIu32 ".tr", configuration, star.GetSpokeNode(n2)->GetId());
    pointToPoint.EnableAscii(f,star.GetSpokeNode(n2)->GetDevice(0),true);

    sprintf(f, "task1-%d-n%" PRIu32 ".tr", configuration, csmaNodes2.Get(n9)->GetId());
    csma2.EnableAscii(f,csma2Devices.Get(n9),true);

    sprintf(f, "task1-%d-n%" PRIu32 ".tr", configuration, csmaNodes2.Get(n8)->GetId());
    csma2.EnableAscii(f,csma2Devices.Get(n8),true);
    
  }else{
//* Create a UdpEchoServer application on nn2.
    
    uint16_t port2 = 63; 
    UdpEchoServerHelper server2(port2);
    ApplicationContainer apps2 = server2.Install(star.GetSpokeNode(1));
    apps2.Start(Seconds(1.0));
    apps2.Stop(Seconds(20.0));

    //
    //* Create a UdpEchoClient application to send UDP datagrams from n8 to n2
    
    //
    uint32_t packetSize = 2560;
    uint32_t maxPacketCount = 5;
    Time interPacketInterval = Seconds(2.0);
    UdpEchoClientHelper client2(star.GetSpokeIpv4Address(1), port2);
    client2.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
    client2.SetAttribute("Interval", TimeValue(interPacketInterval));
    client2.SetAttribute("PacketSize", UintegerValue(packetSize));
    apps2 = client2.Install(csmaNodes2.Get(n8));
    apps2.Start(Seconds(3.0));
    apps2.Stop(Seconds(10.0));  

    client2.SetFill (apps2.Get (0), "7856807");

    client2.SetFill (apps2.Get (0), 0xa5, packetSize);

    uint8_t fill[] = { 0, 1, 2, 3, 4, 5, 6};
    client2.SetFill (apps2.Get (0), fill, sizeof(fill), packetSize);


    // * TCP SINK n1

    uint16_t port1 = 2600;
    PacketSinkHelper sink1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port1));
    ApplicationContainer sinkApp1= sink1.Install(star.GetSpokeNode(n1));
    sinkApp1.Start (Seconds (0));
    sinkApp1.Stop (Seconds (20.0));

    // * UDP sink n3

    uint16_t port3 = 2500;
    PacketSinkHelper sink3("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port3));
    ApplicationContainer sinkApp3= sink3.Install(star.GetSpokeNode(n3));
    sinkApp1.Start (Seconds (0));
    sinkApp1.Stop (Seconds (20.0));

    // * TCP OnOff Client n9

    OnOffHelper onOffHelper9 ("ns3::TcpSocketFactory", InetSocketAddress(star.GetSpokeIpv4Address(n1), port1));
    onOffHelper9.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper9.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onOffHelper9.SetAttribute ("PacketSize", UintegerValue(3000));

    ApplicationContainer client9;
    
    AddressValue remoteAddress9 (InetSocketAddress (star.GetSpokeIpv4Address(n1), port1));
    onOffHelper9.SetAttribute ("Remote", remoteAddress9);

    client9 = onOffHelper9.Install(csmaNodes2.Get(n9));

    client9.Start (Seconds (3.0));
    client9.Stop (Seconds (9.0));


    // *  Udp OnOff Client n8

    OnOffHelper onOffHelper8 ("ns3::UdpSocketFactory", InetSocketAddress(star.GetSpokeIpv4Address(n3), port3));
    onOffHelper8.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper8.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onOffHelper8.SetAttribute ("PacketSize", UintegerValue(3000));

    

    ApplicationContainer client8;
    
    AddressValue remoteAddress8 (InetSocketAddress (star.GetSpokeIpv4Address(n3), port3));
    onOffHelper8.SetAttribute ("Remote", remoteAddress8);

    client8 = onOffHelper8.Install(csmaNodes2.Get(n8));

    client8.Start (Seconds (5.0));
    client8.Stop (Seconds (15.0));

    char f[255] ;
    sprintf(f, "task1-%d-n%" PRIu32 ".tr", configuration, star.GetSpokeNode(n1)->GetId());
    pointToPoint.EnableAscii(f,star.GetSpokeNode(n1)->GetDevice(0),true);

    sprintf(f, "task1-%d-n%" PRIu32 ".tr", configuration, star.GetSpokeNode(n2)->GetId());
    pointToPoint.EnableAscii(f,star.GetSpokeNode(n2)->GetDevice(0),true);

    sprintf(f, "task1-%d-n%" PRIu32 ".tr", configuration, star.GetSpokeNode(n3)->GetId());
    pointToPoint.EnableAscii(f,star.GetSpokeNode(n3)->GetDevice(0),true);

    sprintf(f, "task1-%d-n%" PRIu32 ".tr", configuration, csmaNodes2.Get(n9)->GetId());
    csma2.EnableAscii(f,csma2Devices.Get(n9),true);

    sprintf(f, "task1-%d-n%" PRIu32 ".tr", configuration, csmaNodes2.Get(n8)->GetId());
    csma2.EnableAscii(f,csma2Devices.Get(n8),true);

  }



// ! #######################  ENDING ##################################

  NS_LOG_INFO ("Enable static global routing.");
  //
  // Turn on global static routing so we can actually be routed across the star.
  //
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  NS_LOG_INFO ("Enable pcap tracing.");
  
  char format[255];


  sprintf(format, "task1-%d-n%" PRIu32  ".pcap", configuration, star.GetHub()->GetId());
  pointToPoint.EnablePcap(format,star.GetHub()->GetDevice(0),true,true);

  sprintf(format, "task1-%d-n%" PRIu32 ".pcap", configuration, csmaNodes1.Get(n5)->GetId());
  csma1.EnablePcap(format,csma1Devices.Get(n5),true,true);

  sprintf(format, "task1-%d-n%" PRIu32 ".pcap", configuration, csmaNodes2.Get(n7)->GetId());
  csma2.EnablePcap(format,csma2Devices.Get(n7),true,true);

  // ? get n1 and n9 pcap files

  // sprintf(format, "task1-%d-n%" PRIu32 ".pcap", configuration, csmaNodes2.Get(n9)->GetId());
  // csma2.EnablePcap(format,csma2Devices.Get(n9),true,true);

  // sprintf(format, "task1-%d-n%" PRIu32 ".pcap", configuration, star.GetSpokeNode(n1)->GetId());
  // pointToPoint.EnablePcap(format,star.GetSpokeNode(n1)->GetDevice(0),true,true);

  NS_LOG_INFO ("Run Simulation.");

  Simulator::Run ();
  Simulator::Stop (Seconds(20.0));
  Simulator::Destroy();

  
  NS_LOG_INFO ("Done.");

  return 0;
}

void printNodesIP(PointToPointStarHelper star, Ipv4InterfaceContainer c1Addresses, Ipv4InterfaceContainer c2Addresses, NodeContainer csmaNodes1, NodeContainer csmaNodes2 ){
  std::ostream& os = std::cout;
  star.GetHubIpv4Address(0).Print(os);
  puts("\n");

  
  for (uint32_t i = 0; i < star.SpokeCount(); i++)
  {
    printf("Node: %" PRIu32 " ", star.GetSpokeNode(i)->GetId());
    star.GetSpokeIpv4Address(i).Print(os);
    puts("\n");
  }
for (uint32_t i = 0; i < csmaNodes1.GetN(); i++)
  {
    
    printf("Node: %" PRIu32 " ", csmaNodes1.Get(i)->GetId());
    c1Addresses.Get(i).first->GetAddress(1,0).GetLocal().Print(os);    
    puts("\n");
  }
for (uint32_t i = 0; i < csmaNodes2.GetN(); i++)
  {
    
    printf("Node: %" PRIu32 " ", csmaNodes2.Get(i)->GetId());
    c2Addresses.Get(i).first->GetAddress(1,0).GetLocal().Print(os);    
    puts("\n");
  }
    printf("Node: %" PRIu32 " ", csmaNodes1.Get(0)->GetId());
    c1Addresses.Get(0).first->GetAddress(2,0).GetLocal().Print(os);    
    puts("\n");
  
 printf("Node: %" PRIu32 " ", csmaNodes1.Get(n6)->GetId());
  c1Addresses.Get(2).first->GetAddress(2,0).GetLocal().Print(os);
    puts("\n");
  
   printf("Node: %" PRIu32 " ", csmaNodes2.Get(0)->GetId());
  c2Addresses.Get(0).first->GetAddress(2,0).GetLocal().Print(os);
    puts("\n");
  
}