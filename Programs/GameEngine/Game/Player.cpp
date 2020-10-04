#include "Player.h"
#include "GEngine.h"

namespace PlayerManager {

int current_id = 0;
std::vector<Player *> players;

void CreatePlayer(int id, const char *name)
{
    Player *player = new Player();
    player->id = id;
    player->name = name;
    players.push_back(player);

    current_id = id;
    GEngine::game->CreatePlayer();
}

void DestroyPlayer(int id)
{}

Player *GetCurrentPlayer()
{
    return GetPlayer(current_id);
}

Player *GetPlayer(int id)
{
    for (auto *player : players) {
        if (player->id == id)
            return player;
    }

    return 0;
}

bool IsLocal()
{
    return current_id == 0;
}

bool SetPlayer(int id)
{
    current_id = id;
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
