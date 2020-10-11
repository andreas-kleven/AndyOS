#include "Player.h"
#include "GEngine.h"

namespace PlayerManager {

int current_id = 0;
std::vector<Player *> players;

Player *CreatePlayer(int id, const char *name)
{
    Player *player = new Player();
    player->id = id;
    player->name = name;
    players.push_back(player);

    current_id = id;
    return player;
}

void DestroyPlayer(int id)
{}

Player *GetCurrentPlayer()
{
    return GetPlayer(current_id);
}

Player *GetPlayer(int id)
{
    if (id == 0)
        return players[0];

    for (auto *player : players) {
        if (player->id == id)
            return player;
    }

    return 0;
}

bool IsLocal()
{
    return current_id == players[0]->id;
}

bool SetPlayer(int id)
{
    if (id == 0)
        current_id = players[0]->id;
    else
        current_id = id;

    return true;
}

bool SetPlayer(const Player *player)
{
    if (!player)
        return false;

    current_id = player->id;
    return true;
}

bool NextPlayer()
{
    for (auto *player : players) {
        if (player->id > current_id) {
            current_id = player->id;
            return true;
        }
    }

    return false;
}

} // namespace PlayerManager
