#pragma once
#include <string>
#include <vector>

struct Player
{
    int id;
    std::string name;
};

namespace PlayerManager {

void CreatePlayer(int id, const char *name);
void DestroyPlayer(int id);
Player *GetCurrentPlayer();
Player *GetPlayer(int id);
bool IsLocal();
bool SetPlayer(int id);
bool NextPlayer();

} // namespace PlayerManager
