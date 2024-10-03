#include "turnorderdisplay.h"
#include "Components/battlecomponents.h"
#include "Components/rendering.h"

namespace ChronoShift 
{

	//The Sprite2d renderer will automatically draw it, so we just set the positions, colors, etc.
	void DisplayTurnOrder(std::list<FlexECS::Entity> &queue)
	{
		auto scene = FlexECS::Scene::GetActiveScene();
		int i{ 0 };
		for (auto entity : scene->View<TurnOrderDisplay, IsActive, ZIndex, Position, Scale, Shader, Sprite>())
		{
			if (i >= queue.size())
			{
				entity.GetComponent<IsActive>()->is_active = false;
				continue;
			}
			std::list<FlexECS::Entity>::const_iterator it = queue.begin();
			std::advance(it, i);
			auto character = *it;

			entity.GetComponent<IsActive>()->is_active = true;
			entity.GetComponent<Position>()->position = { 50.f, 100.f + (80.f * i) };
			entity.GetComponent<Sprite>()->color = character.GetComponent<Sprite>()->color;
			entity.GetComponent<Sprite>()->color_to_add = character.GetComponent<Sprite>()->color_to_add;
			entity.GetComponent<Sprite>()->color_to_multiply = character.GetComponent<Sprite>()->color_to_multiply;
			entity.GetComponent<Sprite>()->texture = character.GetComponent<Sprite>()->texture;
			++i;
		}
	}

}

