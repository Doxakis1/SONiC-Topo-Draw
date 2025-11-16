#include "../inc/topodraw.h"

//unsigned int findFontSizeToFit(const sf::Font& font, const std::string& str, const sf::Vector2f& boxSize, float padding = 10.f) {
//	unsigned int fontSize = 1;
//	sf::Text text(str, font, fontSize);
//	while (true) {
//		text.setCharacterSize(fontSize);
//		sf::FloatRect bounds = text.getLocalBounds();
//		if (bounds.width > boxSize.x - padding || bounds.height > boxSize.y - padding)
//			break;
//		fontSize++;
//	}
//	return 1;
//}

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
		std::vector<Vm> vms = parseVms(VMsMap["VMs"]);

		for (const Vm& vm : vms)
		{
			for (auto& device : devices)
			{
				if (device.first == vm.name)
				{
					device.second.m_Vm = vm;
					break ;
				}
			}
		}
		sf::RenderWindow window(sf::VideoMode(1920,1200), "DrawTopology");
		window.setFramerateLimit(60);
		sf::Font font;
		if (!font.loadFromFile("assets/fonts/DejaVuSansMono.ttf")) {
			std::cerr << "Failed to load font" << std::endl;
			return 1;
		}
		std::map<unsigned int, std::vector<Device>> device_map;
		for (const auto& device_tuple : devices) {
			Device device = device_tuple.second;
			for ( unsigned int i = 0; i < sizeof(topology_order); i++){
				if (device.m_Vm.name.find(topology_order[i]) != std::string::npos) {
					device_map[i].push_back(device);
					break;
				}
			}
		}
		unsigned int x_coordinate = 25;
		unsigned int y_coordinate = 50;
		std::vector<sf::RectangleShape> neighbors;
		std::vector<sf::Text> neighbors_text;
		unsigned int dut_device_width = 0;
		unsigned int dut_device_height = 150;
		for (auto& map_entry : device_map) 
		{
			for ( auto& device : map_entry.second ) {
				sf::RectangleShape neigh(sf::Vector2f(150,150));
				neigh.setFillColor(sf::Color::Yellow);
				std::string boxLabel = device.m_Vm.name ;//+ "\nBPG: " + device.m_Bgp.asn + "\n" + device.m_Properties.properties.at(0) + "\n" +  device.m_Properties.properties.at(1);
				neigh.setPosition(x_coordinate, y_coordinate );
				sf::Text text1(boxLabel, font, 20);
				sf::FloatRect textBounds = text1.getLocalBounds();
				text1.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top  + textBounds.height / 2.0f);
				sf::Vector2f deviceCenter(neigh.getPosition().x + neigh.getSize().x / 2.0f, neigh.getPosition().y + neigh.getSize().y / 2.0f);
				text1.setFillColor(sf::Color::Black);
				text1.setPosition(deviceCenter);
				neighbors.push_back(neigh);
				neighbors_text.push_back(text1);
				x_coordinate += 175;
			}
			auto& last_item = neighbors.back();
			if (dut_device_width <= last_item.getPosition().x + 150) {
				dut_device_width = last_item.getPosition().x + 150;
			}
			x_coordinate = 25;
			y_coordinate += dut_device_height + 300;
		}
		sf::RectangleShape dut_device(sf::Vector2f(dut_device_width - 25, dut_device_height));
		dut_device.setFillColor(sf::Color::Blue);
		dut_device.setPosition(25, 300);
		sf::Text text1("DUT DEVICE", font, 100);
		sf::FloatRect textBounds = text1.getLocalBounds();
		text1.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top  + textBounds.height / 2.0f);
		sf::Vector2f deviceCenter(dut_device.getPosition().x + dut_device.getSize().x / 2.0f, dut_device.getPosition().y + dut_device.getSize().y / 2.0f);
		text1.setFillColor(sf::Color::White);
		text1.setPosition(deviceCenter);
		neighbors.push_back(dut_device);
		neighbors_text.push_back(text1);
		sf::View view = window.getDefaultView();
		while (window.isOpen()) {
			sf::Event event;
			while (window.pollEvent(event)) {
				if(event.type == sf::Event::Closed)
					window.close();
				if (event.type == sf::Event::MouseWheelScrolled) {
					if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
						float zoomFactor;
						if (event.mouseWheelScroll.delta > 0)
							zoomFactor = 0.9f;  // zoom in
						else
							zoomFactor = 1.1f;  // zoom out

						view.zoom(zoomFactor);
						window.setView(view);
					}
				}

				window.clear(sf::Color::White);  // White background
				for (auto& obj : neighbors) {
					window.draw(obj);
				}	
				for (auto& obj : neighbors_text) {
					window.draw(obj);
				}
				window.display();
			}
			}

		} catch (const YAML::Exception& e) {
			std::cerr << "YAML Exception: " << e.what() << "\n";
			return 1;
		} catch (const std::exception& e) {
			std::cerr << "Exception: " << e.what() << "\n";
			return 1;
		}

		return 0;
	}

