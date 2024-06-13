#include <SFML/Graphics.hpp>
#include <iostream>
#include <stack>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

const int MAP_WIDTH = 45;
const int MAP_HEIGHT = 20;
const int CELL_SIZE = 40;

const vector<Vector2i> directions = { // �̷θ� ���� ������ 2ĭ�� ����
    Vector2i(2, 0),
    Vector2i(-2, 0),
    Vector2i(0, 2),
    Vector2i(0, -2)
};
const vector<Vector2i> OneDirections = { // ť���� ���
    Vector2i(1, 0),
    Vector2i(-1, 0),
    Vector2i(0, 1),
    Vector2i(0, -1)
};

bool isValid(int x, int y, vector<vector<bool>>& visited) {
    return x > 0 && x < (MAP_WIDTH - 1) && y > 0 && y < (MAP_HEIGHT - 1) && !visited[x][y];
}

void generateMaze(int startX, int startY, int gameMap[], vector<RectangleShape>& displayRects) {
    vector<vector<bool>> visited(MAP_WIDTH, vector<bool>(MAP_HEIGHT, false));
    stack<Vector2i> stack;
    stack.push(Vector2i(startX, startY));
    visited[startX][startY] = true;
    gameMap[startX + startY * MAP_WIDTH] = 0;

    while (!stack.empty()) { // ������ Ȱ���� DFS ���
        Vector2i current = stack.top();
        stack.pop();

        vector<Vector2i> neighbors; 
        for (auto dir : directions) {
            int newX = current.x + dir.x;
            int newY = current.y + dir.y;

            if (isValid(newX, newY, visited)) {
                neighbors.push_back(Vector2i(newX, newY)); // �������� ���ο� neighbors�� �����Ǳ� ������ ���� ����� ����
            }
        }

        if (!neighbors.empty()) {
            stack.push(current); // stack�� Ǫ���Ͽ� ��Ʈ��ŷ ������� ���ƿ� �� ����

            Vector2i next = neighbors[rand() % neighbors.size()]; // ���� ĭ�� ������ �� �� ���� 2ĭ�� �������� �Ͽ� next�� ���
            visited[next.x][next.y] = true;
            gameMap[next.x + next.y * MAP_WIDTH] = 0;

            int wallX = (current.x + next.x) / 2; //next�� current ������ x��ǥ
            int wallY = (current.y + next.y) / 2; //next�� current ������ y��ǥ
            gameMap[wallX + wallY * MAP_WIDTH] = 0; // next�� current ���̸� �վ� �̷θ� ������

            stack.push(next);
        }
    }

    gameMap[(MAP_WIDTH - 2) + (MAP_HEIGHT - 2) * MAP_WIDTH] = 0; // �ⱸ
    gameMap[0] = gameMap[1] = gameMap[MAP_WIDTH] = 0; // opponent�� ���� ���� �ʱ�ȭ
}

vector<Vector2i> findPath(Vector2i start, Vector2i goal, int gameMap[]) {
    queue<Vector2i> q;
    vector<vector<Vector2i>> prev(MAP_WIDTH, vector<Vector2i>(MAP_HEIGHT, Vector2i(-1, -1)));
    vector<vector<bool>> visited(MAP_WIDTH, vector<bool>(MAP_HEIGHT, false));

    q.push(start);
    visited[start.x][start.y] = true;

    while (!q.empty()) { // quene�� ���� bfs ���
        Vector2i current = q.front();
        q.pop();

        if (current == goal)
            break;

        for (auto dir : OneDirections) {
            int newX = current.x + dir.x;
            int newY = current.y + dir.y;

            if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && gameMap[newX + newY * MAP_WIDTH] == 0 && !visited[newX][newY]) {
                q.push(Vector2i(newX, newY));
                visited[newX][newY] = true;
                prev[newX][newY] = current;
            }
        }
    }
    vector<Vector2i> path;
    for (Vector2i at = goal; at != Vector2i(-1, -1); at = prev[at.x][at.y])
        path.push_back(at);
    reverse(path.begin(), path.end());
    return path;
}

void resetGame(Vector2i& player, Vector2i& opponent, RectangleShape& playerRect, RectangleShape& opponentRect, int gameMap[], vector<RectangleShape>& displayRects, vector<Vector2i>& path, size_t& pathIndex, Clock& clock, Clock& freeze, float& moveTimer) {
    player = Vector2i(1, 1);
    playerRect.setPosition(player.x * CELL_SIZE, player.y * CELL_SIZE);

    opponent = Vector2i(0, 0);
    opponentRect.setPosition(opponent.x * CELL_SIZE, opponent.y * CELL_SIZE);

    Vector2i goal(MAP_WIDTH - 2, MAP_HEIGHT - 2);
    path = findPath(opponent, goal, gameMap);
    pathIndex = 0;
    clock.restart();
    freeze.restart();
    moveTimer = 1.0f;
    
    // reset game������ ���� ���� ������ �ٽ� generateMaze ���� ����
}

int main() {
    RenderWindow window(VideoMode(1800, 800), "Outside The MAZE", Style::None); // �ʱ� ȭ��
    RenderWindow chase(VideoMode(0, 0), "Chase Window", Style::None); // opponent�� ���ߴ� ������
    window.setPosition(Vector2i(50, 100)); // �ϵ��ڵ� - �ػ󵵿� ���� �޶��� �� ����
    chase.setPosition(Vector2i(50, 100));

    Font font;
    if (!font.loadFromFile("Comic Sans MS.ttf")) {
        cout << "Error loading font" << endl;
        return -1;
    }

    Text message("", font, 30);
    message.setFillColor(Color::Red);
    message.setPosition(800, 400);

    Vector2i player(1, 1); 
    RectangleShape playerRect(Vector2f(CELL_SIZE, CELL_SIZE));
    playerRect.setPosition(player.x * CELL_SIZE, player.y * CELL_SIZE);
    playerRect.setFillColor(Color(153, 255, 255)); //sprite�� ����ϴ� ��� �ܻ����� ä��
    playerRect.setOutlineColor(Color(0, 0, 0));

    Vector2i opponent(0, 0);
    RectangleShape opponentRect(Vector2f(CELL_SIZE, CELL_SIZE));
    opponentRect.setPosition(opponent.x * CELL_SIZE, opponent.y * CELL_SIZE);
    opponentRect.setFillColor(Color(255, 153, 0)); //sprite�� ����ϴ� ��� �ܻ����� ä��
    opponentRect.setOutlineColor(Color(0, 0, 0));

    int gameMap[MAP_WIDTH * MAP_HEIGHT] = { 0 };
    fill_n(gameMap, MAP_WIDTH * MAP_HEIGHT, 1); // generateMaze�� ���� �վ� �̷θ� �����ϱ� ������ ������ ���� �� ä����

    vector<RectangleShape> displayRects(MAP_WIDTH * MAP_HEIGHT);

    srand(static_cast<unsigned>(time(nullptr))); // generateMaze ���ο��� ���Ǿ� ������ ����Ű�� ���ο� �̷ΰ� ������

    generateMaze(1, 1, gameMap, displayRects);

    for (int i = 0; i < MAP_WIDTH; ++i) {
        for (int j = 0; j < MAP_HEIGHT; ++j) {
            int index = i + j * MAP_WIDTH;
            displayRects[index].setPosition(i * CELL_SIZE, j * CELL_SIZE);
            displayRects[index].setSize(Vector2f(CELL_SIZE, CELL_SIZE));
            displayRects[index].setOutlineThickness(1.f);
            displayRects[index].setOutlineColor(Color(0, 0, 0));

            if (gameMap[index] == 1) {
                displayRects[index].setFillColor(Color(48, 48, 48));
            }
            else {
                displayRects[index].setFillColor(Color(204, 204, 204)); // �� ���� (���� �ƴ�)
            }
        }
    }
    displayRects[(MAP_WIDTH - 2) + (MAP_HEIGHT - 2) * MAP_WIDTH].setFillColor(Color(255, 255, 153)); //�ⱸ

    Vector2i goal(MAP_WIDTH - 2, MAP_HEIGHT - 2);
    vector<Vector2i> path = findPath(opponent, goal, gameMap);
    size_t pathIndex = 0;
    Clock clock;
    Clock freezeClock;
    float moveTimer = 1.0f;

    bool gameOver = false;
    bool gameWon = false;
    bool isFrozen = false;

    while (window.isOpen() && chase.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::KeyPressed) {
                if (gameOver || gameWon) {
                    if (event.key.code == Keyboard::Space) {
                        gameOver = false;
                        gameWon = false;
                        message.setString("");
                        resetGame(player, opponent, playerRect, opponentRect, gameMap, displayRects, path, pathIndex, clock, freezeClock, moveTimer);
                        for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
                            window.draw(displayRects[i]);

                            float viewCenterX = chase.getView().getCenter().x;
                            float viewCenterY = chase.getView().getCenter().y;
                            float viewSizeX = chase.getView().getSize().x;
                            float viewSizeY = chase.getView().getSize().y;

                            float rectPosX = displayRects[i].getPosition().x;
                            float rectPosY = displayRects[i].getPosition().y;

                            if (rectPosX >= viewCenterX - viewSizeX / 2 &&
                                rectPosX < viewCenterX + viewSizeX / 2 &&
                                rectPosY >= viewCenterY - viewSizeY / 2 &&
                                rectPosY < viewCenterY + viewSizeY / 2) {
                                chase.draw(displayRects[i]);
                            }
                        }
                    }
                    else if (event.key.code == Keyboard::X)
                    {
                        chase.close();
                        window.close(); // �ȳ�~
                    }
                }
                else if (event.key.code == Keyboard::LControl) // ���پ��
                {
                    moveTimer = 0.5f; // �ʸ� ������ �Ѱڴ�.

                    window.setPosition(Vector2i(50, 100));
                    window.setView(View(FloatRect(0, 0, 1800, 800))); // �ٽ� ��üȭ������ ��ȯ
                    window.setSize(Vector2u(1800, 800));

                    isFrozen = true;
                    playerRect.setFillColor(Color(50, 150, 255));  // �� �Ķ���
                    freezeClock.restart();
                }
                else if (isFrozen == false){
                    int newX = player.x;
                    int newY = player.y;

                    switch (event.key.code) {
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

                    if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && gameMap[newX + newY * MAP_WIDTH] != 1) {
                        player.x = newX;
                        player.y = newY;
                    }

                    playerRect.setPosition(player.x * CELL_SIZE, player.y * CELL_SIZE);


                    window.setView(View(FloatRect((player.x / 5) * 200, (player.y / 5) * 200, 200, 200))); // window �� 5*5���� ���̱� ����
                    window.setPosition(Vector2i(50 + (player.x / 5) * 200, 100 + (player.y / 5) * 200)); // ��ġ�� �°� ����
                    window.setSize(Vector2u(200, 200)); // ũ�� ����
                }
            }
        }

        if (freezeClock.getElapsedTime().asSeconds() >= 3.f) {
            // �ٽ� ����
            isFrozen = false;
            playerRect.setFillColor(Color(153, 255, 255));

            // �ð��� �ٽ� �帥��.           
            if (!gameOver && !gameWon)
            {
            window.setView(View(FloatRect((player.x / 5) * 200, (player.y / 5) * 200, 200, 200)));
            window.setPosition(Vector2i(50 + (player.x / 5) * 200, 100 + (player.y / 5) * 200));
            window.setSize(Vector2u(200, 200));

            }
            moveTimer = 1.0f;
        }

        if (!gameOver && !gameWon) {
            if (clock.getElapsedTime().asSeconds() >= moveTimer && pathIndex < path.size()) { // moveTimer�ʸ��� �̵�(moveTimer�� ���� ������� �� 0.5��, ���� ���� �� 1��)
                opponent = path[pathIndex];
                opponentRect.setPosition(opponent.x * CELL_SIZE, opponent.y * CELL_SIZE);
                pathIndex++;
                clock.restart();

                chase.setView(View(FloatRect((opponent.x / 5) * 200, (opponent.y / 5) * 200, 200, 200))); // window �� 5*5���� ���̱� ����
                chase.setPosition(Vector2i(50 + (opponent.x / 5) * 200, 100 + (opponent.y / 5) * 200));
                chase.setSize(Vector2u(200, 200)); 

                if (isFrozen == false) // ����ִ� ���¿����� ��üȭ���� �Ǳ� ������ ��ġ ���� x, Focus�� ã�� �ʿ䵵 x
                    window.setPosition(Vector2i(50 + (player.x / 5) * 200, 100 + (player.y / 5) * 200)); //Focus�� ��ã�� �� ����Ű�� ������ �� �ְ� �ϱ� ����
              
                    

            }

            if (player == opponent) {
                gameOver = true;
                window.setView(View(FloatRect(0, 0, 1800, 800))); // �ٽ� ��üȭ������ ��ȯ
                window.setPosition(Vector2i(50, 100));
                window.setSize(Vector2u(1800, 800));
                message.setString("Game Over! Press Space to restart\nPress X to Quit");
            }

            if (player == goal) {
                gameWon = true;
                window.setView(View(FloatRect(0, 0, 1800, 800))); // �ٽ� ��üȭ������ ��ȯ
                window.setPosition(Vector2i(50, 100));
                window.setSize(Vector2u(1800, 800));
                message.setString("Escaped! Press Space to restart\nPress X to Quit");
 
            }
        }



        window.clear(Color(200, 200, 200));
        chase.clear(Color(200, 200, 200));

        for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) { // displayRect �ϴ� ����ȭ �ڵ�    
            float viewCenterX = window.getView().getCenter().x;
            float viewCenterY = window.getView().getCenter().y;
            float viewSizeX = window.getView().getSize().x;
            float viewSizeY = window.getView().getSize().y;

            float rectPosX = displayRects[i].getPosition().x;
            float rectPosY = displayRects[i].getPosition().y;

            if (rectPosX >= viewCenterX - viewSizeX / 2 && // opponent �ֺ�(chasing window ����)�� �׸��� ����ȭ �ڵ�
                rectPosX < viewCenterX + viewSizeX / 2 &&
                rectPosY >= viewCenterY - viewSizeY / 2 &&
                rectPosY < viewCenterY + viewSizeY / 2) {
                window.draw(displayRects[i]);
            }

            


            viewCenterX = chase.getView().getCenter().x;
            viewCenterY = chase.getView().getCenter().y;
            viewSizeX = chase.getView().getSize().x;
            viewSizeY = chase.getView().getSize().y;

            rectPosX = displayRects[i].getPosition().x;
            rectPosY = displayRects[i].getPosition().y;

            if (rectPosX >= viewCenterX - viewSizeX / 2 && // opponent �ֺ�(chasing window ����)�� �׸��� ����ȭ �ڵ�
                rectPosX < viewCenterX + viewSizeX / 2 &&
                rectPosY >= viewCenterY - viewSizeY / 2 &&
                rectPosY < viewCenterY + viewSizeY / 2) {
                chase.draw(displayRects[i]);
            }
        }


        window.draw(playerRect);
        window.draw(opponentRect);
        window.draw(message);
        window.display();

        chase.draw(playerRect);
        chase.draw(opponentRect);
        chase.display();
    }

    return 0;
}
