#pragma once

#include <yaml-cpp/yaml.h>
#include <iostream>
#include <vector>
#include <string>

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

struct Device {
	Properties m_Properties;
	Bgp m_Bgp;
	std::vector<Interface> m_Interfaces;
	Bp_interface m_Bp_Interface;
};

struct Vm {
	std::string name;
	std::vector<std::string> vlans;
	std::string vm_offset;
};

