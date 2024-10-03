#include "battlesystem.h"

#include "States.h"
#include "Layers.h"

#include "Components/battlecomponents.h"
#include "Components/charactercomponents.h"
#include "Components/physics.h"
#include "Components/rendering.h"

#include "BattleSystems/turnorderdisplay.h"
#include "BattleSystems/targetingsystem.h"

#include "Physics/box2d.h"
#include "Renderer/sprite2d.h"


namespace ChronoShift {


  //Sorting functions
  struct SortLowestSpeed
  {
    bool operator()(FlexECS::Entity e1, FlexECS::Entity e2) {
      return e1.GetComponent<CharacterSpeed>()->current_speed < e2.GetComponent<CharacterSpeed>()->current_speed;
    }
  };


  BattleSystem::BattleSystem()
  {
  }
  BattleSystem::~BattleSystem()
  {
  }
  void BattleSystem::InitializeBattleSlots()
  {
    auto scene = FlexECS::Scene::GetActiveScene();
    const Vector3 color_player_slot = { 0.45f, 0.58f, 0.32f };
    const Vector3 color_enemy_slot = { 0.77f, 0.12f, 0.23f };

    for (int i = 0; i < m_slots.size(); i++)
    {
      bool is_player_slot = (i < 4);
      Vector2 position;
      if (is_player_slot) position = { 200.f + 120.f * i, 600.f };
      else                position = { 700.f + 120.f * (i - 4), 200.f };

      FlexECS::Entity slot = FlexECS::Scene::CreateEntity();
      slot.AddComponent<BattleSlot>({ i, FlexECS::Entity::Null });
      slot.AddComponent<OnHover>({});
      slot.AddComponent<OnClick>({});
      slot.AddComponent<IsActive>({ true });
      slot.AddComponent<Position>({ position });
      slot.AddComponent<Scale>({ { 100,100 } });
      slot.AddComponent<ZIndex>({ 9 });
      slot.AddComponent<Sprite>({
        scene->Internal_StringStorage_New(R"()"),
        is_player_slot ? color_player_slot : color_enemy_slot,
        Vector3::Zero,
        Vector3::One,
        Renderer2DProps::Alignment_Center
       });
      slot.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });

      m_slots[i] = slot;
    }

    for (int i = 0; i < 4; i++)
    {
      Vector2 position = { 900.f, 450.f + (80.f * i) };

      FlexECS::Entity move_button = FlexECS::Scene::CreateEntity();
      move_button.AddComponent<MoveButton>({ i });
      move_button.AddComponent<OnHover>({});
      move_button.AddComponent<OnClick>({});
      move_button.AddComponent<IsActive>({ false });
      move_button.AddComponent<Position>({ position });
      move_button.AddComponent<Scale>({ { 350,70 } });
      move_button.AddComponent<ZIndex>({ 9 });
      move_button.AddComponent<Sprite>({
        scene->Internal_StringStorage_New(R"()"),
        Vector3::One,
        Vector3::Zero,
        Vector3::One - Vector3(0.10f * i, 0.10f * i , 0.10f * i),
        Renderer2DProps::Alignment_Center
       });
      move_button.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
    }
  }

  void BattleSystem::SetMovesForWeapon(FlexECS::Entity weapon)
  {
    //this is just here to cleanify code.... to remove..... thanks.....
    FlexECS::Entity move1 = FlexECS::Scene::CreateEntity();
    FlexECS::Entity move2 = FlexECS::Scene::CreateEntity();
    FlexECS::Entity move3 = FlexECS::Scene::CreateEntity();
    //FlexECS::Entity move4 = FlexECS::Scene::CreateEntity();
    move1.AddComponent<MoveID>({});
    move2.AddComponent<MoveID>({});
    move3.AddComponent<MoveID>({});
    //move4.AddComponent<MoveID>({});

    auto movelist = weapon.GetComponent<Weapon>();
    movelist->weapon_move_one = move1;
    movelist->weapon_move_two = move2;
    movelist->weapon_move_three = move3;
    //movelist->weapon_move_four = move4;
  }

  void BattleSystem::AddCharacter(FlexECS::Entity character, int position)
  {
    //SetMovesForCharacter(character);
    m_characters.push_back(character);
    m_slots[position].GetComponent<BattleSlot>()->character = character;

  }

  void BattleSystem::BeginBattle()
  {
    for (FlexECS::Entity entity : m_characters)
    {
      m_speedstack.push_back(entity);
    }
    m_speedstack.sort(SortLowestSpeed());

    auto scene = FlexECS::Scene::GetActiveScene();
    std::cout << "Beginning Stack Order:\nName / HP / Speed \n";
    for (auto& entity : m_speedstack)
    {
      std::cout << scene->Internal_StringStorage_Get(entity.GetComponent<CharacterName>()->character_name)
        << "  HP: " << entity.GetComponent<CharacterHealth>()->current_health
        << "  Spd: " << entity.GetComponent<CharacterSpeed>()->current_speed << std::endl;
    }
    std::cout << "\n";

    FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->View<BattleState>()[0];
    battle_state.GetComponent<BattleState>()->phase = BP_PROCESSING;
  }

  void BattleSystem::UpdateSpeedStack()
  {
    auto scene = FlexECS::Scene::GetActiveScene();
    int speed_to_decrease = m_speedstack.front().GetComponent<CharacterSpeed>()->current_speed;
    for (auto& entity : m_speedstack)
    {
      entity.GetComponent<CharacterSpeed>()->current_speed -= speed_to_decrease;
      std::cout << scene->Internal_StringStorage_Get(entity.GetComponent<CharacterName>()->character_name)
        << "  HP: " << entity.GetComponent<CharacterHealth>()->current_health
        << "  Spd: " << entity.GetComponent<CharacterSpeed>()->current_speed << std::endl;
    }
    std::cout << "\n";

    FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->View<BattleState>()[0];
    battle_state.GetComponent<BattleState>()->active_character = m_speedstack.front();

    if (m_speedstack.front().GetComponent<IsPlayer>()->is_player) {
      battle_state.GetComponent<BattleState>()->phase = BP_PLAYER_TURN;
    }
    else {
      battle_state.GetComponent<BattleState>()->phase = BP_ENEMY_TURN;
    }
  }

  void BattleSystem::Update()
  {
    FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->View<BattleState>()[0];
    int battle_phase = battle_state.GetComponent<BattleState>()->phase;

    if (battle_phase == BP_PROCESSING) {
      UpdateSpeedStack();
    }
    else if (battle_phase == BP_PLAYER_TURN) {
      //Choose a Move
      PlayerMoveSelection();
    }
    else if (battle_phase == BP_ENEMY_TURN) {
      m_speedstack.front().GetComponent<Action>()->move_to_use = 
        static_cast<FlexECS::Entity>(m_speedstack.front().GetComponent<CharacterWeapon>()->equipped_weapon).GetComponent<Weapon>()->weapon_move_one;
      battle_state.GetComponent<BattleState>()->current_target_count = 1;
      battle_state.GetComponent<BattleState>()->target_one = 0;
      battle_state.GetComponent<BattleState>()->phase = BP_MOVE_EXECUTION;

    }
    else if (battle_phase == BP_MOVE_TARGET_SELECTION) {
      PlayerTargetSelection();
    }
    else if (battle_phase == BP_MOVE_EXECUTION) {
      ExecuteMove();
    }
  }

  void BattleSystem::PlayerMoveSelection()
  {
    FlexECS::Entity move_to_use = FlexECS::Entity::Null;
    for (auto& entity : FlexECS::Scene::GetActiveScene()->View<IsActive, MoveButton>()) {
      entity.GetComponent<IsActive>()->is_active = true;
    }
    //move selection system
    GetMoveSelection();
    for (auto& entity : FlexECS::Scene::GetActiveScene()->View<OnClick, MoveButton>())
    {
      auto& click_status = entity.GetComponent<OnClick>()->is_clicked;
      if (click_status == true)
      {
        switch (entity.GetComponent<MoveButton>()->move_number) {
        case 0:
          move_to_use = static_cast<FlexECS::Entity>(m_speedstack.front().GetComponent<CharacterWeapon>()->equipped_weapon).GetComponent<Weapon>()->weapon_move_one;
          break;
        case 1:
          move_to_use = static_cast<FlexECS::Entity>(m_speedstack.front().GetComponent<CharacterWeapon>()->equipped_weapon).GetComponent<Weapon>()->weapon_move_two;
          break;
        case 2:
          move_to_use = static_cast<FlexECS::Entity>(m_speedstack.front().GetComponent<CharacterWeapon>()->equipped_weapon).GetComponent<Weapon>()->weapon_move_three;
          break;
        case 3:
          //move_to_use = m_speedstack.front().GetComponent<MoveList>()->move_four;
          break;
        }
      }
    }

    if (move_to_use == FlexECS::Entity::Null) return;

    m_speedstack.front().GetComponent<Action>()->move_to_use = move_to_use;

    FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->View<BattleState>()[0];
    battle_state.GetComponent<BattleState>()->phase = BP_MOVE_TARGET_SELECTION;

    for (auto& entity : FlexECS::Scene::GetActiveScene()->View<IsActive, MoveButton>()) {
      entity.GetComponent<IsActive>()->is_active = false;
    }


    //printing info
    FlexECS::Entity player = battle_state.GetComponent<BattleState>()->active_character;
    FlexECS::Entity selected_move = player.GetComponent<Action>()->move_to_use;
    Move move = MoveRegistry::GetMove(selected_move.GetComponent<MoveID>()->move_name);
    std::cout << "Move Selected: " << move.name << std::endl;
  }

  void BattleSystem::PlayerTargetSelection()
  {
    FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->View<BattleState>()[0];
    auto& target_count = battle_state.GetComponent<BattleState>()->current_target_count;

    FlexECS::Entity player = battle_state.GetComponent<BattleState>()->active_character;
    FlexECS::Entity move_to_use = player.GetComponent<Action>()->move_to_use;
    Move move = MoveRegistry::GetMove(move_to_use.GetComponent<MoveID>()->move_name);

    if (target_count < move.target_count) {
      GetTargetSelection();

      for (auto& entity : FlexECS::Scene::GetActiveScene()->View<OnClick, BattleSlot>())
      {
        auto& click_status = entity.GetComponent<OnClick>()->is_clicked;
        if (click_status == true)
        {
          int selection = entity.GetComponent<BattleSlot>()->slot_number;
          target_count += 1;
          switch (target_count) {
          case 1:
            battle_state.GetComponent<BattleState>()->target_one = selection;
            break;
          case 2:
            battle_state.GetComponent<BattleState>()->target_two = selection;
            break;
          case 3:
            battle_state.GetComponent<BattleState>()->target_three = selection;
            break;
          case 4:
            battle_state.GetComponent<BattleState>()->target_four = selection;
            break;
          case 5:
            battle_state.GetComponent<BattleState>()->target_five = selection;
            break;
          }
        }
      }
    }
    if (target_count >= move.target_count) {
      battle_state.GetComponent<BattleState>()->phase = BP_MOVE_EXECUTION;
    }
  }



  void BattleSystem::ExecuteMove()
  {
    //get the move user
    FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->View<BattleState>()[0];
    FlexECS::Entity user = battle_state.GetComponent<BattleState>()->active_character;
    FlexECS::Entity move_to_use = user.GetComponent<Action>()->move_to_use;
    Move move = MoveRegistry::GetMove(move_to_use.GetComponent<MoveID>()->move_name);

    std::vector<FlexECS::Entity> targets;

    if (move.target_type == MOVE_TARGET_SINGLE)
    {
      switch (move.target_count) {
      case 5:
        targets.insert(targets.begin(), static_cast<FlexECS::Entity>(m_slots[battle_state.GetComponent<BattleState>()->target_five].GetComponent<BattleSlot>()->character));
        [[fallthrough]];
      case 4:
        targets.insert(targets.begin(), static_cast<FlexECS::Entity>(m_slots[battle_state.GetComponent<BattleState>()->target_four].GetComponent<BattleSlot>()->character));
        [[fallthrough]];
      case 3:
        targets.insert(targets.begin(), static_cast<FlexECS::Entity>(m_slots[battle_state.GetComponent<BattleState>()->target_three].GetComponent<BattleSlot>()->character));
        [[fallthrough]];
      case 2:
        targets.insert(targets.begin(), static_cast<FlexECS::Entity>(m_slots[battle_state.GetComponent<BattleState>()->target_two].GetComponent<BattleSlot>()->character));
        [[fallthrough]];
      case 1:
        targets.insert(targets.begin(), static_cast<FlexECS::Entity>(m_slots[battle_state.GetComponent<BattleState>()->target_one].GetComponent<BattleSlot>()->character));
        break;
      }
    }
    else if (move.target_type == MOVE_TARGET_SELF) {
      targets.push_back(user);
    }
    else if (move.target_type == MOVE_TARGET_ALL_ENEMY) {
      for (FlexECS::Entity target : m_characters)
      {
        if (!target.GetComponent<IsPlayer>()->is_player)
          targets.push_back(target);
      }
    }
    else if (move.target_type == MOVE_TARGET_ALL_PLAYER) {
      for (FlexECS::Entity target : m_characters)
      {
        if (target.GetComponent<IsPlayer>()->is_player)
          targets.push_back(target);
      }
    }

    //execute move
    move.effect(user, targets);
    user.GetComponent<CharacterSpeed>()->current_speed += move.cost + user.GetComponent<CharacterSpeed>()->base_speed;
    user.GetComponent<Action>()->move_to_use = FlexECS::Entity::Null;
    m_speedstack.sort(SortLowestSpeed());

    //Display now info
    auto scene = FlexECS::Scene::GetActiveScene();
    for (auto& entity : m_speedstack)
    {
      std::cout << scene->Internal_StringStorage_Get(entity.GetComponent<CharacterName>()->character_name)
        << "  HP: " << entity.GetComponent<CharacterHealth>()->current_health
        << "  Spd: " << entity.GetComponent<CharacterSpeed>()->current_speed << std::endl;
    }
    std::cout << "\n";
    battle_state.GetComponent<BattleState>()->current_target_count = 0;
    battle_state.GetComponent<BattleState>()->active_character = FlexECS::Entity::Null;
    battle_state.GetComponent<BattleState>()->phase = BP_PROCESSING;
  }

}
