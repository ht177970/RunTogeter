#pragma once

#include <SFML/Graphics.hpp>
#include "Render.hpp"
#include "RenderManager.hpp"
#include "ResourceHolder.hpp"

namespace rpf {
	class GameOverMenu : public Render {
	public:
		GameOverMenu(RenderManager* rm, ResourceHolder* rh);
		void update() override;
		void handleEvent(sf::Event e) override;
	private:
		RenderManager* rm;
		ResourceHolder* rh;
		int m_text_index;
		std::vector<sf::Text*> clickable_texts;
		void init();
	};
}