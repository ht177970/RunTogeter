#include "RenderManager.hpp"

namespace rpf {
	RenderManager::RenderManager(sf::RenderWindow* window) {
		this->window = window;
		this->view = nullptr;
	}

	RenderManager::~RenderManager() {

	}

	void RenderManager::Render() {
		window->clear();
		window->setView(view ? *view : window->getDefaultView());
		for (auto i : graps)
			window->draw(*i);
		window->display();
	}

	void RenderManager::setView(sf::View* v) {
		this->view = v;
	}

	void RenderManager::addGraphics(sf::Drawable* d) {
		graps.push_back(d);
	}

	void RenderManager::delGraphic(sf::Drawable* d) {
		for (auto g = graps.begin(); g != graps.end();g++) {
			if (*g == d) {
				graps.erase(g);
				break;
			}
		}
	}

	void RenderManager::clear() {
		//for (sf::Drawable* subgrap : graps) {
		//	subgrap->~Drawable();
		//	//free(subgrap);
		//}
		graps.clear();
		this->view = nullptr;
	}

	sf::Vector2i RenderManager::getPosOnWindow(sf::Transformable* d) {
		return window->mapCoordsToPixel(d->getPosition());
	}
}