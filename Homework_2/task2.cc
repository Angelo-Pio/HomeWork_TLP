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
 */

#include <stdio.h>
#include <inttypes.h>
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/animation-interface.h"

#define n0 0
#define n3 2
#define n4 3
#define packetSize 512 //bytes
#define npackets 2 //bytes

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HW2_Task2_Team_56");

int main(int argc, char* argv[])
{
// ! COSTANTS

    uint32_t nStations = 5;
    uint32_t nAPnodes = 1;
    uint16_t UDP_PORT = 21;
    bool useRtsCts = false;
    bool useNetAnim = false;
    bool verbose = false;
    // For simulation purposes use ssid = 7856807
    std::string ssid ="TLC2022";
    

// ! Command line arguments

    CommandLine cmd(__FILE__);
    cmd.AddValue("ssid", "Insert LAN SSID", ssid);
    cmd.AddValue("verbose", "Generate netanim files related ", verbose );
    cmd.AddValue("useRtsCts", "Abilitate use of RTS-CTS mechanism", useRtsCts);
    cmd.AddValue("useNetAnim", "Generate netanim files related ", useNetAnim );

    cmd.Parse(argc, argv);

// ! Enabling Logging on application

    // * Tracing

    if(verbose == true){
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

// ! CONFIGURATION BEGIN

    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nStations);

    NodeContainer wifiApNode;
    wifiApNode.Create(nAPnodes);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    Ssid ssid_value = Ssid(ssid);

    WifiHelper wifi;
    // wifi.EnableLogComponents(); // * Set wifi log component and the standard for wifi
    wifi.SetStandard(WIFI_STANDARD_80211g);
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

// * MAC
    NetDeviceContainer staDevices;
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid_value), "ActiveProbing", BooleanValue(false));
    staDevices = wifi.Install(phy, mac, wifiStaNodes);

    NetDeviceContainer apDevices;
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid_value));
    apDevices = wifi.Install(phy, mac, wifiApNode);

// * Mobility

    MobilityHelper mobility;

// ? How to set up?????
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(5.0),
                                  "DeltaY",
                                  DoubleValue(10.0),
                                  "GridWidth",
                                  UintegerValue(3),
                                  "LayoutType",
                                  StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds",
                              RectangleValue(Rectangle(-90, 90, -90, 90)));
    mobility.Install(wifiStaNodes);

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);

// * Internet stack and Ipv4

    InternetStackHelper stack;
    stack.Install(wifiApNode);
    stack.Install(wifiStaNodes);

    Ipv4AddressHelper address;

    address.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterface;
    Ipv4InterfaceContainer staInterface;
    apInterface = address.Assign(apDevices);
    staInterface =  address.Assign(staDevices);

    phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);

    if(useRtsCts == true){
        Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("100"));
        phy.EnablePcap("task2-on-4.pcap",staDevices.Get(n4),true,true);
        phy.EnablePcap("task2-on-AP.pcap",apDevices.Get(n0),true,true);
    }else{
        phy.EnablePcap("task2-off-4.pcap",staDevices.Get(n4),true,true);
        phy.EnablePcap("task2-off-AP.pcap",apDevices.Get(n0),true,true);
        
    }


// * Server

    UdpEchoServerHelper echoServer(UDP_PORT);
    ApplicationContainer serverApps = echoServer.Install(wifiStaNodes.Get(n0));
    // serverApps.Start(Seconds(1.0));
    // serverApps.Stop(Seconds(10.0));


// * CLient 1

    UdpEchoClientHelper echoClient1(apInterface.GetAddress(n0), UDP_PORT);
    echoClient1.SetAttribute("MaxPackets", UintegerValue(npackets));
    echoClient1.SetAttribute("Interval", TimeValue(Seconds(2.0)));
    echoClient1.SetAttribute("PacketSize", UintegerValue(packetSize));

    ApplicationContainer client1 = echoClient1.Install(wifiStaNodes.Get(n3));
    client1.Start(Seconds(2.0));
    client1.Stop(Seconds(4.0));

// * CLient 2
    UdpEchoClientHelper echoClient2(apInterface.GetAddress(n0),UDP_PORT);
    echoClient2.SetAttribute("MaxPackets", UintegerValue(npackets));
    echoClient2.SetAttribute("Interval", TimeValue(Seconds(3.0)));
    echoClient2.SetAttribute("PacketSize", UintegerValue(packetSize));
    
    ApplicationContainer client2 = echoClient2.Install(wifiStaNodes.Get(n4));
    client2.Start(Seconds(1.0));
    client2.Stop(Seconds(4.0));

// * Routing Table

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();




// * NetAnim
    if(useNetAnim == true){

        char str[40];
        
        if(useRtsCts == true){
            sprintf(str,"wireless-task2-rts-on.xml") ;
        }else{
            sprintf(str,"wireless-task2-rts-off.xml") ;
        }

        AnimationInterface anim(str); // Mandatory
        for (uint32_t i = 0; i < nStations; i++)
        {
            Ptr<Node> node = wifiStaNodes.Get(i);
            char f[20];
            if(i == n0){
                sprintf(f,"SRV-%" PRIu32, node->GetId());
                anim.UpdateNodeDescription(node,f); // Optional
                anim.UpdateNodeColor(node->GetId(),255,0,0);
            }else if(i == n3 || i == n4 ){
                sprintf(f,"CLi-%" PRIu32, node->GetId());
                anim.UpdateNodeDescription(node,f); // Optional
                anim.UpdateNodeColor(node->GetId(),0,255,0);
            }else{
                sprintf(f,"STA-%" PRIu32, node->GetId());
                anim.UpdateNodeDescription(node,f); // Optional
                anim.UpdateNodeColor(node->GetId(),0,0,255);
            }
        }
        
        // * AP node
        anim.UpdateNodeDescription(wifiApNode.Get(n0),"AP"); // Optional
        anim.UpdateNodeColor(wifiApNode.Get(n0)->GetId(),66,49,137);
              

        anim.EnablePacketMetadata(); // Optional
        anim.EnableWifiMacCounters(Seconds(0), Seconds(10)); // Optional
        anim.EnableWifiPhyCounters(Seconds(0), Seconds(10)); // Optional

        // anim.EnableIpv4RouteTracking("routingtable-wireless.xml",
        //                             Seconds(0),
        //                             Seconds(5),
        //                             Seconds(0.25));         // Optional


        Simulator::Stop(Seconds(10.0));
        Simulator::Run();
        Simulator::Destroy();
    }else{

        Simulator::Stop(Seconds(10.0));
        Simulator::Run();
        Simulator::Destroy();
    }
    return 0;
}
