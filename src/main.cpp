#include <iostream>
#include <string>


int main(int argc, char *argv[]) {
    while (true) {
        std::string cmd;
        std::getline(std::cin >> std::ws, cmd);
    }
    return 0;
}
