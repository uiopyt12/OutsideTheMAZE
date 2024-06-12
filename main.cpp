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

// 방향 벡터
const vector<Vector2i> directions = { // 미로를 뚫는 데 사용
    Vector2i(2, 0),
    Vector2i(-2, 0),
    Vector2i(0, 2),
    Vector2i(0, -2)
};
const vector<Vector2i> OneDirections = { // 큐를 활용한 bfs에서 사용
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

    while (!stack.empty()) {
        Vector2i current = stack.top();
        stack.pop();

        vector<Vector2i> neighbors; // 각 반복마다 새로운 neighbors가 생성
        for (auto dir : directions) {
            int newX = current.x + dir.x;
            int newY = current.y + dir.y;

            if (isValid(newX, newY, visited)) {
                neighbors.push_back(Vector2i(newX, newY));
            }
        }

        if (!neighbors.empty()) { // neighbors가 비었다는 것은 해당 방향의 끝까지 탐색하여 미로를 만들었다는 것
            stack.push(current);

            Vector2i next = neighbors[rand() % neighbors.size()];
            visited[next.x][next.y] = true;
            gameMap[next.x + next.y * MAP_WIDTH] = 0;

            int wallX = (current.x + next.x) / 2;
            int wallY = (current.y + next.y) / 2;
            gameMap[wallX + wallY * MAP_WIDTH] = 0;

            stack.push(next);
        }
    }

    // 출구 추가
    gameMap[(MAP_WIDTH - 2) + (MAP_HEIGHT - 2) * MAP_WIDTH] = 0;
    gameMap[0] = gameMap[1] = gameMap[MAP_WIDTH] = 0;
}

vector<Vector2i> findPath(Vector2i start, Vector2i goal, int gameMap[]) {
    queue<Vector2i> q;
    vector<vector<Vector2i>> prev(MAP_WIDTH, vector<Vector2i>(MAP_HEIGHT, Vector2i(-1, -1)));
    vector<vector<bool>> visited(MAP_WIDTH, vector<bool>(MAP_HEIGHT, false));

    q.push(start);
    visited[start.x][start.y] = true;

    while (!q.empty()) {
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

int main() {
    RenderWindow window(VideoMode(1800, 800), "Outside The MAZE", Style::None);
    RenderWindow chase(VideoMode(200, 200), "Chase Window", Style::None);
    window.setPosition(Vector2i(50, 100));
    chase.setPosition(Vector2i(50, 100));

    Vector2i player(1, 1);
    RectangleShape playerRect(Vector2f(CELL_SIZE, CELL_SIZE));
    playerRect.setPosition(player.x * CELL_SIZE, player.y * CELL_SIZE);
    playerRect.setFillColor(Color(50, 200, 150));
    playerRect.setOutlineColor(Color(0, 0, 0));

    Vector2i opponent(1, 1);
    RectangleShape opponentRect(Vector2f(CELL_SIZE, CELL_SIZE));
    opponentRect.setPosition(opponent.x * CELL_SIZE, opponent.y * CELL_SIZE);
    opponentRect.setFillColor(Color(250, 200, 150));
    opponentRect.setOutlineColor(Color(0, 0, 0));

    int gameMap[MAP_WIDTH * MAP_HEIGHT] = { 0 };
    fill_n(gameMap, MAP_WIDTH * MAP_HEIGHT, 1); // 모든 셀을 벽(1)으로 초기화

    vector<RectangleShape> displayRects(MAP_WIDTH * MAP_HEIGHT);

    srand(static_cast<unsigned>(time(nullptr))); // 시드 설정

    // Generate maze
    generateMaze(1, 1, gameMap, displayRects);

    for (int i = 0; i < MAP_WIDTH; ++i) {
        for (int j = 0; j < MAP_HEIGHT; ++j) {
            int index = i + j * MAP_WIDTH;
            displayRects[index].setPosition(i * CELL_SIZE, j * CELL_SIZE);
            displayRects[index].setSize(Vector2f(CELL_SIZE, CELL_SIZE));
            displayRects[index].setOutlineThickness(1.f);
            displayRects[index].setOutlineColor(Color(0, 0, 0));

            if (gameMap[index] == 1) {
                displayRects[index].setFillColor(Color(0, 0, 0));
            }
            else {
                displayRects[index].setFillColor(Color(200, 200, 200)); // 빈 셀 색상 설정
            }
        }
    }

    Vector2i goal(MAP_WIDTH - 2, MAP_HEIGHT - 2);
    vector<Vector2i> path = findPath(opponent, goal, gameMap);
    size_t pathIndex = 0;
    Clock clock;

    while (window.isOpen() && chase.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                chase.close();
            }
            else if (event.type == Event::KeyPressed) {
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

                // 새 위치가 벽이 아닌지 확인
                if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && gameMap[newX + newY * MAP_WIDTH] != 1) {
                    player.x = newX;
                    player.y = newY;
                }

                playerRect.setPosition(player.x * CELL_SIZE, player.y * CELL_SIZE);

                window.setView(View(FloatRect((player.x / 5) * 200, (player.y / 5) * 200, 200, 200))); // 한 화면 당 5개의 셀
                window.setPosition(Vector2i(50 + (player.x / 5) * 200, 100 + (player.y / 5) * 200));
                window.setSize(Vector2u(200, 200));
            }
        }

        window.clear(Color(200, 200, 200));
        chase.clear(Color(200, 200, 200));

        // 0.1초마다 opponent 이동
        if (clock.getElapsedTime().asSeconds() >= 1.5f && pathIndex < path.size()) {
            opponent = path[pathIndex];
            opponentRect.setPosition(opponent.x * CELL_SIZE, opponent.y * CELL_SIZE);
            pathIndex++;
            clock.restart();

            chase.setView(View(FloatRect((opponent.x / 5) * 200, (opponent.y / 5) * 200, 200, 200))); // 한 화면 당 5개의 셀
            chase.setPosition(Vector2i(50 + (opponent.x / 5) * 200, 100 + (opponent.y / 5) * 200));
            chase.setSize(Vector2u(200, 200));

            window.setPosition(Vector2i(50 + (player.x / 5) * 200, 100 + (player.y / 5) * 200));
        }

        for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
            window.draw(displayRects[i]);

            // 뷰 중심과 크기를 가져옵니다.
            float viewCenterX = chase.getView().getCenter().x;
            float viewCenterY = chase.getView().getCenter().y;
            float viewSizeX = chase.getView().getSize().x;
            float viewSizeY = chase.getView().getSize().y;

            // 현재 Rect의 위치를 가져옵니다.
            float rectPosX = displayRects[i].getPosition().x;
            float rectPosY = displayRects[i].getPosition().y;

            // Rect가 뷰 안에 있는지 확인합니다.
            if (rectPosX >= viewCenterX - viewSizeX / 2 &&
                rectPosX < viewCenterX + viewSizeX / 2 &&
                rectPosY >= viewCenterY - viewSizeY / 2 &&
                rectPosY < viewCenterY + viewSizeY / 2) {
                chase.draw(displayRects[i]);
            }
        }


        window.draw(playerRect);
        window.draw(opponentRect);
        window.display();

        chase.draw(playerRect);
        chase.draw(opponentRect);
        chase.display();
    }

    return 0;
}
