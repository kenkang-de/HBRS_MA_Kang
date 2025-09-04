#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    std::ifstream file("battle_configs.csv");
    if (!file.is_open()) {
        std::cout << "Cannot open file!" << std::endl;
        return 1;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    std::cout << "Header: " << line << std::endl;
    
    if (!std::getline(file, line)) {
        std::cout << "No data line!" << std::endl;
        return 1;
    }
    
    std::cout << "Data: " << line << std::endl;
    
    std::stringstream ss(line);
    std::string token;
    int column = 0;
    
    while (std::getline(ss, token, ',')) {
        std::cout << "Column " << column << ": '" << token << "'" << std::endl;
        column++;
    }
    
    return 0;
}
