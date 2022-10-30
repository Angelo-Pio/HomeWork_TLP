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

  printf("Configuration is: %d", configuration);


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

  // NS_LOG_INFO ("Install internet stack on all nodes.");
  // InternetStackHelper internet;
  // star.InstallStack (internet);

  // NS_LOG_INFO ("Assign IP Addresses.");
  // star.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"));

  // NS_LOG_INFO ("Create applications.");
  // //
  // // Create a packet sink on the star "hub" to receive packets.
  // // 
  // uint16_t port = 50000;
  // Address hubLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  // PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", hubLocalAddress);
  // ApplicationContainer hubApp = packetSinkHelper.Install (star.GetHub ());
  // hubApp.Start (Seconds (1.0));
  // hubApp.Stop (Seconds (10.0));

  // //
  // // Create OnOff applications to send TCP to the hub, one on each spoke node.
  // //
  // OnOffHelper onOffHelper ("ns3::TcpSocketFactory", Address ());
  // onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  // onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  // ApplicationContainer spokeApps;

  // for (uint32_t i = 0; i < star.SpokeCount (); ++i)
  //   {
  //     AddressValue remoteAddress (InetSocketAddress (star.GetHubIpv4Address (i), port));
  //     onOffHelper.SetAttribute ("Remote", remoteAddress);
  //     spokeApps.Add (onOffHelper.Install (star.GetSpokeNode (i)));
  //   }
  // spokeApps.Start (Seconds (1.0));
  // spokeApps.Stop (Seconds (10.0));

// ! #######################  ENDING ##################################

  NS_LOG_INFO ("Enable static global routing.");
  //
  // Turn on global static routing so we can actually be routed across the star.
  //
  // Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

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
