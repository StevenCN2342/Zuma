#include<zuma.h>
using namespace std;

Zuma Zuma::load_level(int level)
{
    vector<char> active;
    queue<char> feed;
    queue<char> ammo;
    int max_length;

    string filename = "level" + to_string(level) + ".txt";
    string path = "config/" + filename;

    ifstream inFile(path);

    if (!inFile.is_open()) 
    {
        cout << "ERROR: Failed to open the file" << path << endl;
        return Zuma(active, feed, ammo, max_length);
    }

    string line;

    while (getline(inFile, line))
    {
        // 跳过空行
        if (line.empty()) continue;

        // 找到等号 '=' 的位置
        size_t pos = line.find('=');
        if (pos == string::npos) continue; // 没找到等号则跳过

        // 截取键（等号左边）和值（等号右边）
        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);

        // 5. 根据不同的键，将数据存入对应的容器
        if (key == "ACTIVE")
        {
            // 将字符串中的字符依次放入 vector
            for (char c : value) {
                active.push_back(c);
            }
        }
        else if (key == "FEED")
        {
            // 将字符串中的字符依次放入 queue
            for (char c : value) {
                feed.push(c);
            }
        }
        else if (key == "AMMO")
        {
            for (char c : value) {
                ammo.push(c);
            }
        }
        else if (key == "MAX_LENGTH")
        {
            // 将字符串转换为整数 (stoi 是 C++11 的标准库函数)
            max_length = stoi(value);
        }
    }

    inFile.close();

    return Zuma(active,feed,ammo,max_length);
}

void Zuma::shoot(int pos)
{
    char fired_ammo = this->current_ammo;

    this->current_ammo = this->ammo.front();
    ammo.push(this->current_ammo);
    ammo.pop();

    this->active.insert(this->active.begin() + pos, fired_ammo);
}

void Zuma::quit()
{
    this->current_ammo = this->ammo.front();
    ammo.push(this->current_ammo);
    ammo.pop();
}

void Zuma::combo()
{
    while(true)
    {
        bool should_combo = false;
        int n = this->active.size();
        int start = -1;
        int end = -1;

        if(n<3) break;

        int left_window = 0;
        int mid_window = 1;
        int right_window = 2;

        while(right_window < n)
        {
            if(this->active[left_window] == this->active[mid_window]
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

        if(should_combo == false) break;

        int index = start;
        while(index < n && this->active[start] == this->active[index])
        {
            end = index;
            index++;
        }

        this->active.erase(this->active.begin()+start,this->active.begin()+end+1);
    }
    return;
}

void Zuma::add_ball()
{
    if(this->feed.empty()) return;

    char current_ball = this->feed.front();
    this->feed.pop();

    this->active.push_back(current_ball);
}

void Zuma::print_zuma()
{
    cout << "Current active balls: ";
    for(auto &c : this->active)
    {
        cout << c;
    }
    cout << endl;

    cout << "Balls to appear: ";
    queue<char> temp = this->feed;
    while(!temp.empty())
    {
        char c = temp.front();
        cout << c;
        temp.pop();
    }
    cout << endl;

    cout << "Current ammo: ";
    cout << this->current_ammo << endl;

    cout << "Next ammo: ";
    cout << this->ammo.front() << endl;
}

bool Zuma::is_victory()
{
    if(this->active.empty() && this->feed.empty())
    {
        return true;
    }
    else return false;
}

bool Zuma::is_defeat()
{
    if(this->active.size() > this->max_length)
    {
        return true;
    }
    else return false;
}