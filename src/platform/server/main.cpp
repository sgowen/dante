/* 
 * File:   main.cpp
 * Author: sgowen
 *
 * Created on January 5, 2017, 4:18 PM
 */

#include "server.h"

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
}

#include <sstream>
#include <iomanip>
#include <map>
#include <vector>

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

class ClientTouchInput
{
public:
    std::string clientGuid;
    float x;
    float y;

    ClientTouchInput(std::string clientGuidIn, float xIn, float yIn) : clientGuid(clientGuidIn), x(xIn), y(yIn)
    {
        // Empty
    }
};

void insertIntoMap(std::map<std::string, ClientTouchInput*>& clients, std::string key, std::string clientGuid, float x, float y)
{
    if (clients.size() > 4)
    {
        for (std::map<std::string, ClientTouchInput*>::iterator itr = clients.begin(); itr != clients.end(); itr++)
        {
            delete (*itr).second;
        }
        
        clients.clear();
    }
    
    auto q2 = clients.find(key);

    if (q2 == clients.end())
    {
        clients[key] = new ClientTouchInput(clientGuid, x, y);
    }
    else
    {
        ClientTouchInput* cti = q2->second;
        cti->clientGuid = clientGuid;
        cti->x = x;
        cti->y = y;
    }
}

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());

    return tokens;
}

int main(int argc, const char * argv[])
{
    if (startServerUDP() > 0)
    {
        char buffer[MAX_DATA_SIZE];
        std::map<std::string, ClientTouchInput*> clients;

        std::string delim = ",";

        while (true)
        {
            bzero(buffer, MAX_DATA_SIZE);
            const char * clientId = readFromClient(buffer);

            printf("listener: got packet from %s\n", clientId);
            printf("listener: packet contains \"%s\"\n", buffer);

            std::stringstream ss2;
            ss2 << buffer;
            std::string input = ss2.str();

            std::vector<std::string> coords = split(input, delim);

            if (coords.size() == 3)
            {
                std::string clientGuid = coords.at(0);
                int xInt = ::atoi(coords.at(1).c_str());
                float x = xInt / 100.0f;
                int yInt = ::atoi(coords.at(2).c_str());
                float y = yInt / 100.0f;
                
                std::stringstream ss1;
                ss1 << clientId << "|" << clientGuid;
                std::string key = ss1.str();

                insertIntoMap(clients, key, clientGuid, x, y);

                std::stringstream ssO;
                for (std::map<std::string, ClientTouchInput*>::iterator itr = clients.begin(); itr != clients.end(); itr++)
                {
                    int xInt = (*itr).second->x * 100;
                    int yInt = (*itr).second->y * 100;
                    
                    ssO << (*itr).second->clientGuid;
                    ssO << ",";
                    ssO << xInt;
                    ssO << ",";
                    ssO << yInt;
                    ssO << ",";
                }

                std::string output = ssO.str();

                int status = writeToClients(output.c_str());
                if (!status)
                {
                    break;
                }
            }
        }
    }

    return 0;
}
