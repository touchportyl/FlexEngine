#include "CharacterMoves/moves.h"
#include "Components/charactercomponents.h"
#include "Components/battlecomponents.h"
namespace ChronoShift 
{

	#pragma region MoveData
	
	void Move_Empty(FlexECS::Entity& user, std::vector<FlexECS::Entity> targets) {

	}

	void Move_DualStrike(FlexECS::Entity& user, std::vector<FlexECS::Entity> targets) {
		for (FlexECS::Entity target : targets)
		{
			target.GetComponent<CharacterHealth>()->current_health -= 1;
		}
	}

	void Move_Whirlwind(FlexECS::Entity& user, std::vector<FlexECS::Entity> targets) {
		for (FlexECS::Entity target : targets)
		{
			target.GetComponent<CharacterHealth>()->current_health -= 2;
		}
		//modify speed buff/debuff here
	}

	void Move_Block(FlexECS::Entity& user, std::vector<FlexECS::Entity> targets) {
		//add block modifier here

	}

	void Move_RiftSurge(FlexECS::Entity& user, std::vector<FlexECS::Entity> targets) {
		for (FlexECS::Entity target : targets)
		{
			target.GetComponent<CharacterHealth>()->current_health -= 3;
		}
	}

	void Move_Flashbang(FlexECS::Entity& user, std::vector<FlexECS::Entity> targets) {
		for (FlexECS::Entity target : targets)
		{
			target.GetComponent<CharacterHealth>()->current_health -= 2;
			//Add dizzy
		}
	}

	void Move_TemporalWave(FlexECS::Entity& user, std::vector<FlexECS::Entity> targets) {
		for (FlexECS::Entity target : targets)
		{
			target.GetComponent<StatusEffects>()->speed.value -= 1;
			target.GetComponent<StatusEffects>()->speed.duration = 5;
		}
	}

	void Move_Poke(FlexECS::Entity& user, std::vector<FlexECS::Entity> targets) {
		for (FlexECS::Entity target : targets)
		{
			target.GetComponent<CharacterHealth>()->current_health -= 1;
		}
	}
	#pragma endregion;



	//declaration of statics
	std::unordered_map<std::string, Move> MoveRegistry::s_move_registry;


	void MoveRegistry::RegisterMoves()
	{
		FlexEngine::Log::Debug("Registering Moves:");
		Move EMPTY{ "", "YO", 0, MOVE_TARGET_NONE, 0, Move_Empty };
		Move dual_strike{ "Dual Strike", "Hit target 2 times for 1 damage", 4, MOVE_TARGET_SINGLE, 2, Move_DualStrike };
		Move whirlwind{ "Whirlwind", "Hits 2 Targets for 2 damage. Increases self speed by 1.", 7, MOVE_TARGET_SINGLE, 2, Move_Whirlwind };
		Move block{ "Block", "Takes no damage from moves.", 5, MOVE_TARGET_SELF, 0, Move_Block };
		Move rift_surge{ "Rift Surge", "Hits 2 targets for 3 damage", 7, MOVE_TARGET_SINGLE, 2, Move_RiftSurge };
		Move flashbang{ "Flashbang", "Hit 1 target for 2 damage and adds dizzy.", 5, MOVE_TARGET_SINGLE, 1, Move_Flashbang };
		Move temporal_wave{ "Temporal Wave", "Increase all drifter’s speed by 1.", 7, MOVE_TARGET_ALL_PLAYER, 0, Move_TemporalWave };
		Move poke{ "Poke", "Owie.", 7, MOVE_TARGET_SINGLE, 1, Move_Poke };

		s_move_registry[EMPTY.name] = EMPTY;
		s_move_registry[dual_strike.name] = dual_strike;
		s_move_registry[whirlwind.name] = whirlwind;
		s_move_registry[block.name] = block;
		s_move_registry[rift_surge.name] = rift_surge;
		s_move_registry[flashbang.name] = flashbang;
		s_move_registry[temporal_wave.name] = temporal_wave;
		s_move_registry[poke.name] = poke;
		FlexEngine::Log::Debug("Registering Moves finished");
	}

	Move MoveRegistry::GetMove(std::string name)
	{
		//return Instance().s_move_registry[name];
		return s_move_registry[name];
	}


	MoveRegistry::MoveRegistry()
	{
		//FlexEngine::Log::Flow("Registering Moves:");
		//Move EMPTY{ "", "YO", 0, MOVE_TARGET_NONE, 0, Move_Empty };
		//Move dual_strike{ "Dual Strike", "Hit target 2 times for 1 damage", 4, MOVE_TARGET_SINGLE, 2, Move_DualStrike };
		//Move whirlwind{ "Whirlwind", "Hits 2 Targets for 2 damage. Increases self speed by 1.", 7, MOVE_TARGET_SINGLE, 2, Move_Whirlwind };
		//Move block{ "Block", "Takes no damage from moves.", 5, MOVE_TARGET_SELF, 0, Move_Block };
		//Move rift_surge{ "Rift Surge", "Hits 2 targets for 3 damage", 7, MOVE_TARGET_SINGLE, 2, Move_RiftSurge };
		//Move flashbang{ "Flashbang", "Hit 1 target for 2 damage and adds dizzy.", 5, MOVE_TARGET_SINGLE, 1, Move_Flashbang };
		//Move temporal_wave{ "Temporal Wave", "Increase all drifter’s speed by 1.", 7, MOVE_TARGET_ALL_PLAYER, 0, Move_TemporalWave };
		//Move poke{ "Poke", "Owie.", 7, MOVE_TARGET_SINGLE, 1, Move_Poke};

		//m_move_registry[EMPTY.name]= EMPTY;
		//m_move_registry[dual_strike.name]= dual_strike;
		//m_move_registry[whirlwind.name]= whirlwind;
		//m_move_registry[block.name]= block;
		//m_move_registry[rift_surge.name]= rift_surge;
		//m_move_registry[flashbang.name]= flashbang;
		//m_move_registry[temporal_wave.name]= temporal_wave;
		//m_move_registry[poke.name]= poke;

		//MoveRegistry::Instance().m_move_registry.emplace(EMPTY.name, EMPTY);
		//MoveRegistry::Instance().m_move_registry.emplace(dual_strike.name, dual_strike);
		//MoveRegistry::Instance().m_move_registry.emplace(whirlwind.name, whirlwind);
		//MoveRegistry::Instance().m_move_registry.emplace(block.name, block);
		//MoveRegistry::Instance().m_move_registry.emplace(rift_surge.name, rift_surge);
		//MoveRegistry::Instance().m_move_registry.emplace(flashbang.name, flashbang);
		//MoveRegistry::Instance().m_move_registry.emplace(temporal_wave.name, temporal_wave);
		//MoveRegistry::Instance().m_move_registry.emplace(poke.name, poke);
		//FlexEngine::Log::Flow("Registering Moves finished");
	}

	MoveRegistry::~MoveRegistry()
	{
	}

}