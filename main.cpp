#include<zuma.h>
#include<cctype>
#include<stack>
#include<stdexcept>
using namespace std;

namespace
{
string trim(const string& text)
{
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && std::isspace(static_cast<unsigned char>(text[begin])))
    {
        ++begin;
    }
    while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1])))
    {
        --end;
    }
    return text.substr(begin, end - begin);
}

bool parse_int(const string& text, int& value)
{
    string trimmed = trim(text);
    if (trimmed.empty())
    {
        return false;
    }

    size_t consumed = 0;
    try
    {
        value = std::stoi(trimmed, &consumed);
    }
    catch (const std::exception&)
    {
        return false;
    }

    return consumed == trimmed.size();
}

bool read_line(string& line)
{
    if (getline(cin, line))
    {
        return true;
    }
    return false;
}
}

int main()
{
    cout << "Welcome to the Zuma Game! " << endl;

    string line;
    int level = 0;
    while (true)
    {
        cout << "Please select a difficulty level (1, 2, or 3) : " << endl;
        if (!read_line(line))
        {
            if (cin.eof())
            {
                cout << "Input ended." << endl;
                return 0;
            }
            cerr << "Input error." << endl;
            return 1;
        }

        if (parse_int(line, level) && level >= 1 && level <= 3)
        {
            break;
        }

        cout << "Invalid level. Enter 1, 2, or 3." << endl;
    }

    Zuma zuma;
    try
    {
        zuma = Zuma::load_level(level);
    }
    catch (const exception& ex)
    {
        cerr << "ERROR: " << ex.what() << endl;
        return 1;
    }

    if (zuma.is_victory())
    {
        zuma.print_zuma();
        cout << "You win! " << endl;
        return 0;
    }

    if (zuma.is_defeat())
    {
        zuma.print_zuma();
        cout << "Nice try. " << endl;
        return 0;
    }

    bool win = false;
    bool lose = false;
    stack<Zuma> history;

    auto prompt_after_game = [&]() -> char
    {
        while (true)
        {
            cout << "Enter r to undo the last round, or press Enter to exit." << endl;
            if (!read_line(line))
            {
                if (cin.eof())
                {
                    cout << "Input ended." << endl;
                    return 'e';
                }
                cerr << "Input error." << endl;
                return 'x';
            }

            string trimmed = trim(line);
            if (trimmed == "r")
            {
                return 'r';
            }
            if (trimmed.empty())
            {
                return 'e';
            }

            cout << "Invalid input. Enter r to undo, or press Enter to exit." << endl;
        }
    };

    while (!win && !lose)
    {
        zuma.print_zuma();

        char option = '\0';
        while (true)
        {
            cout << "Shoot, quit, or undo? (s/q/r)" << endl;
            if (!read_line(line))
            {
                if (cin.eof())
                {
                    cout << "Input ended." << endl;
                    return 0;
                }
                cerr << "Input error." << endl;
                return 1;
            }

            string trimmed = trim(line);
            if (trimmed == "s" || trimmed == "q" || trimmed == "r")
            {
                option = trimmed[0];
                break;
            }

            cout << "Invalid option! Input again. " << endl;
        }

        if (option == 'r')
        {
            if (history.empty())
            {
                cout << "No previous round to undo." << endl;
                continue;
            }

            zuma = history.top();
            history.pop();
            win = false;
            lose = false;
            cout << "Undo complete." << endl;
            continue;
        }

        if (option == 's')
        {
            int pos = 0;
            while (true)
            {
                cout << "Select a target, input the number ahead of which you want to shoot: " << endl;
                if (!read_line(line))
                {
                    if (cin.eof())
                    {
                        cout << "Input ended." << endl;
                        return 0;
                    }
                    cerr << "Input error." << endl;
                    return 1;
                }

                int n = static_cast<int>(zuma.get_active().size());
                if (!parse_int(line, pos) || pos < 0 || pos > n)
                {
                    cout << "Invalid target position!" << endl;
                    continue;
                }
                break;
            }

            history.push(zuma);
            zuma.shoot(pos);
            zuma.combo();
            zuma.add_ball();
            zuma.finish_round();
        }
        else
        {
            history.push(zuma);
            zuma.quit();
            zuma.add_ball();
            zuma.finish_round();
        }

        win = zuma.is_victory();
        lose = zuma.is_defeat();

        if (win || lose)
        {
            zuma.print_zuma();
            cout << (win ? "You win! " : "Nice try. ") << endl;

            if (history.empty())
            {
                return 0;
            }

            char choice = prompt_after_game();
            if (choice == 'x')
            {
                return 1;
            }
            if (choice == 'e')
            {
                return 0;
            }

            zuma = history.top();
            history.pop();
            win = false;
            lose = false;
            cout << "Undo complete." << endl;
        }
    }

    return 0;
}
