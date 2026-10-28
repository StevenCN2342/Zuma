#include<zuma.h>
#include<cctype>
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

bool has_whitespace(const string& text)
{
    for (char ch : text)
    {
        if (std::isspace(static_cast<unsigned char>(ch)))
        {
            return true;
        }
    }
    return false;
}
}

Zuma Zuma::load_level(int level)
{
    if (level != 1 && level != 2)
    {
        throw std::invalid_argument("Unsupported level number");
    }

    string path = "config/level" + to_string(level) + ".txt";
    ifstream inFile(path);
    if (!inFile.is_open())
    {
        throw std::runtime_error("Cannot open level file: " + path);
    }

    vector<char> active;
    queue<char> feed;
    queue<char> ammo;
    int max_length = 0;
    bool has_active = false;
    bool has_feed = false;
    bool has_ammo = false;
    bool has_max_length = false;

    string line;
    int line_number = 0;

    while (getline(inFile, line))
    {
        ++line_number;
        string trimmed = trim(line);
        if (trimmed.empty())
        {
            continue;
        }

        size_t pos = trimmed.find('=');
        if (pos == string::npos)
        {
            throw std::runtime_error("Invalid level file line " + to_string(line_number) + ": missing '='");
        }

        string key = trim(trimmed.substr(0, pos));
        string value = trim(trimmed.substr(pos + 1));
        if (key.empty())
        {
            throw std::runtime_error("Invalid level file line " + to_string(line_number) + ": empty key");
        }

        if (key == "ACTIVE" || key == "FEED" || key == "AMMO")
        {
            if (has_whitespace(value))
            {
                throw std::runtime_error("Invalid " + key + " value on line " + to_string(line_number) + ": whitespace is not allowed");
            }
        }

        if (key == "ACTIVE")
        {
            has_active = true;
            for (char c : value)
            {
                active.push_back(c);
            }
        }
        else if (key == "FEED")
        {
            has_feed = true;
            for (char c : value)
            {
                feed.push(c);
            }
        }
        else if (key == "AMMO")
        {
            has_ammo = true;
            for (char c : value)
            {
                ammo.push(c);
            }
        }
        else if (key == "MAX_LENGTH")
        {
            has_max_length = true;
            size_t consumed = 0;
            try
            {
                max_length = std::stoi(value, &consumed);
            }
            catch (const std::exception&)
            {
                throw std::runtime_error("Invalid MAX_LENGTH value on line " + to_string(line_number) + ": " + value);
            }

            if (consumed != value.size() || max_length < 0)
            {
                throw std::runtime_error("Invalid MAX_LENGTH value on line " + to_string(line_number) + ": " + value);
            }
        }
    }

    if (inFile.bad())
    {
        throw std::runtime_error("Error while reading level file: " + path);
    }

    string missing;
    if (!has_active)
    {
        missing += "ACTIVE ";
    }
    if (!has_feed)
    {
        missing += "FEED ";
    }
    if (!has_ammo)
    {
        missing += "AMMO ";
    }
    if (!has_max_length)
    {
        missing += "MAX_LENGTH ";
    }

    if (!missing.empty())
    {
        throw std::runtime_error("Level file " + path + " is missing required field(s): " + missing);
    }

    if (ammo.empty())
    {
        throw std::runtime_error("Level file " + path + " has an empty AMMO field");
    }

    if (max_length < 0)
    {
        throw std::runtime_error("Level file " + path + " has an invalid MAX_LENGTH");
    }

    return Zuma(active, feed, ammo, max_length);
}

void Zuma::shoot(int pos)
{
    char fired_ammo = this->current_ammo;

    this->current_ammo = this->ammo.front();
    this->ammo.push(this->current_ammo);
    this->ammo.pop();

    this->active.insert(this->active.begin() + pos, fired_ammo);
}

void Zuma::quit()
{
    this->current_ammo = this->ammo.front();
    this->ammo.push(this->current_ammo);
    this->ammo.pop();
}

void Zuma::combo()
{
    while (true)
    {
        bool should_combo = false;
        int n = static_cast<int>(this->active.size());
        int start = -1;
        int end = -1;

        if (n < 3)
        {
            break;
        }

        int left_window = 0;
        int mid_window = 1;
        int right_window = 2;

        while (right_window < n)
        {
            if (this->active[left_window] == this->active[mid_window]
                && this->active[mid_window] == this->active[right_window])
            {
                should_combo = true;
                start = left_window;
                end = start;
                break;
            }

            left_window++;
            mid_window++;
            right_window++;
        }

        if (!should_combo)
        {
            break;
        }

        int index = start;
        while (index < n && this->active[start] == this->active[index])
        {
            end = index;
            index++;
        }

        this->active.erase(this->active.begin() + start, this->active.begin() + end + 1);

        this->points += pow(2, end + 1 - start);
    }
}

void Zuma::add_ball()
{
    if (this->feed.empty())
    {
        return;
    }

    char current_ball = this->feed.front();
    this->feed.pop();

    this->active.push_back(current_ball);
}

void Zuma::finish_round()
{
    this->round++;
}

void Zuma::print_zuma()
{
    cout << "===== Round " << this->round << " | Points " << this->points << " =====" << endl;

    cout << "Current active balls: ";
    for (auto& c : this->active)
    {
        cout << c;
    }
    cout << endl;

    cout << "Balls to appear: ";
    queue<char> temp = this->feed;
    while (!temp.empty())
    {
        char c = temp.front();
        cout << c;
        temp.pop();
    }
    cout << endl;

    cout << "Current ammo: ";
    cout << this->current_ammo << endl;

    cout << "Next ammo: ";
    if (!this->ammo.empty())
    {
        cout << this->ammo.front();
    }
    else
    {
        cout << '?';
    }
    cout << endl;
}

bool Zuma::is_victory()
{
    return this->active.empty() && this->feed.empty();
}

bool Zuma::is_defeat()
{
    return this->active.size() > static_cast<vector<char>::size_type>(this->max_length);
}
