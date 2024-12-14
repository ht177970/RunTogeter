#pragma once

#include <vector>
#include "SFML/Graphics.hpp"


namespace rpf {
	class RenderManager {
	public:
		RenderManager(sf::RenderWindow* window);
		~RenderManager() = default;
		void Render();
		void setView(sf::View* v);
		void addGraphics(sf::Drawable* d);
		void delGraphic(sf::Drawable* d);
		void clear();
		sf::Vector2i getPosOnWindow(sf::Transformable* d);
		sf::RenderWindow* window;
	//private:
		std::vector<sf::Drawable*> graps;
		sf::View* view;
	};
}