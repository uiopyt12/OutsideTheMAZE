#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

const int MAP_WIDTH = 45;
const int MAP_HEIGHT = 20;
const int CELL_SIZE = 40;

int main()
{
    int gameMap[MAP_WIDTH * MAP_HEIGHT] = { 0 };

    RectangleShape displayRects[MAP_WIDTH * MAP_HEIGHT];

    std::srand(static_cast<unsigned>(std::time(NULL))); // 시드 설정



    Vector2i player(1, 1);
    RectangleShape playerRect(Vector2f(CELL_SIZE, CELL_SIZE));
    playerRect.setPosition(player.x * CELL_SIZE, player.y * CELL_SIZE);
    playerRect.setFillColor(Color(0, 180, 10));
    playerRect.setOutlineThickness(1.f);
    playerRect.setOutlineColor(Color(0, 0, 0));




    for (int i = 0; i < MAP_WIDTH; ++i)
    {
        for (int j = 0; j < MAP_HEIGHT; ++j)
        {
            int index = i + j * MAP_WIDTH;
            displayRects[index].setPosition(i * CELL_SIZE, j * CELL_SIZE);
            displayRects[index].setSize(Vector2f(CELL_SIZE, CELL_SIZE));
            displayRects[index].setOutlineThickness(1.f);
            displayRects[index].setOutlineColor(Color(0, 0, 0));

            if (!(i == player.x && j == player.y))
            {
                if (static_cast<float>(std::rand()) / RAND_MAX < 0.f || i == 0 || j == 0 || i == MAP_WIDTH - 1 || j == MAP_HEIGHT - 1)
                {
                    gameMap[index] = 1;
                    displayRects[index].setFillColor(Color(0, 0, 0));
                }
                else
                {
                    displayRects[index].setFillColor(Color(200, 200, 200)); // 빈 셀 색상 설정
                }
            }
        }
    }





    RenderWindow window(VideoMode(200, 200), "Outside the MAZE", Style::Titlebar);
    window.setPosition(Vector2i(50, 100));

    //window.setView(View(FloatRect(0, 0, 200, 200)));








    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::KeyPressed)
            {
                int newX = player.x;
                int newY = player.y;

                switch (event.key.code)
                {
                case Keyboard::Up:
                    newY -= 1;
                    break;

                case Keyboard::Down:
                    newY += 1;
                    break;

                case Keyboard::Left:
                    newX -= 1;
                    break;

                case Keyboard::Right:
                    newX += 1;
                    break;
                }

                // 새 위치가 벽이 아닌지 확인
                if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && gameMap[newX + newY * MAP_WIDTH] != 1)
                {
                    player.x = newX;
                    player.y = newY;
                }



                playerRect.setPosition(player.x * CELL_SIZE, player.y * CELL_SIZE);
                printf("X좌표 : %d          , Y좌표 : %d \n", player.x, player.y);
                window.setView(View(FloatRect((player.x / 5) * 200, (player.y / 5) * 200, 200, 200))); // 한 화면 당 5개의 셀
                window.setPosition(Vector2i(50 + (player.x / 5) * 200, 100 + (player.y / 5) * 200));


            }
        }





        for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i)
        {
            window.draw(displayRects[i]);
        }
        window.draw(playerRect);
        window.display();


    }


    return 0;
}
