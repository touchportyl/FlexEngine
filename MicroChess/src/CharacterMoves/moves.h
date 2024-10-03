#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include <FlexEngine.h>

using namespace FlexEngine;
namespace ChronoShift
{
  enum MOVE_TARGET_TYPE : int {
    MOVE_TARGET_NONE = 1,
    MOVE_TARGET_ALL_ENEMY = 2,
    MOVE_TARGET_ALL_PLAYER = 3,
    MOVE_TARGET_SINGLE = 4,
    MOVE_TARGET_SELF = 5,
    MOVE_TARGET_NULL = -100
  };
  using MoveEffect = std::function<void(FlexECS::Entity& user, std::vector<FlexECS::Entity> targets)>;
  struct Move {
    std::string name; //use to access map of moves
    std::string description;
    
    int cost;

    int target_type;
    int target_count; //how many times you need to target
    //std::pair<MOVE_TARGET_TYPE, int>
    MoveEffect effect;
  };

  class MoveRegistry {
  public:
    MoveRegistry(MoveRegistry&) = delete;
    MoveRegistry& operator=(MoveRegistry &) = delete;

    static void RegisterMoves();

    static Move GetMove(std::string move_name);

  private:
    MoveRegistry();
    ~MoveRegistry();

    static std::unordered_map<std::string, Move> s_move_registry;  //this one used for the moves in the specific battle only

  };
}