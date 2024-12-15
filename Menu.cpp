#include "Menu.hpp"
#include "Core.hpp"
#include "MySocket.cpp"

#include <iostream>

namespace rpf {

#pragma region ClickableMenu

	ClickableMenu::ClickableMenu(RenderManager* _rm, ResourceHolder* _rh) {
		this->rm = _rm;
		this->rh = _rh;
		m_text_index = -1;
	}

	void ClickableMenu::handleEvent(sf::Event e) {
		switch (e.type) {
		case sf::Event::KeyPressed:
			onKeyDown(e.key.code);
			break;
		case sf::Event::MouseMoved:
			onMouseMove(e.mouseMove);
			break;
		case sf::Event::MouseButtonPressed:
			onMouseClick();
			break;
		}
	}

	void ClickableMenu::update() {
		for (Text* text : m_clickable_texts) {
			text->updateText();
			//text->grap.rotate(1);
		}
	}

	int ClickableMenu::changeTextIndex(int new_index) {
		this->m_text_index = new_index;
		return this->m_text_index;
	}

	void ClickableMenu::onKeyDown(sf::Keyboard::Key keycode) {
		switch (keycode) {
		case sf::Keyboard::Up:
			if (changeTextIndex(m_text_index - 1) < 0)
				changeTextIndex(this->getClickableSize() - 1);
			break;
		case sf::Keyboard::Down:
			if (changeTextIndex(m_text_index + 1) > this->getClickableSize() - 1)
				changeTextIndex(0);
			break;
		case sf::Keyboard::Enter:
			EnterPressed(m_text_index);
			break;
		default:
			otherKeyDown(keycode);
			break;
		}
	}

	void ClickableMenu::onMouseMove(sf::Event::MouseMoveEvent mouse) {
		if (rm->view) {
			mouse.x += rm->view->getCenter().x - rh->s_width / 2;
			mouse.y += rm->view->getCenter().y - rh->s_height / 2;
		}
		for (auto t : this->getBaseClickable()) {
			if (t->isPosIn(sf::Vector2i(mouse.x, mouse.y))) {
				changeTextIndex(t->getId());
				break;
			}
		}
	}

	void ClickableMenu::onMouseClick() {
		sf::Vector2i mousepos = sf::Mouse::getPosition(*rm->window);
		if (rm->view) {
			mousepos.x += rm->view->getCenter().x - rh->s_width / 2;
			mousepos.y += rm->view->getCenter().y - rh->s_height / 2;
		}
		if (this->getBaseClickable()[m_text_index]->isPosIn(mousepos))
			EnterPressed(m_text_index);
	}

#pragma endregion


#pragma region Text/Textbox

	Text::Text(sf::String text, Pos position, sf::Font font) {
		this->m_id = -1;//default
		this->m_font = font;
		this->grap = sf::Text(text, m_font, 30U);//default
		this->grap.setOrigin(grap.getLocalBounds().width / 2, grap.getLocalBounds().height / 2);
		this->grap.setPosition(position.x, position.y);
		this->index = nullptr;
	}

	void Text::setId(int id) {
		this->m_id = id;
	}

	void Text::setTextIndexPointer(int* text_index) {
		this->index = text_index;
	}

	void Text::setTextSize(unsigned int size) {
		this->grap.setCharacterSize(size);
		this->grap.setOrigin(grap.getLocalBounds().width / 2, grap.getLocalBounds().height / 2);
		this->TextChanged();
	}

	void Text::setTextColor(sf::Color color) {
		this->grap.setFillColor(color);
	}

	int Text::getId() {
		return m_id;
	}

	void Text::updateText() {
		if (this->index) {
			if (*this->index == m_id) {
				this->setTextColor(sf::Color::Yellow);
				this->grap.setScale(1.1f, 1.1f); // 聚焦時放大
			}
			else {
				this->setTextColor(sf::Color::White);
				this->grap.setScale(1.0f, 1.0f); // 恢復原大小
			}
		}
	}

	bool Text::isPosIn(sf::Vector2i pos) {
		sf::Vector2f grapPos = grap.getPosition();
		sf::FloatRect grap_rect = grap.getLocalBounds();
		pos.x -= grapPos.x + grap_rect.left - grap_rect.width / 2;
		pos.y -= grapPos.y + grap_rect.top - grap_rect.height / 2;
		return 0 <= pos.x && pos.x <= grap_rect.width &&
			0 <= pos.y && pos.y <= grap_rect.height;
	}

	TextBox::TextBox(Pos position, sf::Font font, unsigned int size, RenderManager* rm) {
		this->m_font = font;
		this->rm = rm;

		// 初始化框
		box.setSize(sf::Vector2f(300.f, 50.f));
		box.setFillColor(sf::Color::White);
		box.setOutlineColor(sf::Color::Black);
		box.setOutlineThickness(2.f);
		box.setOrigin(box.getSize().x / 2, box.getSize().y / 2);  // 設置中心原點
		box.setPosition(position.x, position.y);

		// 初始化輸入文本
		inputText = sf::Text(sf::String(), m_font, size);
		inputText.setFillColor(sf::Color::Black);
		inputText.setPosition(box.getPosition().x - box.getSize().x / 2 + 10, box.getPosition().y - inputText.getCharacterSize() / 2);

		// 初始化占位符文本
		placeholderText = sf::Text("", m_font, size);
		placeholderText.setFillColor(sf::Color(150, 150, 150));
		placeholderText.setPosition(inputText.getPosition());

		isFocused = false;
	}

	void TextBox::updateVisuals() {
		if (isFocused) {
			box.setOutlineColor(sf::Color::Blue);  // 聚焦時外框顏色變藍
			box.setOutlineThickness(3.f);         // 增加框線厚度
		}
		else {
			box.setOutlineColor(sf::Color::Black); // 失焦時恢復黑色
			box.setOutlineThickness(2.f);         // 恢復框線厚度
		}
	}

	void TextBox::handleEvent(sf::Event e) {
		if (e.type == sf::Event::MouseButtonPressed) {
			sf::Vector2i pixelPos = sf::Mouse::getPosition(*rm->window);
			sf::Vector2f worldPos = rm->window->mapPixelToCoords(pixelPos);
			isFocused = box.getGlobalBounds().contains(worldPos);
			updateVisuals();
		}
		else if (isFocused && e.type == sf::Event::TextEntered) {
			if (e.text.unicode == '\b' && !inputText.getString().isEmpty()) {
				// Backspace 處理
				sf::String text = inputText.getString();
				text.erase(text.getSize() - 1);
				inputText.setString(text);
			}
			else if (e.text.unicode >= 32 && e.text.unicode < 128) {
				// 限制 ASCII 輸入範圍
				inputText.setString(inputText.getString() + e.text.unicode);
			}
		}

		// 占位符顯示
		if (inputText.getString().isEmpty()) {
			placeholderText.setFillColor(sf::Color(150, 150, 150));
		}
		else {
			placeholderText.setFillColor(sf::Color::Transparent);
		}
	}

	void TextBox::setPlaceholder(const sf::String& text) {
		placeholderText.setString(text);
	}

	sf::String TextBox::getInputText() const {
		return inputText.getString();
	}

	sf::RectangleShape* TextBox::getBox() {
		return &box;
	}

	sf::Text* TextBox::getInputTextGraphic() {
		return &inputText;
	}

	sf::Text* TextBox::getPlaceholderTextGraphic() {
		return &placeholderText;
	}

#pragma endregion

#pragma region effects

	void createGradientBackground(sf::VertexArray& background, sf::Color topColor, sf::Color bottomColor, sf::Vector2f size) {
		background.setPrimitiveType(sf::Quads);
		background.resize(4);

		background[0].position = sf::Vector2f(0, 0);
		background[1].position = sf::Vector2f(size.x, 0);
		background[2].position = sf::Vector2f(size.x, size.y);
		background[3].position = sf::Vector2f(0, size.y);

		background[0].color = topColor;
		background[1].color = topColor;
		background[2].color = bottomColor;
		background[3].color = bottomColor;
	}


	std::vector<sf::CircleShape> stars;

	void createStarfield(int numStars, sf::Vector2f size, std::vector<sf::CircleShape>& stars) {
		stars.clear();
		for (int i = 0; i < numStars; ++i) {
			sf::CircleShape star(1.f);
			star.setFillColor(sf::Color::White);
			star.setPosition(rand() % static_cast<int>(size.x), rand() % static_cast<int>(size.y));
			stars.push_back(star);
		}
	}

	void updateStarfield(std::vector<sf::CircleShape>& stars, sf::Vector2f size) {
		for (auto& star : stars) {
			star.move(0.f, 0.5f); // 星星向下移動
			if (star.getPosition().y > size.y) {
				star.setPosition(rand() % static_cast<int>(size.x), 0); // 重置到頂部
			}
		}
	}

	void drawStarfield(RenderManager* rm, std::vector<sf::CircleShape>& stars) {
		for (auto& star : stars) {
			rm->addGraphics(&star);
		}
	}

#pragma endregion


#pragma region MainMenu

	MainMenu::MainMenu(RenderManager* rm, ResourceHolder* rh) : ClickableMenu(rm, rh) {
		this->rm = rm;
		this->rh = rh;
		this->initMenu();
	}

	void MainMenu::initMenu() {
		sf::VertexArray* gradient = new sf::VertexArray();
		createGradientBackground(*gradient, sf::Color(30, 30, 120), sf::Color(10, 10, 40), sf::Vector2f(rh->s_width, rh->s_height));
		rm->addGraphics(gradient);

		createStarfield(100, sf::Vector2f(rh->s_width, rh->s_height), stars);
		drawStarfield(rm, stars);

		sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(rh->s_width, rh->s_height));
		rect->setFillColor(sf::Color::Cyan);
		rect->setOrigin(rh->s_width / 2, 0);
		rect->setPosition(rh->view->getCenter().x, 0);
		rm->addGraphics(rect);

		sf::String texts[] = { "Play with others", "Quit the game" };
		Pos position = Pos(rh->s_width / 2, rh->s_height / 4);
		unsigned int text_size;
		text_size = static_cast<unsigned int>(30.f * position.x / 500);
		for (int i = 0; i < 2; i++) {
			Text* tmp = new Text(texts[i], position.AddY(text_size * 2), rh->font);
			tmp->setTextSize(30U);
			tmp->setId(i);
			tmp->setTextIndexPointer(&m_text_index);
			m_clickable_texts.push_back(tmp);
		}
		for (Text* t : m_clickable_texts)
			rm->addGraphics(&t->grap);
		m_text_index = 0;
	}

	void MainMenu::update() {
		updateStarfield(stars, sf::Vector2f(rh->s_width, rh->s_height));
		ClickableMenu::update();
	}

	void MainMenu::EnterPressed(int index) {
		if (index == 0)
			Core::CORE->switchMode(Mode::CONNECTION_MENU);
		else if (index == 1)
			Core::CORE->switchMode(Mode::CLOSED);
	}

#pragma endregion

#pragma region GameOverMenu

	GameOverMenu::GameOverMenu(RenderManager* rm, ResourceHolder* rh) : ClickableMenu(rm, rh) {
		this->rm = rm;
		this->rh = rh;
		this->initMenu();
	}

	void GameOverMenu::EnterPressed(int index) {
		if (index == 0)
			Core::CORE->switchMode(Mode::MAIN_MENU);
		else if (index == 1)
			Core::CORE->switchMode(Mode::CLOSED);
	}

	void GameOverMenu::initMenu() {
		sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(rh->s_width, rh->s_height));
		rect->setFillColor(sf::Color(0x00000088));
		rect->setOrigin(rh->s_width / 2, 0);
		rect->setPosition(rh->view->getCenter().x, 0);
		rm->addGraphics(rect);

		sf::String texts[] = { "Back to Menu"/*, L"回到選單"*/, "Quit the game" };
		Pos text_position(rh->view->getCenter().x, rh->view->getCenter().y - rh->s_height / 4);
		unsigned int text_size;
		text_size = static_cast<unsigned int>(30.f * text_position.x / 500);
		for (int i = 0; i < 2; i++) {
			Text* tmp = new Text(texts[i], text_position.AddY(text_size * 2), rh->font);
			tmp->setTextSize(30U);
			tmp->setId(i);
			tmp->setTextIndexPointer(&m_text_index);
			m_clickable_texts.push_back(tmp);
		}
		for (Text* t : m_clickable_texts)
			rm->addGraphics(&t->grap);
		m_text_index = 0;
	}

#pragma endregion

#pragma region ConnectionMenu

	ConnectionMenu::ConnectionMenu(RenderManager* rm, ResourceHolder* rh) : ClickableMenu(rm, rh) {
		this->rm = rm;
		this->rh = rh;
		this->initMenu();
	}

	void ConnectionMenu::initMenu() {
		sf::VertexArray* gradient = new sf::VertexArray();
		createGradientBackground(*gradient, sf::Color(30, 30, 120), sf::Color(10, 10, 40), sf::Vector2f(rh->s_width, rh->s_height));
		rm->addGraphics(gradient);

		createStarfield(100, sf::Vector2f(rh->s_width, rh->s_height), stars);
		drawStarfield(rm, stars);

		sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(rh->s_width, rh->s_height));
		rect->setFillColor(sf::Color::Blue);
		rect->setOrigin(rh->s_width / 2, 0);
		rect->setPosition(rh->view->getCenter().x, 0);
		rm->addGraphics(rect);

		errorText = sf::Text("", rh->font, 20U);
		errorText.setFillColor(sf::Color::Transparent);//初始為透明
		rm->addGraphics(&errorText);

		Pos position(rh->s_width / 2, rh->s_height / 4);
		textBox = new TextBox(position.AddY(50), rh->font, 20U, rm);
		textBox->setPlaceholder("Enter IP Address");

		// 確保文本框正確對齊
		textBox->getBox()->setOrigin(textBox->getBox()->getSize().x / 2, textBox->getBox()->getSize().y / 2);
		textBox->getInputTextGraphic()->setPosition(position.x - 140, position.y - 15);  // 偏移值根據字體大小微調

		// 將 TextBox 的圖形物件添加到 RenderManager
		rm->addGraphics(textBox->getBox());
		rm->addGraphics(textBox->getInputTextGraphic());
		rm->addGraphics(textBox->getPlaceholderTextGraphic());

		sf::String texts[] = { "Connect", "Back", "Create Game" };
		for (int i = 0; i < 3; i++) {
			Text* tmp = new Text(texts[i], position.AddY(100), rh->font);
			tmp->setTextSize(30U);
			tmp->setId(i);
			tmp->setTextIndexPointer(&m_text_index);
			m_clickable_texts.push_back(tmp);
		}

		for (Text* t : m_clickable_texts)
			rm->addGraphics(&t->grap);
		m_text_index = 0;
	}

	void ConnectionMenu::update() {
		updateStarfield(stars, sf::Vector2f(rh->s_width, rh->s_height));
		ClickableMenu::update();
	}

	void handleMsgServer() {//For test, not code here
		int nowid = 1;
		MySocket* sock = Core::CORE->sock;
		std::stringstream& in = sock->sin;
		std::stringstream& out = sock->sout;
		std::string msg;
		while (1) {
			std::string receivedData;
			{
				std::lock_guard<std::mutex> lock(sock->sin_mutex);
				receivedData = in.str();
				in.str("");
				in.clear();
			}
			if (!receivedData.empty()) {
				std::cout << "[Received]: " << receivedData << std::endl;
				std::stringstream handler(receivedData);
				handler >> msg;
				if (msg == "ask") {
					nowid++;
					out << "players";
					out << nowid;
					for (int i = 1; i <= nowid; i++)
						out << i;
					out << nowid;
					out.flush();
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	void handleMsgClient(RoomMenu* menu) {//For test, not code here
		int nowid = 1;
		MySocket* sock = Core::CORE->sock;
		std::stringstream& in = sock->sin;
		std::stringstream& out = sock->sout;
		std::string msg;
		bool first = 0;
		while (1) {
			std::string receivedData;
			{
				std::lock_guard<std::mutex> lock(sock->sin_mutex);
				receivedData = in.str();
				in.str("");
				in.clear();
			}
			if (!receivedData.empty()) {
				std::cout << "[Received]: " << receivedData << std::endl;
				std::stringstream handler(receivedData);
				handler >> msg;
				if (msg == "players") {
					if (first) {
						int N;
						handler >> N;
						int tmp;
						for (int i = 0; i < N; i++)
							handler >> tmp;
						handler >> tmp;
						continue;//ignore
					}
					menu->leaved.push_back(1);
					first = 1;
					int N;
					handler >> N;
					int tmp;
					for (int i = 0; i < N; i++)
						handler >> tmp, menu->joined.push_back(tmp);
					handler >> tmp;
					menu->selfId = tmp;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	void ConnectionMenu::EnterPressed(int index) {
		if (index == 0) {
			sf::String ip = textBox->getInputText();
			std::cout << "Connecting to IP: " << ip.toAnsiString() << std::endl;

			Core::CORE->sock = new MySocket();
			MySocket* sock = Core::CORE->sock;
			bool connectionSuccess = sock->connect(ip, 55072);
			if (connectionSuccess) {
				//std::vector<int> players = { 1, 2, 3, 4 }; // 模擬玩家清單
				//int selfId = 2; // 假設自己的編號
				{
					std::lock_guard<std::mutex> lock(sock->sout_mutex);
					sock->sout << "ask" << "\n";
				}
				int selfId = 1;
				std::vector<int> players = { 1 };
				//int selfId = 3;
				//std::lock_guard<std::mutex> lock(sock->sin_mutex);
				/*std::string tmp;
				while (sock->sin.eof())
					sleep(50);
				sock->sin >> tmp;
				int N = 10;
				sock->sin >> N;
				std::vector<int> players(N);
				for (auto& i : players)
					sock->sin >> i;
				sock->sin >> selfId;*/
				RoomMenu* menu = new RoomMenu(rm, rh, players, selfId);
				Core::CORE->switchMode(menu);

				std::thread([this, menu]() { handleMsgClient(menu); }).detach();
			}
			else {
				errorText.setString("Connection failed!"); // 顯示錯誤
				errorText.setFillColor(sf::Color::Red);
				errorText.setPosition(textBox->getBox()->getPosition().x, textBox->getBox()->getPosition().y + 80);
				errorText.setPosition(
					textBox->getBox()->getPosition().x,
					textBox->getBox()->getPosition().y + textBox->getBox()->getSize().y / 2 + 10
				);
				errorText.setOrigin(errorText.getLocalBounds().left + errorText.getLocalBounds().width / 2,
					errorText.getLocalBounds().top + errorText.getLocalBounds().height / 2);
				//rm->addGraphics(&errorText);
			}
		}
		else if (index == 1) {
			Core::CORE->switchMode(Mode::MAIN_MENU);
		}
		else if (index == 2) {
			std::cout << "Creating a new game..." << std::endl;

			Core::CORE->sock = new MySocket();
			MySocket* sock = Core::CORE->sock;
			sock->host(55072);

			std::vector<int> players = { 1 };
			int selfId = 1;
			Core::CORE->switchMode(new RoomMenu(rm, rh, players, selfId));

			std::thread([this]() { handleMsgServer(); }).detach();
		}
	}

	void ConnectionMenu::handleEvent(sf::Event e) {
		textBox->handleEvent(e);
		ClickableMenu::handleEvent(e);
	}

#pragma endregion

	RoomMenu::RoomMenu(RenderManager* rm, ResourceHolder* rh, std::vector<int> players, int selfId)
		: ClickableMenu(rm, rh), players(players), selfId(selfId) {
		rm->clear();
		this->m_font = rh->font;
		this->initMenu();
	}

	void RoomMenu::initMenu() {
		sf::VertexArray* gradient = new sf::VertexArray();
		createGradientBackground(*gradient, sf::Color(30, 30, 120), sf::Color(10, 10, 40), sf::Vector2f(rh->s_width, rh->s_height));
		rm->addGraphics(gradient);

		createStarfield(100, sf::Vector2f(rh->s_width, rh->s_height), stars);
		drawStarfield(rm, stars);

		renderPlayers();

		sf::String texts[] = { "Leave Room", "Test del", "Test add"};
		Pos position(rh->s_width / 2, rh->s_height - 300); // 放在底部 //-100
		for (int i = 0; i < 3; i++) {
			Text* tmp = new Text(texts[i], position.AddY(40), rh->font);
			tmp->setTextSize(30U);
			tmp->setId(i);
			tmp->setTextIndexPointer(&m_text_index);
			m_clickable_texts.push_back(tmp);
		}
		for (Text* t : m_clickable_texts)
			rm->addGraphics(&t->grap);
		m_text_index = 0;
	}

	void RoomMenu::renderPlayers() {
		Pos position(rh->s_width / 2, 240); // 起始位置
		unsigned int textSize = 25U;
		for (int playerId : players) {
			sf::Text* playerText = new sf::Text();
			playerText->setFont(m_font);
			playerText->setString(playerId == selfId ? "Player " + std::to_string(playerId) + " (You)" : "Player " + std::to_string(playerId));
			playerText->setCharacterSize(textSize);
			playerText->setFillColor(playerId == selfId ? sf::Color::Green : sf::Color::White);
			playerText->setOrigin(playerText->getLocalBounds().width / 2, playerText->getLocalBounds().height / 2);
			playerText->setPosition(position.x, position.y);
			position.y += 40; // 每個玩家往下排
			playerTexts.push_back({ playerId, playerText });
			rm->addGraphics(playerText);
		}
	}

	void RoomMenu::EnterPressed(int index) {
		if (index == 0) { // Leave Room
			Core::CORE->switchMode(Mode::MAIN_MENU);
		}
		else if (index == 1) {
			leaved.push_back(2);
		}
		if (index == 2) {
			joined.push_back(5);
		}
	}

	void RoomMenu::update() {
		updateStarfield(stars, sf::Vector2f(rh->s_width, rh->s_height));
		bool repos = 0;
		if (!leaved.empty()) {
			for (int id : leaved) {
				for (auto it = playerTexts.begin(); it != playerTexts.end();) {
					if (it->first == id) {
						rm->delGraphic(it->second);
						free(it->second);
						it = playerTexts.erase(it);
					}
					else
						it++;
				}
			}
			leaved.clear();
			repos = 1;
		}
		if (!joined.empty()) {
			Pos position(rh->s_width / 2, 240); // 起始位置
			unsigned int textSize = 25U;
			for (int playerId : joined) {
				sf::Text* playerText = new sf::Text();
				playerText->setFont(m_font);
				playerText->setString(playerId == selfId ? "Player " + std::to_string(playerId) + " (You)" : "Player " + std::to_string(playerId));
				playerText->setCharacterSize(textSize);
				playerText->setFillColor(playerId == selfId ? sf::Color::Green : sf::Color::White);
				playerText->setOrigin(playerText->getLocalBounds().width / 2, playerText->getLocalBounds().height / 2);
				playerText->setPosition(position.x, position.y);
				position.y += 40; // 每個玩家往下排
				playerTexts.push_back({ playerId, playerText });
				rm->addGraphics(playerText);
			}
			joined.clear();
			repos = 1;
		}
		if (repos) {
			Pos position(rh->s_width / 2, 240); // 起始位置
			for (std::pair<int, sf::Text*> element : playerTexts) {
				sf::Text* playerText = element.second;
				playerText->setPosition(position.x, position.y);
				position.y += 40;
			}
		}
		ClickableMenu::update();
	}

}