#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
using namespace sf;

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 600;

class Car
{
public:
    float posX, posY, speed, angle;
    int carId;

    Car(int carId) : carId(carId)
    {
        speed = 0;
        angle = 0;
    }

    void move()
    {
        posX += sin(angle) * speed;
        posY -= cos(angle) * speed;
    }
};

class Input
{
public:
    bool keyW, keyS, keyA, keyD, keyUp, keyDown, keyLeft, keyRight;
    Input() : keyW(0), keyS(0), keyA(0), keyD(0), keyUp(0), keyDown(0), keyLeft(0), keyRight(0) {}
};

class SplitRacers
{
public:
    RenderWindow window;

    SoundBuffer engineSoundBuffer;
    Sound engineSound, engineSound2;

    Car *player1;
    Car *player2;

    View player1View, player2View;
    Vector2f player1CenterPosition, player2CenterPosition;

    Input playersInput;

    Texture *mapTexture;
    Sprite mapSprite;

    Image *obstacleMaskImage;
    Sprite obstacleMaskSprite;
    Texture *obstacleMaskTexture;

    Texture *carTexture;
    Sprite carSprite;

    IntRect frameSelection;

    float maxSpeed, accelFactor, decelFactor, turnSpeed;
    bool isDebug;

    SplitRacers() : window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Split Racers")
    {
        window.setFramerateLimit(60);

        player1View = View();
        player2View = View();

        player1CenterPosition = Vector2f(0, 0);
        player2CenterPosition = Vector2f(0, 0);

        obstacleMaskImage = new Image();

        float trackScaleFactor = 1.6;

        Image *tempImage = new Image();
        if (tempImage->loadFromFile("assets/obstacles.png"))
        {
            tempImage->createMaskFromColor(Color(188, 0, 188));
        }

        Vector2u originalSize = tempImage->getSize();
        Vector2u newSize(originalSize.x * trackScaleFactor, originalSize.y * trackScaleFactor);

        obstacleMaskImage->create(newSize.x, newSize.y, Color::Transparent);

        for (unsigned int y = 0; y < newSize.y; y++)
        {
            for (unsigned int x = 0; x < newSize.x; x++)
            {
                unsigned int origX = x * originalSize.x / newSize.x;
                unsigned int origY = y * originalSize.y / newSize.y;
                obstacleMaskImage->setPixel(x, y, tempImage->getPixel(origX, origY));
            }
        }

        obstacleMaskTexture = new Texture();
        obstacleMaskTexture->loadFromImage(*obstacleMaskImage);
        obstacleMaskSprite = Sprite(*obstacleMaskTexture);
        obstacleMaskSprite.setPosition(0, 0);

        carTexture = loadTexture("assets/cars.png", Color(188, 0, 188));
        carTexture->setSmooth(1);

        mapTexture = loadTexture("assets/track1.png", Color(188, 0, 188));
        mapSprite = Sprite(*mapTexture);
        mapSprite.setPosition(0, 0);
        mapSprite.scale(trackScaleFactor, trackScaleFactor);
        mapTexture->setSmooth(1);

        maxSpeed = 10.0;
        turnSpeed = 0.07;
        accelFactor = 0.3;
        decelFactor = 0.2;
        
        isDebug = 0;

        int soundVol = 30;

        if (engineSoundBuffer.loadFromFile("assets/engine.wav"))
        {
            engineSound.setBuffer(engineSoundBuffer);
            engineSound.setLoop(true);
            engineSound.setVolume(soundVol);

            engineSound2.setBuffer(engineSoundBuffer);
            engineSound2.setLoop(true);
            engineSound2.setVolume(soundVol);
        }
    }

    bool isRuning()
    {
        return window.isOpen();
    }

    float getNewSpeed(float speed, int up, int down)
    {
        if (up && speed < maxSpeed)
        {
            if (speed < 0)
            {
                speed += decelFactor;
            }

            else
            {
                speed += accelFactor;
            }
        }

        if (down && speed > -maxSpeed)
        {
            if (speed > 0)
            {
                speed -= decelFactor;
            }
            else
            {
                speed -= accelFactor;
            }
        }

        if (!up && !down)
        {
            if (speed - decelFactor > 0)
            {
                speed -= decelFactor;
            }
            else if (speed + decelFactor < 0)
            {
                speed += decelFactor;
            }
            else
            {
                speed = 0;
            }
        }

        return speed;
    }

    float getNewAngle(int right, int left, float speed, float angle)
    {
        if (right && speed != 0)
        {
            angle += turnSpeed * speed / maxSpeed;
        }
        else if (left && speed != 0)
        {
            angle -= turnSpeed * speed / maxSpeed;
        }
        return angle;
    }

    void Update()
    {
        player1->speed = getNewSpeed(player1->speed, playersInput.keyUp, playersInput.keyDown);
        player1->angle = getNewAngle(playersInput.keyRight, playersInput.keyLeft, player1->speed, player1->angle);
        if (isMoveBlocked(player1))
        {
            player1->speed = -3.5;
        }
        player1->move();

        player2->speed = getNewSpeed(player2->speed, playersInput.keyW, playersInput.keyS);
        player2->angle = getNewAngle(playersInput.keyD, playersInput.keyA, player2->speed, player2->angle);
        if (isMoveBlocked(player2))
        {
            player2->speed = -3.5;
        }
        player2->move();

        player1CenterPosition.x = player1->posX;
        player1CenterPosition.y = player1->posY;

        player2CenterPosition.x = player2->posX;
        player2CenterPosition.y = player2->posY;

        player1View.setCenter(player1CenterPosition);
        player2View.setCenter(player2CenterPosition);

        playEngineSound(player1, engineSound);
        playEngineSound(player2, engineSound2);
    }

    void Render()
    {
        window.clear(Color(0, 0, 0));

        // player 1 screen view

        window.setView(player1View);

        window.draw(mapSprite);

        if (isDebug)
        {
            window.draw(obstacleMaskSprite);
        }

        renderPlayer(player1, Color(60, 188, 252));
        renderPlayer(player2, Color(216, 40, 0));

        // player 2 screen view

        window.setView(player2View);

        window.draw(mapSprite);

        renderPlayer(player1, Color(60, 188, 252));
        renderPlayer(player2, Color(216, 40, 0));

        window.display();
    }

    void processEvents()
    {
        Event e;
        while (window.pollEvent(e))
        {
            switch (e.type)
            {
            case Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                switch (e.key.code)
                {
                case sf::Keyboard::Left:
                    playersInput.keyLeft = 1;
                    break;
                case sf::Keyboard::Right:
                    playersInput.keyRight = 1;
                    break;
                case sf::Keyboard::Up:
                    playersInput.keyUp = 1;
                    break;
                case sf::Keyboard::Down:
                    playersInput.keyDown = 1;
                    break;
                case sf::Keyboard::A:
                    playersInput.keyA = 1;
                    break;
                case sf::Keyboard::D:
                    playersInput.keyD = 1;
                    break;
                case sf::Keyboard::W:
                    playersInput.keyW = 1;
                    break;
                case sf::Keyboard::S:
                    playersInput.keyS = 1;
                    break;
                default:
                    break;
                }
                break;

            case sf::Event::KeyReleased:
                switch (e.key.code)
                {
                case sf::Keyboard::Left:
                    playersInput.keyLeft = 0;
                    break;
                case sf::Keyboard::Right:
                    playersInput.keyRight = 0;
                    break;
                case sf::Keyboard::Up:
                    playersInput.keyUp = 0;
                    break;
                case sf::Keyboard::Down:
                    playersInput.keyDown = 0;
                    break;
                case sf::Keyboard::A:
                    playersInput.keyA = 0;
                    break;
                case sf::Keyboard::D:
                    playersInput.keyD = 0;
                    break;
                case sf::Keyboard::W:
                    playersInput.keyW = 0;
                    break;
                case sf::Keyboard::S:
                    playersInput.keyS = 0;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
    }

    void InitGame()
    {
        player1 = new Car(1);
        player1->posX = 1210;
        player1->posY = 2840;

        player2 = new Car(2);
        player2->posX = 1250;
        player2->posY = 2840;

        player1View.setViewport(FloatRect(0, 0, 0.5f, 1.0f));
        player1View.setSize((int)(SCREEN_WIDTH / 2), SCREEN_HEIGHT);

        player2View.setViewport(FloatRect(0.5f, 0, 0.5f, 1.0f));
        player2View.setSize((int)(SCREEN_WIDTH / 2), SCREEN_HEIGHT);
    }

    void run()
    {
        InitGame();
        while (isRuning())
        {
            processEvents();
            Update();
            Render();
        }
    }

    ~SplitRacers()
    {
        delete player1;
        delete player2;

        delete carTexture;
        delete mapTexture;

        delete obstacleMaskImage;
    }

    void renderPlayer(Car *player, Color color)
    {
        carSprite = Sprite(*carTexture);
        frameSelection = IntRect(3, 3, 20, 22);
        carSprite.setTextureRect(frameSelection);
        carSprite.setOrigin(10, 11);
        carSprite.setPosition(player->posX, player->posY);
        carSprite.setRotation(player->angle * 180 / 3.141593);
        carSprite.setColor(color);
        window.draw(carSprite);

        if (isDebug)
        {
            RectangleShape shape(Vector2f(20, 22));
            shape.setOrigin(10, 11);
            shape.setPosition(player->posX, player->posY);
            shape.setRotation(player->angle * 180 / 3.141593);
            shape.setFillColor(Color::Red);
            window.draw(shape);

            renderBumper(player);
        }
    }

    void playEngineSound(Car *player, Sound &engineSound)
    {
        float pitch = abs(player->speed) / maxSpeed;
        engineSound.setPitch(pitch);
        if (pitch > 0.1f && engineSound.getStatus() != sf::Sound::Playing)
        {
            engineSound.play();
        }
    }

    void renderBumper(Car *player)
    {
        float detectorRange = 10.0f; // obstacle detector range
        float detectorRangeX = player->posX + detectorRange * cos(player->angle - M_PI / 2);
        float detectorRangeY = player->posY + detectorRange * sin(player->angle - M_PI / 2);

        sf::VertexArray line(sf::Lines, 2);
        line[0].position = sf::Vector2f(player->posX, player->posY);
        line[1].position = sf::Vector2f(detectorRangeX, detectorRangeY);
        line[0].color = sf::Color::Green;
        line[1].color = sf::Color::Green;
        window.draw(line);

        // detectorRange perpendicular line calculation
        float m = (detectorRangeY - player->posY) / (detectorRangeX - player->posX);
        float m_perpendicular = -1.0f / m;

        // perpendicular line starting point (detectorRange end)
        float detectorStartX = detectorRangeX;
        float detectorStartY = detectorRangeY;

        // perpendicular line length
        float perpendicularLength = 10.0f;

        // perpendicular line left end
        float detectorEnd1X = detectorStartX - perpendicularLength / std::sqrt(1 + m_perpendicular * m_perpendicular);
        float detectorEnd1Y = detectorStartY + m_perpendicular * (detectorEnd1X - detectorStartX);

        sf::VertexArray line2(sf::Lines, 2);
        line2[0].position = sf::Vector2f(detectorStartX, detectorStartY);
        line2[1].position = sf::Vector2f(detectorEnd1X, detectorEnd1Y);
        line2[0].color = sf::Color::Green;
        line2[1].color = sf::Color::Green;
        window.draw(line2);

        // perpendicular line right end
        float detectorEnd2X = detectorStartX + perpendicularLength / std::sqrt(1 + m_perpendicular * m_perpendicular);
        float detectorEnd2Y = detectorStartY + m_perpendicular * (detectorEnd2X - detectorStartX);

        sf::VertexArray line3(sf::Lines, 2);
        line3[0].position = sf::Vector2f(detectorStartX, detectorStartY);
        line3[1].position = sf::Vector2f(detectorEnd2X, detectorEnd2Y);
        line3[0].color = sf::Color::Green;
        line3[1].color = sf::Color::Green;
        window.draw(line3);
    }

    bool isMoveBlocked(Car *player)
    {
        if (player->posX < 100 || player->posX > 4130 - 100) // scaled map width here!
        {
            return 1;
        }

        if (player->posY < 100 || player->posY > 4800 - 100) // scaled map height here!
        {
            return 1;
        }

        float detectorRange = 10.0f; // obstacle detector range
        float detectorRangeX = player->posX + detectorRange * cos(player->angle - M_PI / 2);
        float detectorRangeY = player->posY + detectorRange * sin(player->angle - M_PI / 2);

        if ((int)obstacleMaskImage->getPixel(detectorRangeX, detectorRangeY).r == 255)
        {
            return 1;
        }

        // detectorRange perpendicular line calculation
        float m = (detectorRangeY - player->posY) / (detectorRangeX - player->posX);
        float m_perpendicular = -1.0f / m;

        // perpendicular line starting point (detectorRange end)
        float detectorStartX = detectorRangeX;
        float detectorStartY = detectorRangeY;

        // perpendicular line length
        float perpendicularLength = 10.0f;

        // perpendicular line left end
        float detectorEnd1X = detectorStartX - perpendicularLength / std::sqrt(1 + m_perpendicular * m_perpendicular);
        float detectorEnd1Y = detectorStartY + m_perpendicular * (detectorEnd1X - detectorStartX);

        if ((int)obstacleMaskImage->getPixel(detectorEnd1X, detectorEnd1Y).r == 255)
        {
            return 1;
        }

        // perpendicular line right end
        float detectorEnd2X = detectorStartX + perpendicularLength / std::sqrt(1 + m_perpendicular * m_perpendicular);
        float detectorEnd2Y = detectorStartY + m_perpendicular * (detectorEnd2X - detectorStartX);

        if ((int)obstacleMaskImage->getPixel(detectorEnd2X, detectorEnd2Y).r == 255)
        {
            return 1;
        }

        return 0;
    }

    Texture *loadTexture(const char *filename, sf::Color transparencyColor = sf::Color::Transparent)
    {
        Texture *tex = new Texture();
        Image img;
        if (img.loadFromFile(filename))
        {
            img.createMaskFromColor(transparencyColor);
            tex->loadFromImage(img);
        }
        return tex;
    }
};

int main()
{
    SplitRacers game;
    game.run();
    return 0;
}