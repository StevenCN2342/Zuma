#include<iostream>
#include<fstream>
#include <sstream>
#include<string>
#include<vector>
#include<queue>
#include<algorithm>
#include<iterator>
using std::string;
using std::vector;
using std::queue;
using std::ifstream;
using std::ofstream;
using std::cout;
using std::endl;
using std::cin;

class Zuma
{
private:
    vector<char> active;
    queue<char> feed;
    queue<char> ammo;
    int max_length;
    char current_ammo;

public:
    Zuma()
    {
        this->active = vector<char>();
        this->feed = queue<char>();
        this->ammo = queue<char>();
        this->max_length = 0;
        this->current_ammo = '\0';
    }
    Zuma(vector<char> active, queue<char> feed, queue<char> ammo, int max_length)
    {
        this->active = active;
        this->feed = feed;
        this->ammo = ammo;
        this->max_length = max_length;

        this->current_ammo = ammo.front();
        ammo.push(this->current_ammo);

        ammo.pop();
    }

    vector<char> get_active()
    {
        return this->active;
    }
    queue<char> get_feed()
    {
        return this->feed;
    }
    queue<char> get_ammo()
    {
        return this->ammo;
    }
    int get_max_length()
    {
        return this->max_length;
    }
    char get_current_ammo()
    {
        return this->current_ammo;
    }

    Zuma load_level(int level);

    void shoot(int pos);
    void quit();

    void combo();

    void add_ball();

    void print_zuma();
    bool is_victory();
    bool is_defeat();
};