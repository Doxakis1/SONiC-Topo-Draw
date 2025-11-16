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
		std::cout << "We tried atleast3" << std::endl;
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

int main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << "USE: program <file_name>" << std::endl;	
		return 1;
	}
	try {
		YAML::Node config = YAML::LoadFile(av[1]);

		// parse devices
		if (!config["configuration"] || !config["configuration"].IsMap()) {
			std::cerr << "error: 'configuration' key missing or not a map\n";
			return 1;
		}
		const YAML::Node& configMap = config["configuration"];

		std::vector<std::pair<std::string, Device>> devices;
		for (const auto& deviceEntry : configMap) {
			std::string deviceName = deviceEntry.first.as<std::string>();
			Device device = parseDevice(deviceEntry.second);
			devices.emplace_back(deviceName, device);
		}

		if (!config["topology"] || !config["topology"].IsMap()) {
			std::cerr << "error: 'VMs' key missing or not a map\n";
			return 1;
		}
		const YAML::Node& VMsMap = config["topology"];
		// Parse VMs at top-level
		std::vector<Vm> vms = parseVms(VMsMap["VMs"]);

		// Print devices and their details
		for (const auto& [devName, device] : devices) {
			std::cout << "Device: " << devName << "\n";
			std::cout << "  BGP ASN: " << device.m_Bgp.asn << "\n";

			for (const auto& peer : device.m_Bgp.peers) {
				std::cout << "    Peer ASN: " << peer.asn
					<< ", IPv4: " << peer.ipv4
					<< ", IPv6: " << peer.ipv6 << "\n";
			}

			for (const auto& iface : device.m_Interfaces) {
				std::cout << "  Interface: " << iface.name
					<< ", IPv4: " << iface.ipv4
					<< ", IPv6: " << iface.ipv6 << "\n";
			}

			std::cout << "  BP Interface IPv4: " << device.m_Bp_Interface.ipv4
				<< ", IPv6: " << device.m_Bp_Interface.ipv6 << "\n";

			std::cout << "  Properties:\n";
			for (const auto& prop : device.m_Properties.properties) {
				std::cout << "    " << prop << "\n";
			}

			std::cout << std::endl;
		}

		// Print all VMs
		std::cout << "VMs:\n";
		for (const auto& vm : vms) {
			std::cout << "VM Name: " << vm.name << std::endl;
			std::cout << "\tvm_offset: " << vm.vm_offset << std::endl;
			std::cout << "\tvlans:" << std::endl;
			for (const auto& vlan : vm.vlans) {
				std::cout << "\t\t" << vlan << std::endl;
			}
		}
		std::cout << std::endl;

	} catch (const YAML::Exception& e) {
		std::cerr << "YAML Exception: " << e.what() << "\n";
		return 1;
	} catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
		return 1;
	}
	return 0;
}

