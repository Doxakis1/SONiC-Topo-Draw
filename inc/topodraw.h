#pragma once

#include <yaml-cpp/yaml.h>
#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

struct Interface {
	std::string name;
	std::string ipv4;
	std::string ipv6;
};

struct Bp_interface {
	std::string ipv4;
	std::string ipv6;
};

struct Peer {
	std::string asn;
	std::string ipv4;
	std::string ipv6;
};

struct Bgp {
	std::string asn;
	std::vector<Peer> peers;
};

struct Properties {
	std::vector<std::string> properties;
};

struct Vm {
	std::string name;
	std::vector<std::string> vlans;
	std::string vm_offset;
};

struct Device {
	Properties m_Properties;
	Bgp m_Bgp;
	std::vector<Interface> m_Interfaces;
	Bp_interface m_Bp_Interface;
	Vm m_Vm;
};

const std::string topology_order[4] = {"T3", "T2", "T1", "T0"};
//Function definitions
Device parseDevice(const YAML::Node& node);
std::vector<Vm> parseVms(const YAML::Node& node);
