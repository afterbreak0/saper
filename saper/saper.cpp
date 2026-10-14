#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

const int SIZE = 8;
const int MINES = 10;
const int CELL = 50;
const int OFFSET_X = 40;
const int OFFSET_Y = 60;

int board[SIZE][SIZE] = {};
bool opened[SIZE][SIZE] = {};
bool flagged[SIZE][SIZE] = {};

bool gameOver = false;
bool won = false;
int openedCount = 0;

int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

void createBoard()
{
    int count = 0;

    while (count < MINES)
    {
        int x = rand() % SIZE;
        int y = rand() % SIZE;

        if (board[x][y] != -1)
        {
            board[x][y] = -1;
            count++;
        }
    }

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == -1)
                continue;

            for (int k = 0; k < 8; k++)
            {
                int x = i + dx[k];
                int y = j + dy[k];

                if (x >= 0 && x < SIZE &&
                    y >= 0 && y < SIZE &&
                    board[x][y] == -1)
                {
                    board[i][j]++;
                }
            }
        }
    }
}

void openCell(int x, int y)
{
    if (x < 0 || x >= SIZE ||
        y < 0 || y >= SIZE)
        return;

    if (opened[x][y] || flagged[x][y])
        return;

    opened[x][y] = true;

    if (board[x][y] == -1)
    {
        gameOver = true;
        won = false;
        return;
    }

    openedCount++;

    // Автоматически открываем соседние клетки
    // Если вокруг нет мин
    if (board[x][y] == 0)
    {
        for (int k = 0; k < 8; k++)
        {
            openCell(x + dx[k], y + dy[k]);
        }
    }

    if (openedCount == SIZE * SIZE - MINES)
    {
        gameOver = true;
        won = true;
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    createBoard();

    sf::RenderWindow window(
        sf::VideoMode({ 480, 500 }),
        "Minesweeper"
    );

    window.setFramerateLimit(60);

    sf::Font font;

    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        cout << "Cannot load font!\n";
        cout << "Check C:/Windows/Fonts/arial.ttf\n";
        return 1;
    }

    sf::Text text(font, "", 24);
    sf::Text header(font, "", 20);

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* mouse =
                event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (gameOver)
                    continue;

                int mx = mouse->position.x;
                int my = mouse->position.y;

                int x = (my - OFFSET_Y) / CELL;
                int y = (mx - OFFSET_X) / CELL;

                if (mx < OFFSET_X || my < OFFSET_Y ||
                    x < 0 || x >= SIZE ||
                    y < 0 || y >= SIZE)
                    continue;

                if (mouse->button == sf::Mouse::Button::Left)
                {
                    openCell(x, y);
                }
                else if (mouse->button == sf::Mouse::Button::Right)
                {
                    if (!opened[x][y])
                        flagged[x][y] = !flagged[x][y];
                }
            }
        }

        window.clear(sf::Color(35, 35, 40));

        header.setCharacterSize(18);
        header.setFillColor(sf::Color::White);
        header.setPosition({ 10.f, 10.f });
        header.setString(
            "Mines: 10 | Left: open | Right: flag"
        );
        window.draw(header);

        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                sf::RectangleShape cell(
                    sf::Vector2f(
                        static_cast<float>(CELL - 2),
                        static_cast<float>(CELL - 2)
                    )
                );

                cell.setPosition({
                    static_cast<float>(OFFSET_X + j * CELL),
                    static_cast<float>(OFFSET_Y + i * CELL)
                    });

                if (opened[i][j])
                    cell.setFillColor(sf::Color(190, 190, 190));
                else
                    cell.setFillColor(sf::Color(90, 100, 115));

                window.draw(cell);

                string symbol = "";

                if (opened[i][j])
                {
                    if (board[i][j] == -1)
                        symbol = "*";
                    else if (board[i][j] > 0)
                        symbol = to_string(board[i][j]);
                }
                else if (flagged[i][j])
                {
                    symbol = "F";
                }

                if (gameOver && board[i][j] == -1)
                    symbol = "*";

                if (!symbol.empty())
                {
                    text.setString(symbol);
                    text.setCharacterSize(24);

                    if (symbol == "*")
                        text.setFillColor(sf::Color::Red);
                    else if (symbol == "F")
                        text.setFillColor(sf::Color::Yellow);
                    else
                        text.setFillColor(sf::Color::Blue);

                    text.setPosition({
                        static_cast<float>(OFFSET_X + j * CELL + 15),
                        static_cast<float>(OFFSET_Y + i * CELL + 8)
                        });

                    window.draw(text);
                }
            }
        }

        if (gameOver)
        {
            header.setCharacterSize(24);
            header.setPosition({ 150.f, 460.f });

            if (won)
            {
                header.setString("YOU WIN!");
                header.setFillColor(sf::Color::Green);
            }
            else
            {
                header.setString("GAME OVER!");
                header.setFillColor(sf::Color::Red);
            }

            window.draw(header);
        }

        window.display();
    }

    return 0;
}
