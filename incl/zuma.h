#ifndef ZUMA_H
#define ZUMA_H

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<queue>
#include<algorithm>
#include<iterator>
#include<stdexcept>
#include<cmath>
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
    int round;
    int points;

public:
    Zuma()
    {
        this->active = vector<char>();
        this->feed = queue<char>();
        this->ammo = queue<char>();
        this->max_length = 0;

        this->current_ammo = '\0';
        this->round = 0;
        this->points = 0;
    }

    Zuma(vector<char> active, queue<char> feed, queue<char> ammo, int max_length)
    {
        if (ammo.empty())
        {
            throw std::invalid_argument("AMMO must not be empty");
        }
        if (max_length < 0)
        {
            throw std::invalid_argument("MAX_LENGTH must be non-negative");
        }

        this->active = active;
        this->feed = feed;
        this->ammo = ammo;
        this->max_length = max_length;

        this->current_ammo = this->ammo.front();
        this->ammo.pop();
        this->ammo.push(this->current_ammo);

        this->round = 0;
        this->points = 0;
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
    int get_round()
    {
        return this->round;
    }
    int get_points()
    {
        return this->points;
    }

    static Zuma load_level(int level);

    void shoot(int pos);
    void quit();

    void combo();

    void add_ball();
    void finish_round();

    void print_zuma();
    bool is_victory();
    bool is_defeat();
};

#endif
