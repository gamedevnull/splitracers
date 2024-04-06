#include <iostream>

#include <SFML/Graphics.hpp>
using namespace sf;

class Car
{
public:
    float posX, posY, speed, angle;

    Car()
    {
        speed = 2;
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

    SplitRacers() : window(VideoMode(800, 600), "Split Racers")
    {
        window.setFramerateLimit(60);
    }

    bool isRuning()
    {
        return window.isOpen();
    }

    void Update()
    {
    }

    void Render()
    {
        window.clear(Color(0, 0, 0));

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

            default:
                break;
            }
        }
    }

    void run()
    {
        while (isRuning())
        {
            processEvents();
            Update();
            Render();
        }
    }
};

int main()
{
    SplitRacers game;
    game.run();
    return 0;
}