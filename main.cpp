#include <iostream>

#include <SFML/Graphics.hpp>
using namespace sf;

class SplitRacers
{
public:
    SplitRacers()
    {
    }

    void run()
    {
        std::cout << "here we go!" << std::endl;
    }
};

int main()
{
    SplitRacers game;
    game.run();
    return 0;
}