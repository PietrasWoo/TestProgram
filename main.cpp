#include <iostream>
#include <fstream>
#include "rapidxml.hpp"
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>

void findNodes(rapidxml::xml_node<char> *rootNode, std::map<std::string, std::string> &values);

enum DataFormat {
    AAA,
    NAS
};

DataFormat dataFormat;

const std::vector<std::string> nodeNamesAAA = {"flstkennz", "flaeche", "landschl", "kreisschl", "gmdschl", "gemaschl"};
const std::vector<std::string> nodeNamesNAS = {"flurstueckskennzeichen", "amtlicheFlaeche", "land", "kreis", "gemeinde", "gemarkungsnummer"};
const std::vector<std::string> nodeNames    = {"Numer dzialki", "Wielkosc dzialki", "Numer landu", "Numer okregu", "Numer powiatu", "Numer gemarkung"};

int main(int argc, char* argv[])
{
    std::cout << std::endl;
    std::string fileName;
    if (argc < 2) {
        std::cout << "Prosze podac nazwe pliku i zatwierdzic naciskajac ENTER" << std::endl;
        std::getline(std::cin, fileName);
        std::cout << std::endl;
    } else {
        fileName = argv[1];
    }

    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cout << "Nie udalo sie otworzyc pliku: " << fileName << std::endl;
        std::cout << std::endl << "Nacisnij ENTER aby wyjsc";
        std::getchar();
        return 1;
    }

    if (fileName.find("aaa") != std::string::npos)
        dataFormat = AAA;
    else if (fileName.find("nas") != std::string::npos)
        dataFormat = NAS;
    else {
        std::cout << "Format danych nie obslugiwany" << std::endl;
        std::cout << std::endl << "Nacisnij ENTER aby wyjsc";
        std::getchar();
        return 2;
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    buffer.push_back('\0');

    rapidxml::xml_document<> document;
    document.parse<0>(&buffer[0]);
    auto rootNode = document.first_node();

    std::map<std::string, std::string> values;
    findNodes(rootNode, values);
    for (auto& value : values) {
        std::cout << value.first << ": " << value.second << std::endl;
    }

    std::cout << std::endl << "Nacisnij ENTER aby wyjsc";
    std::getchar();

    return 0;
}

void findNodes(rapidxml::xml_node<char> *rootNode, std::map<std::string, std::string> &values) {
    if (!rootNode)
        return;

    if (rootNode->type() != 1)
        return;

    for (auto node = rootNode->first_node(); node; node = node->next_sibling()) {
        std::string name(node->name());
        std::vector<std::string> nodes;
        if (dataFormat == AAA)
            nodes = nodeNamesAAA;
        else if (dataFormat == NAS)
            nodes = nodeNamesNAS;

        auto it = std::find(nodes.begin(), nodes.end(), name);
        if (it != nodes.end()) {
            int ind = std::distance(nodes.begin(), it);
            values.insert({nodeNames.at(ind), node->value()});
        } else {
            findNodes(node, values);
        }
    }
}
