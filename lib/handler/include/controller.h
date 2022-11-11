#pragma once

#include "handler.h"
#include "Room.h"
#include "Player.h"
#include <map>
#include <string> 
#include "Server.h"
#include <nlohmann/json.hpp>

enum Status {
    SUCCESS,
    FAIL
};

struct Response {
    Status status;
    std::string message;
};

class Controller{

public:
    std::unordered_map<std::string, Room> GameRoomLookUp;
    std::unordered_map<networking::Connection, int, networking::ConnectionHash> playerLookUp;
    
    // create room from json file and host player connection
    //returns the random code to the room
    Response createRoom(json jsonFile, networking::Connection connectionInfo);

    Response joinRoom(std::string roomCode, networking::Connection connectionInfo);

    Response leaveRoom(std::string roomCode, networking::Connection connectionInfo);

    Response handleUserInput(json userInput);

    
private:
    std::string generateRoomCode();
    Room createRoomFromConfig(json jsonFile, Player host);
};