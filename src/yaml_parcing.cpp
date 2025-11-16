#include "topodraw.h"

Device parseDevice(const YAML::Node& node) {
	Device device;

	if (node["properties"] && node["properties"].IsSequence()) {
		for (const auto& prop : node["properties"]) {
			device.m_Properties.properties.push_back(prop.as<std::string>());
		}
	}

	if (node["bgp"] && node["bgp"].IsMap()) {
		const YAML::Node& bgp = node["bgp"];
		if (bgp["asn"]) device.m_Bgp.asn = bgp["asn"].as<std::string>();
		if (bgp["peers"] && bgp["peers"].IsMap()) {
			for (const auto& peerEntry : bgp["peers"]) {
				Peer peer;
				peer.asn = peerEntry.first.as<std::string>();
				const YAML::Node& ips = peerEntry.second;
				if (ips.IsSequence()) {
					if (ips.size() > 0) peer.ipv4 = ips[0].as<std::string>();
					if (ips.size() > 1) peer.ipv6 = ips[1].as<std::string>();
				}
				device.m_Bgp.peers.push_back(peer);
			}
		}
	}

	if (node["interfaces"] && node["interfaces"].IsMap()) {
		for (const auto& ifaceEntry : node["interfaces"]) {
			Interface iface;
			iface.name = ifaceEntry.first.as<std::string>();
			const YAML::Node& ifaceNode = ifaceEntry.second;
			if (ifaceNode["ipv4"]) iface.ipv4 = ifaceNode["ipv4"].as<std::string>();
			if (ifaceNode["ipv6"]) iface.ipv6 = ifaceNode["ipv6"].as<std::string>();
			device.m_Interfaces.push_back(iface);
		}
	}

	if (node["bp_interface"] && node["bp_interface"].IsMap()) {
		const YAML::Node& bp = node["bp_interface"];
		if (bp["ipv4"]) device.m_Bp_Interface.ipv4 = bp["ipv4"].as<std::string>();
		if (bp["ipv6"]) device.m_Bp_Interface.ipv6 = bp["ipv6"].as<std::string>();
	}

	return device;
}

std::vector<Vm> parseVms(const YAML::Node& node) {
	std::vector<Vm> vms;

	for (const auto& vmEntry : node) {
		Vm vm;
		vm.name = vmEntry.first.as<std::string>();
		const YAML::Node& vmNode = vmEntry.second;
		if (vmNode["vm_offset"]) vm.vm_offset = vmNode["vm_offset"].as<std::string>();
		if (vmNode["vlans"]) {
			const YAML::Node& vlanEntryMap = vmNode["vlans"];
			std::vector<std::string> vlans;
			for (auto& vlanEntry : vlanEntryMap) {
				vlans.push_back(vlanEntry.as<std::string>());
			}
			vm.vlans = vlans;
		}
		vms.push_back(vm);
	}
	return vms;
}

