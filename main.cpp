#include<zuma.h>
#include<zuma.cpp>
using namespace std;

int main()
{
    cout << "Welcome to the Zuma Game! " << endl;

    cout << "Please select a difficulty level (1 or 2) : " << endl;

    int level;
    cin >> level;

    if(level != 1 && level != 2)
    {
        cout << "Invalid level! Restarted. " << endl;
        return;
    }

    Zuma zuma = zuma.load_level(level);

    bool win;
    bool lose;

    while(!win && !lose)
    {
        zuma.print_zuma();

        cout << "Shoot or quit? (s/q)" << endl;

        char option;
        cin >> option;

        if(option != 's' || option != 'q')
        {
            cout << "Invalid option! Input again. " << endl;
            break;
        }

        if(option == 's')
        {
            cout << "Select a target, input the number ahead of which you want to shoot: " << endl;

            int pos;
            cin >> pos;

            int n = zuma.get_active().size();
            // pos表示“在下标为pos的元素之前插入”，若pos = n，则代表在末尾插入，合法pos为 0 <= pos <= n

            if(pos<0||pos>n)
            {
                cout << "Invalid target position!" << endl;
                break;
            }

            zuma.shoot(pos);

            zuma.combo();

            zuma.add_ball();

            win = zuma.is_victory();
            lose = zuma.is_defeat();

            if(win)
            {
                cout << "You win! " << endl;
                break;
            }
            else if(lose)
            {
                cout << "Nice try. " << endl;
                break;
            }
        }
        else
        {
            zuma.quit();

            zuma.combo();

            zuma.add_ball();

            win = zuma.is_victory();
            lose = zuma.is_defeat();

            if(win)
            {
                cout << "You win! " << endl;
                break;
            }
            else if(lose)
            {
                cout << "Nice try. " << endl;
                break;
            }
        }
    }

    return 0;
}