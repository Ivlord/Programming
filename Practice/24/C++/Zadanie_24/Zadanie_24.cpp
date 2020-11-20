//
// v.3.0
//
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

using json = nlohmann::json; 
using namespace std;

int main()
{   setlocale(LC_ALL, "Russian");

    std::ifstream inFile("in.json"); json j_in; inFile >> j_in;
    json j_out = json::array();
    j_out.push_back({ {"Total", 1} });

    
    for (auto i : j_in) {        

        std::string id = to_string(i["userId"]); //.get<int>()

        if (i["completed"].get<bool>()){
            if (j_out[0].contains(id)) {
                int m = j_out[0][id].get<int>(); //.get<int>()
                j_out[0][id] = ++m;
            }
            else j_out[0].push_back( { id, 1}  );
        }
    }

    j_in.clear(); j_in= json::array();
    for (auto& el : j_out[0].items())
    {
        if (el.key() != "Total")
        {
            j_in.push_back({ {"task_completed", el.value()} , {"userId", el.key() } });
        }
    }

    std::ofstream outFile("out.json"); //std::fstream stream("out.json");
    outFile << j_out; 
    return 0;
}