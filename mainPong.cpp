/*
Heavily based on SFML Pong example.
Attempts made at organizing code into separate files. Attepts abandoned due to prior wasting of time.

left paddle controlled by up/down arrow keys
right paddle computer player

bonus pong feature - 
	view of game constantly spinning to disorient player
	- without hardMode: left and right boundaries obviously marked, rotation speed increases during volley 
		and is reset after each point
	- with hard mode: left and right boundaries not obviously marked, rotation speeds up until end of game
	toggle hard mode by pressing "h", may be toggled during gameplay


*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>

#include "paddle.h";
#include "ball.h";

int main()
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// Define some constants
	const float pi = 3.14159f;
	const int gameWidth = 800;
	const int gameHeight = 600;
	sf::Vector2f paddleSize(25, 100);
	float ballRadius = 10.f;

	bool easyMode = true;
	bool hardMode = false;

	// create the window
	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight), "Dizzy Pong");

	// set a view
	sf::View view = window.getDefaultView();
	view.zoom(2.f);
	window.setView(view);
	float rVal = 0.005;

	// Load the sounds used in the game
	sf::SoundBuffer ballSoundBuffer;
	if (!ballSoundBuffer.loadFromFile("resources/ball.wav"))
		return EXIT_FAILURE;
	sf::Sound ballSound(ballSoundBuffer);
	
	sf::SoundBuffer outSoundBuffer;
	if (!outSoundBuffer.loadFromFile("resources/boxing_bell.wav"))
		return EXIT_FAILURE;
	sf::Sound outSound(outSoundBuffer);

	// set up text
	sf::Text text;
	int textXpos = 0;
	int textYpos = 0;
	int textSize = 44;
	sf::Color textColor = sf::Color::White;
	sf::Font font;
	if (!font.loadFromFile("resources/sansation.ttf"))
		return EXIT_FAILURE;
	text.setFont(font);
	text.setString("Hello!");
	text.setCharacterSize(textSize);
	text.setColor(textColor);
	text.setPosition(textXpos, textYpos);

	sf::Text pauseMessage;
	pauseMessage.setFont(font);
	pauseMessage.setString("Welcome to SFML pong!\nPress space to start the game");
	pauseMessage.setCharacterSize(textSize);
	pauseMessage.setColor(textColor);
	pauseMessage.setPosition(170.f, 150.f);

	sf::Text toggleInstruct;
	toggleInstruct.setFont(font);
	toggleInstruct.setString("Press h to toggle hard mode");
	toggleInstruct.setCharacterSize(textSize*.8);
	toggleInstruct.setColor(textColor);
	toggleInstruct.setPosition(gameWidth / 3, gameHeight - 50);

	sf::Text playerScore;
	int pScore = 0;
	playerScore.setFont(font);
	playerScore.setString(std::to_string(pScore));
	playerScore.setCharacterSize(textSize);
	playerScore.setColor(textColor);
	playerScore.setPosition(170.f, 150.f);

	sf::Text compScore;
	int cScore = 0;
	compScore.setFont(font);
	compScore.setString(std::to_string(cScore));
	compScore.setCharacterSize(textSize);
	compScore.setColor(textColor);
	compScore.setPosition(370.f, 150.f);

	//set up visual boundaries
	sf::Vector2f boundSize(gameWidth, gameHeight);
	sf::RectangleShape boundary(boundSize);
	boundary.setFillColor(sf::Color::Black);
	boundary.setOutlineColor(sf::Color::White);
	boundary.setOutlineThickness(10);

	sf::Color boundColor;
	if (easyMode) boundColor = sf::Color::Red;
	else boundColor = sf::Color::Black;

	sf::RectangleShape leftBound(sf::Vector2f(15, gameHeight));
	leftBound.setPosition(sf::Vector2f(-15, 0));
	leftBound.setFillColor(boundColor);
	sf::RectangleShape rightBound(sf::Vector2f(15, gameHeight));
	rightBound.setPosition(gameWidth, 0);
	rightBound.setFillColor(boundColor);
	
	
	//set up left paddle
	sf::Vector2f paddlePosL(10 + paddleSize.x / 2, gameHeight / 2);
	Paddle leftPaddle = Paddle::Paddle(paddleSize, sf::Color::White, paddlePosL);
	
	//set up right paddle
	sf::Vector2f paddlePosR((gameWidth-10)-paddleSize.x/2, gameHeight / 2);
	Paddle rightPaddle = Paddle::Paddle(paddleSize, sf::Color::White, paddlePosR);

	//set up ball
	Ball ball = Ball::Ball(ballRadius);

	// Define the paddles properties
	sf::Clock AITimer;
	const sf::Time AITime = sf::seconds(0.1f);
	const float paddleSpeed = 400.f;
	float rightPaddleSpeed = 0.f;
	const float ballSpeed = 400.f;
	float ballAngle = 0.f; // to be changed later

	sf::Clock clock;
	bool isPlaying = false;
	bool newRound = true;
	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				window.close();
				break;
			}

			// h key pressed: toggle hardMode
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::H))
			{
				hardMode = !hardMode;
				if (hardMode) boundColor = sf::Color::Black;
				else if (!hardMode) boundColor = sf::Color::Red;
				leftBound.setFillColor(boundColor);
				rightBound.setFillColor(boundColor);
			}

			// Space key pressed: play
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
			{

				if (!isPlaying)
				{
					// (re)start the game
					isPlaying = true;
					clock.restart();

					// Reset the position of the paddles and ball
					leftPaddle.paddle.setPosition(10 + paddleSize.x / 2, gameHeight / 2);
					rightPaddle.paddle.setPosition(gameWidth - 10 - paddleSize.x / 2, gameHeight / 2);
					ball.ball.setPosition(gameWidth / 2, gameHeight / 2);

					// Reset the ball angle
					do
					{
						// Make sure the ball initial angle is not too much vertical
						ballAngle = (std::rand() % 360) * 2 * pi / 360;
					} while (std::abs(std::cos(ballAngle)) < 0.7f);
				}
			}
		}

		if (isPlaying)
		{
			float deltaTime = clock.restart().asSeconds();


			if (newRound){
				// Reset the position of the paddles and ball
				leftPaddle.paddle.setPosition(10 + paddleSize.x / 2, gameHeight / 2);
				rightPaddle.paddle.setPosition(gameWidth - 10 - paddleSize.x / 2, gameHeight / 2);
				ball.ball.setPosition(gameWidth / 2, gameHeight / 2);

				// Reset the ball angle
				do
				{
					// Make sure the ball initial angle is not too much vertical
					ballAngle = (std::rand() % 360) * 2 * pi / 360;
				} while (std::abs(std::cos(ballAngle)) < 0.7f);
				newRound = false;
			}

			
			// Move the player's paddle
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
				(leftPaddle.paddle.getPosition().y - paddleSize.y / 2 > 5.f))
			{
				leftPaddle.paddle.move(0.f, -paddleSpeed * deltaTime);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
				(leftPaddle.paddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f))
			{
				leftPaddle.paddle.move(0.f, paddleSpeed * deltaTime);
			}

			// Move the computer's paddle
			if (((rightPaddleSpeed < 0.f) && (rightPaddle.paddle.getPosition().y - paddleSize.y / 2 > 5.f)) ||
				((rightPaddleSpeed > 0.f) && (rightPaddle.paddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)))
			{
				rightPaddle.paddle.move(0.f, rightPaddleSpeed * deltaTime);
			}

			// Update the computer's paddle direction according to the ball position
			if (AITimer.getElapsedTime() > AITime)
			{
				AITimer.restart();
				if (ball.ball.getPosition().y + ballRadius > rightPaddle.paddle.getPosition().y + paddleSize.y / 2)
					rightPaddleSpeed = paddleSpeed;
				else if (ball.ball.getPosition().y - ballRadius < rightPaddle.paddle.getPosition().y - paddleSize.y / 2)
					rightPaddleSpeed = -paddleSpeed;
				else
					rightPaddleSpeed = 0.f;
			}
			

			// Move the ball
			float factor = ballSpeed * deltaTime;
			ball.ball.move(std::cos(ballAngle) * factor, std::sin(ballAngle) * factor);


			// Check collisions between the ball and the screen

			//offscreen, play out sound
			if (ball.ball.getPosition().x - ballRadius < 0.f)
			{
				outSound.play();
				if (cScore >= 3)
				{
					isPlaying = false;
					pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
					cScore = 0;
					pScore = 0;
					compScore.setString(std::to_string(cScore));
					playerScore.setString(std::to_string(pScore));
					newRound = true;
					rVal = 0;
				}
				else{
					cScore = cScore + 1;
					compScore.setString(std::to_string(cScore));
					newRound = true;
					if (!hardMode) rVal = 0;
					//pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
				}
			}
			if (ball.ball.getPosition().x + ballRadius > gameWidth)
			{
				outSound.play();
				if (pScore >= 3)
				{
					isPlaying = false;
					pauseMessage.setString("You won!\nPress space to restart or\nescape to exit");
					pScore = 0;
					cScore = 0;
					compScore.setString(std::to_string(cScore));
					playerScore.setString(std::to_string(pScore));
					newRound = true;
					rVal = 0;
				}
				else{
					pScore++;
					playerScore.setString(std::to_string(pScore));
					newRound = false;
					if (!hardMode) rVal = 0;
					//pauseMessage.setString("You won!\nPress space to restart or\nescape to exit");
				}
			}

			//collisions with walls, play bounce sound
			if (ball.ball.getPosition().y - ballRadius < 0.f)
			{
				ballSound.play();
				ballAngle = -ballAngle;
				ball.ball.setPosition(ball.ball.getPosition().x, ballRadius + 0.1f);
			}
			if (ball.ball.getPosition().y + ballRadius > gameHeight)
			{
				ballSound.play();
				ballAngle = -ballAngle;
				ball.ball.setPosition(ball.ball.getPosition().x, gameHeight - ballRadius - 0.1f);
			}

			// Check the collisions between the ball and the paddles
			// Left Paddle
			if (ball.ball.getPosition().x - ballRadius < leftPaddle.paddle.getPosition().x + paddleSize.x / 2 &&
				ball.ball.getPosition().x - ballRadius > leftPaddle.paddle.getPosition().x &&
				ball.ball.getPosition().y + ballRadius >= leftPaddle.paddle.getPosition().y - paddleSize.y / 2 &&
				ball.ball.getPosition().y - ballRadius <= leftPaddle.paddle.getPosition().y + paddleSize.y / 2)
			{
				if (ball.ball.getPosition().y > leftPaddle.paddle.getPosition().y)
					ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
				else
					ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;

				ballSound.play();
				ball.ball.setPosition(leftPaddle.paddle.getPosition().x + ballRadius + paddleSize.x / 2 + 0.1f, 
					ball.ball.getPosition().y);
				if (!hardMode) rVal += .003;
			}

			// Right Paddle
			if (ball.ball.getPosition().x + ballRadius > rightPaddle.paddle.getPosition().x - paddleSize.x / 2 &&
				ball.ball.getPosition().x + ballRadius < rightPaddle.paddle.getPosition().x &&
				ball.ball.getPosition().y + ballRadius >= rightPaddle.paddle.getPosition().y - paddleSize.y / 2 &&
				ball.ball.getPosition().y - ballRadius <= rightPaddle.paddle.getPosition().y + paddleSize.y / 2)
			{
				if (ball.ball.getPosition().y > rightPaddle.paddle.getPosition().y)
					ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
				else
					ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;

				ballSound.play();
				ball.ball.setPosition(rightPaddle.paddle.getPosition().x - ballRadius - paddleSize.x / 2 - 0.1f, 
					ball.ball.getPosition().y);
				if (!hardMode) rVal += .003;
			}

			
		}

		// clear the window with black color
		window.clear(sf::Color::Black);

		// draw boundaries
		window.draw(boundary);
		window.draw(leftBound);
		window.draw(rightBound);
		

		if (isPlaying)
		{
			//roatate the view
			if (hardMode) rVal += 0.000001;
			view.rotate(rVal);
			window.setView(view);
			// Draw the paddles, score, and the ball
			window.draw(leftPaddle.paddle);
			window.draw(rightPaddle.paddle);
			window.draw(playerScore);
			window.draw(compScore);
			window.draw(ball.ball);
		}
		else
		{
			// Draw the pause message
			if (newRound){
				view.setRotation(0);
				window.setView(view);
				window.draw(pauseMessage);
				window.draw(toggleInstruct);
			}
		}
		

		// end the current frame
		window.display();
	}

	return 0;
}