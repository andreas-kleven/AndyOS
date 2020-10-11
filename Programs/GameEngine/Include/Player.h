#pragma once
#include <string>
#include <vector>

struct Player
{
    int id;
    std::string name;
};

namespace PlayerManager {

Player *CreatePlayer(int id, const char *name);
void DestroyPlayer(int id);
Player *GetCurrentPlayer();
Player *GetPlayer(int id);
bool IsLocal();
bool SetPlayer(int id);
bool SetPlayer(const Player *player);
bool NextPlayer();

} // namespace PlayerManager
