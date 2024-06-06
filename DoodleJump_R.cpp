#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <random>
#include <ctime>
#include <iomanip> // Для std::setfill и std::setw
#include <sstream> // Для std::stringstream


int main()
{
	sf::RenderWindow window(sf::VideoMode(500, 700), "Doodle Jump", sf::Style::Close); // it can't be resize
	window.setFramerateLimit(60);
	sf::Texture backgroundTexture;
	sf::Texture playerTexture;
	sf::Texture platformTexture;
	backgroundTexture.loadFromFile("background (2).png");
	playerTexture.loadFromFile("doodle.png");
	platformTexture.loadFromFile("platform.png");
	sf::Sprite background(backgroundTexture);
	sf::Sprite player(playerTexture);
	sf::Sprite platform(platformTexture);
	/*
	*      ----Size of images----
	*    background.png size: 500 * 700
	*    doodle.png     size:  80 *  80
	*    platform.png   size:  68 *  14
	*/

	sf::RectangleShape gameoverBackground(sf::Vector2f(500, 700));
	gameoverBackground.setFillColor(sf::Color::White);

	sf::Font font;
	font.loadFromFile("arial.ttf");
	sf::Text scoreText; 
	scoreText.setFont(font);
	scoreText.setCharacterSize(50);
	scoreText.setFillColor(sf::Color::Red);
	sf::Text timerText;
	timerText.setFont(font);
	timerText.setCharacterSize(50);
	timerText.setFillColor(sf::Color::Red);
	sf::Text gameoverText;
	gameoverText.setFont(font);
	gameoverText.setString("Game Over!");
	gameoverText.setCharacterSize(80);
	gameoverText.setFillColor(sf::Color::Red);
	sf::Text winText;
	winText.setFont(font);
	winText.setString("Win!");
	winText.setCharacterSize(80);
	winText.setFillColor(sf::Color::Green);

	// звук
	sf::SoundBuffer buffer;
	buffer.loadFromFile("By_winter.wav");
	sf::Sound sound;
	sound.setBuffer(buffer);

	// Инициализация платформ
	sf::Vector2u platformPosition[10];
	std::uniform_int_distribution<unsigned> x(0, 500 - platformTexture.getSize().x);
	std::uniform_int_distribution<unsigned> y(100, 700);
	std::default_random_engine e(time(0));
	for (size_t i = 0; i < 10; ++i)
	{
		platformPosition[i].x = x(e);
		platformPosition[i].y = y(e);
	}

	// позиция игрока и скорость падения вниз
	int playerX = 250;
	int playerY = 151;
	float dy = 0;
	int height = 150;
	int score = 0;

	// ограничивающий прямоугольник игрока. Он должен изменяться в зависимости от размера изображения
	const int PLAYER_LEFT_BOUNDING_BOX = 20;
	const int PLAYER_RIGHT_BOUNDING_BOX = 60;
	const int PLAYER_BOTTOM_BOUNDING_BOX = 70;

	// таймер
	sf::Clock gameTimer;

	// Флаг для победы
	bool gameWon = false;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				gameTimer.restart(); // Останавливаем таймер при закрытии окна
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			playerX -= 4;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			playerX += 4;

		
		if (playerX > 500)
			playerX = 0;
		if (playerX < -40)
			playerX = window.getSize().x - playerTexture.getSize().x;

		// счет
		// dy = -1.60001 - это предельная скорость, при которой игрок стоит на платформе и больше не поднимается
		// счет не может увеличиваться в этой ситуации
		if (playerY == height && dy < (-1.62))
		{
			score += 1;
			scoreText.setString("Score: " + std::to_string(score));
		}

		// механизм прыжка
		dy += 0.2;
		playerY += dy;

		if (playerY < height)
			for (size_t i = 0; i < 10; ++i)
			{
				playerY = height;
				platformPosition[i].y -= dy; // Вертикальное перемещение
				if (platformPosition[i].y >= 600) // Установка новой платформы вверху
				{
					platformPosition[i].y = 0;
					platformPosition[i].x = x(e);
				}

			}

		// detect jump on the platform
		for (size_t i = 0; i < 10; ++i)
		{
			if ((playerX + PLAYER_RIGHT_BOUNDING_BOX > platformPosition[i].x) && (playerX + PLAYER_LEFT_BOUNDING_BOX < platformPosition[i].x + platformTexture.getSize().x)        // player's horizontal range can touch the platform
				&& (playerY + PLAYER_BOTTOM_BOUNDING_BOX > platformPosition[i].y) && (playerY + PLAYER_BOTTOM_BOUNDING_BOX < platformPosition[i].y + platformTexture.getSize().y)  // player's vertical   range can touch the platform
				&& (dy > 0)) // player is falling
			{
				sound.play();
				dy = -10;
			}

		}
		player.setPosition(playerX, playerY);

		window.draw(background);
		window.draw(player);

		// установка и отрисовка платформ
		for (size_t i = 0; i < 10; ++i)
		{
			platform.setPosition(platformPosition[i].x, platformPosition[i].y);
			window.draw(platform);
		}

		// отображение таймера
		sf::Time elapsedTime = gameTimer.getElapsedTime();
		int minutes = elapsedTime.asSeconds() / 60;
		int seconds = static_cast<int>(elapsedTime.asSeconds()) % 60;
		int milliseconds = elapsedTime.asMilliseconds() % 1000 / 10; // Отображаем до десятых секунды

		std::stringstream ss;
		ss << std::setfill('0') << std::setw(2) << minutes << ':'
			<< std::setfill('0') << std::setw(2) << seconds << '.'
			<< std::setfill('0') << std::setw(1) << milliseconds;

		timerText.setString("Time: " + ss.str());

		// Проверка победы
		if (score >= 5000)
		{
			gameWon = true;
			break; // Выходим из основного цикла игры
		}

		// Обновляем счет перед отображением результата
		scoreText.setString("Score: " + std::to_string(score));


		if (playerY > 700)
        {
            gameoverText.setPosition(30, 200);
            // Изменяем позицию scoreText для отображения в левом верхнем углу
			scoreText.setPosition(150, 400);
            goto gameover;
        }
        window.draw(scoreText);
        window.display();

	}

	

	if (gameWon)
	{
		scoreText.setPosition(150, 400);
		
		// Центрирование текста
		sf::FloatRect textRect = winText.getLocalBounds();
		winText.setOrigin(textRect.left + textRect.width / 2.0f,
			textRect.top + textRect.height / 2.0f);
		winText.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));

		// Отображение таймера
		sf::Time elapsedTime = gameTimer.getElapsedTime();
		int minutes = elapsedTime.asSeconds() / 60;
		int seconds = static_cast<int>(elapsedTime.asSeconds()) % 60;
		int milliseconds = elapsedTime.asMilliseconds() % 1000 / 10; // Отображаем до десятых секунды

		std::stringstream ss;
		ss << std::setfill('0') << std::setw(2) << minutes << ':'
			<< std::setfill('0') << std::setw(2) << seconds << '.'
			<< std::setfill('0') << std::setw(1) << milliseconds;

		timerText.setString("Time: " + ss.str());

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
					gameTimer.restart(); // Останавливаем таймер при закрытии окна
				}
			}

			window.draw(gameoverBackground);
			window.draw(timerText);
			window.draw(winText);
			window.draw(scoreText);
			window.display();
		}
	}
	else
	{
		// Game Over
	gameover:
		// Отображение таймера в левом верхнем углу окна
		sf::Time elapsedTime = gameTimer.getElapsedTime();
		int minutes = elapsedTime.asSeconds() / 60;
		int seconds = static_cast<int>(elapsedTime.asSeconds()) % 60;
		int milliseconds = elapsedTime.asMilliseconds() % 1000 / 10; // Отображаем до десятых секунды

		std::stringstream ss;
		ss << std::setfill('0') << std::setw(2) << minutes << ':'
			<< std::setfill('0') << std::setw(2) << seconds << '.'
			<< std::setfill('0') << std::setw(1) << milliseconds;

		timerText.setString("Time: " + ss.str());

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
					gameTimer.restart(); // Останавливаем таймер при закрытии окна
				}
			}

			window.draw(gameoverBackground);
			window.draw(timerText);
			window.draw(gameoverText);
			window.draw(scoreText);
			window.display();
		}
	}

	
	return 0;
}