#pragma once

#include "SFML/Graphics.hpp"

namespace rpf {
	class Map : public sf::Drawable, public sf::Transformable
	{
	public:
		bool load(const std::string& tileset, sf::Vector2u tileSize, std::vector<int> tiles, unsigned int width, unsigned int height);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		int m_width;
		int m_height;
	private:
		sf::VertexArray m_vertices;
		sf::Texture m_tileset;
	};
}