#include "raylib.h"
#include "zuma.h"

#include <algorithm>
#include <exception>
#include <stack>
#include <string>

namespace
{
Color BallColor(char ball)
{
    switch (ball)
    {
    case 'R': return RED;
    case 'G': return GREEN;
    case 'B': return BLUE;
    case 'Y': return YELLOW;
    case 'P': return PURPLE;
    default:  return GRAY;
    }
}

void DrawGame(Zuma& game, int level, int cursor,
              bool win, bool lose, const std::string& message)
{
    auto active = game.get_active();
    auto feed = game.get_feed();
    auto ammo = game.get_ammo();

    DrawText(TextFormat("Level %d | Round %d | Points %d",
                        level, game.get_round(), game.get_points()),
             40, 35, 28, DARKGRAY);

    const int count = static_cast<int>(active.size());

    DrawText(TextFormat("Active: %d / %d",
                        count, game.get_max_length()),
             40, 85, 24, DARKGRAY);

    // 随球数增加缩小间距，容纳第三关及超长终局。
    const float spacing =
        std::min(50.0f, 1100.0f / std::max(1, count));
    const float radius = std::min(18.0f, spacing * 0.4f);
    const float firstX = 640.0f - (count - 1) * spacing / 2.0f;

    for (int i = 0; i < count; ++i)
    {
        const int x = static_cast<int>(firstX + i * spacing);

        DrawCircle(x, 200, radius, BallColor(active[i]));
        DrawCircleLines(x, 200, radius, DARKGRAY);

        char label[2] = {active[i], '\0'};
        DrawText(label, x - 5, 191, 18,
                 active[i] == 'Y' ? BLACK : WHITE);
    }

    const int markerX = static_cast<int>(
        count == 0 ? 640.0f
                   : firstX + (cursor - 0.5f) * spacing);

    DrawLine(markerX, 145, markerX, 240, MAROON);
    DrawCircle(markerX, 140, 6, MAROON);

    DrawText(TextFormat("Insert position: %d (0..%d)", cursor, count),
             40, 275, 24, DARKGRAY);

    std::string waiting;
    while (!feed.empty())
    {
        waiting.push_back(feed.front());
        feed.pop();
    }

    DrawText(("Feed: " + waiting).c_str(),
             40, 325, 22, DARKGRAY);

    const char next = ammo.empty() ? '?' : ammo.front();
    DrawText(TextFormat("Current: %c | Next: %c",
                        game.get_current_ammo(), next),
             40, 375, 24, DARKGRAY);

    if (win || lose)
    {
        DrawText(win ? "YOU WIN!" : "NICE TRY.",
                 40, 465, 32, win ? DARKGREEN : MAROON);
        DrawText("R: undo the last round",
                 40, 510, 22, DARKGRAY);
    }

    DrawText("Left/Right: position | Space: shoot | Q: discard",
             40, 590, 22, DARKGRAY);
    DrawText("R: undo | Esc: exit",
             40, 625, 22, DARKGRAY);

    DrawText(message.c_str(), 40, 665, 20, MAROON);
}
}

int main()
{
    InitWindow(1280, 720, "Zuma");
    if (!IsWindowReady())
        return 1;

    SetTargetFPS(60);

    Zuma game;
    std::stack<Zuma> history;

    bool loaded = false;
    bool win = false;
    bool lose = false;
    int level = 0;
    int cursor = 0;
    std::string message;

    while (!WindowShouldClose())
    {
        if (!loaded)
        {
            int chosen = 0;
            if (IsKeyPressed(KEY_ONE)) chosen = 1;
            else if (IsKeyPressed(KEY_TWO)) chosen = 2;
            else if (IsKeyPressed(KEY_THREE)) chosen = 3;

            if (chosen != 0)
            {
                try
                {
                    game = Zuma::load_level(chosen);
                    history = std::stack<Zuma>();
                    level = chosen;
                    cursor = 0;
                    win = game.is_victory();
                    lose = game.is_defeat();
                    message.clear();
                    loaded = true;
                }
                catch (const std::exception& error)
                {
                    message = std::string("Load failed: ") + error.what();
                }
            }
        }
        else
        {
            const int count =
                static_cast<int>(game.get_active().size());

            if (IsKeyPressed(KEY_LEFT))
                cursor = std::max(0, cursor - 1);

            if (IsKeyPressed(KEY_RIGHT))
                cursor = std::min(count, cursor + 1);

            if (IsKeyPressed(KEY_R))
            {
                if (history.empty())
                {
                    message = "No previous round to undo.";
                }
                else
                {
                    game = history.top();
                    history.pop();

                    win = false;
                    lose = false;
                    cursor = std::min(
                        cursor,
                        static_cast<int>(game.get_active().size()));

                    message = "Undo complete.";
                }
            }
            else if (!win && !lose)
            {
                const bool shoot = IsKeyPressed(KEY_SPACE);
                const bool discard = IsKeyPressed(KEY_Q);

                if (shoot || discard)
                {
                    history.push(game);

                    if (shoot)
                    {
                        game.shoot(cursor);
                        game.combo();
                    }
                    else
                    {
                        game.quit();
                    }

                    game.add_ball();
                    game.finish_round();

                    win = game.is_victory();
                    lose = game.is_defeat();

                    cursor = std::min(
                        cursor,
                        static_cast<int>(game.get_active().size()));

                    message.clear();
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!loaded)
        {
            DrawText("ZUMA", 500, 160, 60, DARKGRAY);
            DrawText("Press 1, 2 or 3 to select a level",
                     350, 280, 28, DARKGRAY);
            DrawText(message.c_str(), 40, 400, 20, MAROON);
        }
        else
        {
            DrawGame(game, level, cursor, win, lose, message);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}