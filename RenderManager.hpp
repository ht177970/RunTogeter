#pragma once

#include <vector>
#include "SFML/Graphics.hpp"


namespace rpf {
	class RenderManager {
	public:
		RenderManager(sf::RenderWindow* window);
		~RenderManager();
		void Render();
		void setView(sf::View* v);
		void addGraphics(sf::Drawable* d);
		void delGraphic(sf::Drawable* d);
		void clear();
		sf::Vector2i getPosOnWindow(sf::Transformable* d);
	private:
		sf::RenderWindow* window;
		std::vector<sf::Drawable*> graps;
		sf::View* view;
	};
}