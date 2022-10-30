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
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/netanim-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/csma-helper.h"

// Network topology (default)


// 
//        n1
//        /
//     n2-n0 ------ n4  n5  n6 --- n7  n8  n9
//        \          |  |  |     |   |  | 
//        n3        ========     ========
// 

// Constants

#define n4 3

#define n6 2
#define n5 1

#define n7 0
#define n8 1
#define n9 2

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Task_1_Team_56");

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

  if(configuration != 0 || configuration != 1 || configuration != 2){
    printf("Configuration is: %d\n", configuration);
    printf("Accepted values: 0 1 2");
    exit(1);
  }



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
  csma2.SetChannelAttribute ("DataRate", StringValue ("30Mbps"));
  csma2.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (20)));


  NetDeviceContainer csma2Devices;
  csma2Devices = csma2.Install(csmaNodes2);
  
// * ############################################# * 

  NS_LOG_INFO ("Building PointToPointCsma topology");

  PointToPointHelper p2pCsma;
  p2pCsma.SetDeviceAttribute ("DataRate", StringValue ("80Mbps"));
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
  Address.SetBase("10.1.1.0", "255.255.255.0");
  star.AssignIpv4Addresses (Address);

  std::ostream& os = std::cout;
  // star.GetHubIpv4Address(0).Print(os);
  puts("\n");

  
  // for (uint32_t i = 0; i < star.SpokeCount(); i++)
  // {
  //   printf("Node: %" PRIu32 " ", star.GetSpokeNode(i)->GetId());
  //   star.GetSpokeIpv4Address(i).Print(os);
  //   puts("\n");
  // }

  Address.SetBase("192.118.1.0", "255.255.255.0");
  Ipv4InterfaceContainer c1Addresses;
  c1Addresses = Address.Assign(csma1Devices);

  // for (uint32_t i = 0; i < csmaNodes1.GetN(); i++)
  // {
    
  //   printf("Node: %" PRIu32 " ", csmaNodes1.Get(i)->GetId());
  //   c1Addresses.Get(i).first->GetAddress(1,0).GetLocal().Print(os);    
  //   puts("\n");
  // }

  // Ipv4AddressHelper csma2Address;
  Address.SetBase("192.118.2.0", "255.255.255.0");
  Ipv4InterfaceContainer c2Addresses;
  c2Addresses = Address.Assign(csma2Devices);

  // for (uint32_t i = 0; i < csmaNodes2.GetN(); i++)
  // {
    
  //   printf("Node: %" PRIu32 " ", csmaNodes2.Get(i)->GetId());
  //   c2Addresses.Get(i).first->GetAddress(1,0).GetLocal().Print(os);    
  //   puts("\n");
  // }
  //   printf("Node: %" PRIu32 " ", csmaNodes1.Get(0)->GetId());
  //   c1Addresses.Get(0).first->GetAddress(2,0).GetLocal().Print(os);    
  

  Address.SetBase("10.2.1.0", "255.255.255.252");
  Ipv4InterfaceContainer p2pAddresses;
  p2pAddresses = Address.Assign(csmaP2PDevices);
  
   printf("Node: %" PRIu32 " ", csmaNodes1.Get(n6)->GetId());
  c1Addresses.Get(2).first->GetAddress(2,0).GetLocal().Print(os);
  
   printf("Node: %" PRIu32 " ", csmaNodes2.Get(0)->GetId());
  c2Addresses.Get(0).first->GetAddress(2,0).GetLocal().Print(os);
  
  
// ! ####################### TASKS ################################

  if(configuration == 0){

  }else if (configuration == 1){

  }else{

  }



// ! #######################  ENDING ##################################

  NS_LOG_INFO ("Enable static global routing.");
  //
  // Turn on global static routing so we can actually be routed across the star.
  //
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  NS_LOG_INFO ("Enable pcap tracing.");
  
  char format[255];


  sprintf(format, "task1-%d-%" PRIu32 , configuration, star.GetHub()->GetId());
  pointToPoint.EnablePcap(format,star.GetHub()->GetDevice(0),true,true);

  sprintf(format, "task1-%d-%" PRIu32, configuration, csmaNodes1.Get(n5)->GetId());
  csma1.EnablePcap(format,csma1Devices.Get(n5),true,true);

  sprintf(format, "task1-%d-%" PRIu32, configuration, csmaNodes2.Get(n7)->GetId());
  csma2.EnablePcap(format,csma2Devices.Get(n7),true,true);


  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  return 0;
}
