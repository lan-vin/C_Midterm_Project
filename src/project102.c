//
// Created by asus on 2022/12/27.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>

#define spot_num 50
#define map_size 52
#define map_num 3
#define row 5
#define column 10
#define chance 13
char print_map[map_size][map_size];
char map[map_size][map_size] = {};
bool position_a[spot_num] = {}; // 地图位点是否放了卡牌
bool position_b[spot_num] = {};
bool position_c[spot_num] = {};
int a_size , b_size , c_size; // 地图上总放多少张卡牌
char a = 'a' , b = 'b' , c = 'c';
int cur_x , cur_y; // 位点图上的坐标
typedef struct Card_
{
    char card[10][5];
    int index; // 牌的类型
} Card;
Card *position[row][column][map_num] = {}; // 三维位点图，每个位点放结构体
Card *top[row][column] = {};               // 表示最上层的卡牌

int chosen_card[chance] = {}; // 卡槽
int check[13] = {};           // 用来给卡槽计数
int num = 0;                  // 记录卡槽现有多少张牌
int result = 3;
int total = 0;         // 记录总共地图上还剩多少张牌
int num_pool[96] = {}; // 卡牌数字从中选取
int pool_index = 0;
struct Copy
{ // 用来提供撤销的信息，备份最近选走的一张牌
    int x;
    int y;
    int z;
    Card *copy;
};
struct Copy reserve;

Card *new_card() // 初始化一张新卡牌
{
    Card *card = ( Card * ) malloc(sizeof(Card));
    card->index = 0;
    for ( int i = 0 ; i < 10 ; ++i )
    {
        for ( int j = 0 ; j < 5 ; ++j )
        {
            card->card[i][j] = '\0';
        }
    }
    return card;
}

void color( const unsigned short textColor ) // 自定义函根据参数改变颜色
{
    if ( textColor >= 0 && textColor <= 15 )                                   // 参数在0-15的范围颜色
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE) , textColor); // 用一个参数，改变字体颜色
    else                                                                     // 默认的字体颜色是黑色
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE) , 15);
}

HANDLE consoleHandle; // 操作控制台（也就是那个黑框框）需要的一个变量
int w , h;             // 高度，宽度，对应 y 和 x

void ScreenInit();

void Initialize();

void InitPrintMap();

void TopMap();

void PaintCard( Card * );

void PrintMap( int , int );

void GetInput();

void MarkCard( int , int );

void ChooseCard();

void Check();

void Rank();

int main()
{
Restart:
    system("cls");
    ScreenInit();
    printf("Let's play the game!\n");
    Sleep(2000); // 程序暂停 2000 毫秒
    system("cls");
    ScreenInit();
    Initialize();
    GetInput();
    Sleep(2000); // 程序暂停 2000 毫秒
    system("cls");
    ScreenInit();
    printf("press 'Enter' to restart a new game\n");
    getchar();
    goto Restart; // 我知道用goto不好，但是真的太香了www）
}

void Move() // 把卡槽里的牌放到地图任意位点
{
    int counter = 0;
    int temp_a = num / 3; // 大概把这些牌平均分到每层里
    int temp_b = (num - temp_a) / 2;
    int temp_c = num - temp_a - temp_b;
    for ( int i = 0 ; i < temp_a ; ++i )
    {
        Card *cur_card = new_card();
        bool find = false;
        for ( int j = 0 ; j < row ; ++j )
        {
            for ( int k = 0 ; k < column ; ++k )
            {
                if ( position[j][k][0] == NULL ) // 空位点
                {
                    position[j][k][0] = cur_card; // 放新的牌
                    for ( int l = 0 ; l < 5 ; ++l )
                    {
                        position[j][k][0]->card[0][l] = position[j][k][0]->card[9][l] = '-';
                    }
                    for ( int l = 1 ; l < 9 ; ++l )
                    {
                        position[j][k][0]->card[l][0] = position[j][k][0]->card[l][4] = '|';
                    }
                    position[j][k][0]->index = chosen_card[counter];
                    position[j][k][0]->card[5][2] = chosen_card[counter] + 48;
                    counter++;
                    find = true;
                    break;
                }
            }
            if ( find )
            {
                break;
            }
        }
    }
    for ( int i = 0 ; i < temp_b ; ++i )
    {
        Card *cur_card = new_card();
        bool find = false;
        for ( int j = 0 ; j < row ; ++j )
        {
            for ( int k = 0 ; k < column ; ++k )
            {
                if ( position[j][k][1] == NULL ) // 遍历地图找空位点
                {
                    position[j][k][1] = cur_card; // 放新的牌
                    for ( int l = 0 ; l < 5 ; ++l )
                    {
                        position[j][k][1]->card[0][l] = position[j][k][1]->card[9][l] = '-';
                    }
                    for ( int l = 1 ; l < 9 ; ++l )
                    {
                        position[j][k][1]->card[l][0] = position[j][k][1]->card[l][4] = '|';
                    }
                    position[j][k][1]->index = chosen_card[counter];
                    position[j][k][1]->card[5][2] = chosen_card[counter] + 48;
                    counter++;
                    find = true;
                    break;
                }
            }
            if ( find )
            {
                break;
            }
        }
    }
    for ( int i = 0 ; i < temp_c ; ++i )
    {
        Card *cur_card = new_card();
        bool find = false;
        for ( int j = 0 ; j < row ; ++j )
        {
            for ( int k = 0 ; k < column ; ++k )
            {
                if ( position[j][k][2] == NULL ) // 空位点
                {
                    position[j][k][2] = cur_card; // 放新的牌
                    for ( int l = 0 ; l < 5 ; ++l )
                    {
                        position[j][k][2]->card[0][l] = position[j][k][2]->card[9][l] = '-';
                    }
                    for ( int l = 1 ; l < 9 ; ++l )
                    {
                        position[j][k][2]->card[l][0] = position[j][k][2]->card[l][4] = '|';
                    }
                    position[j][k][2]->index = chosen_card[counter];
                    position[j][k][2]->card[5][2] = chosen_card[counter] + 48;
                    counter++;
                    find = true;
                    break;
                }
            }
            if ( find )
            {
                break;
            }
        }
    }
    for ( int i = 0 ; i < num ; i++ )
    {
        chosen_card[i] = 11; // 清空卡槽
    }
    total += num;
    num = 0;
    TopMap();
    cur_x = 0;
    cur_y = 0;
    while ( top[cur_x][cur_y] == NULL ) // 找到第一张顶层牌
    {
        if ( cur_y == column - 1 )
        {
            if ( cur_x == row - 1 )
            {
                cur_x = 0;
            }
            else
            {
                cur_x++;
            }
            cur_y = 0;
        }
        else
        {
            cur_y++;
        }
    }
    MarkCard(cur_x , cur_y); // 初始状态，把它标红
}

void DealCard() // 把当前地图上剩的牌放到任意位点
{
    int counter = 0;
    Card *temp[total];
    for ( int i = 0 ; i < total ; ++i )
    {
        temp[i] = malloc(sizeof(Card));
    }
    for ( int i = 0 ; i < 3 ; ++i )
    {
        for ( int j = 0 ; j < row ; ++j )
        {
            for ( int k = 0 ; k < column ; ++k )
            {
                if ( position[j][k][i] != NULL )
                {
                    temp[total - counter - 1] = position[j][k][i];
                    position[j][k][i] = NULL; // 找到并记录目前剩下的所有牌,清空地图
                    counter++;                // counter最多等于total-1
                }
            }
        }
    }
    counter = 0;
    for ( int i = 0 ; i < spot_num ; ++i )
    {
        position_a[i] = position_b[i] = position_c[i] = false; // 没放牌的状态
    }
    for ( int i = 0 ; i < a_size ; )
    {
        int k = rand() % spot_num;
        if ( !position_a[k] ) // 表示空位点
        {
            position[k / 10][k % 10][0] = temp[counter];
            counter++;
            i++;
            position_a[k] = true;
        }
        else
            continue;
    }
    for ( int i = 0 ; i < b_size ; )
    {
        int k = rand() % spot_num;
        if ( !position_b[k] ) // 表示空位点
        {
            position[k / 10][k % 10][1] = temp[counter];
            counter++;
            i++;
            position_b[k] = true;
        }
        else
            continue;
    }
    for ( int i = 0 ; i < c_size ; )
    {
        int k = rand() % spot_num;
        if ( !position_c[k] ) // 表示空位点
        {
            position[k / 10][k % 10][2] = temp[counter];
            counter++;
            i++;
            position_c[k] = true;
        }
        else
            continue;
    }
    system("cls");
    ScreenInit();
    TopMap();
    cur_x = 0;
    cur_y = 0;
    while ( top[cur_x][cur_y] == NULL ) // 找到第一张顶层牌
    {
        if ( cur_y == column - 1 )
        {
            if ( cur_x == row - 1 )
            {
                cur_x = 0;
            }
            else
            {
                cur_x++;
            }
            cur_y = 0;
        }
        else
        {
            cur_y++;
        }
    }
    MarkCard(cur_x , cur_y); // 初始状态，把它标红
}

void Cancel()
{
    position[reserve.x][reserve.y][reserve.z] = reserve.copy;
    cur_x = reserve.x;
    cur_y = reserve.y;
    for ( int i = 0 ; i < num ; ++i )
    {//找到卡槽里相同数字的牌并消除
        if ( chosen_card[i] == reserve.copy->index )
        {
            chosen_card[i] = 11;
            break;
        }
    }
    num--;
    total++;
    Rank();
    MarkCard(reserve.x , reserve.y);
}

void Pause()
{
    system("cls");
    printf("press 'Enter' to continue...\n");
    getchar();
    system("cls");
    ScreenInit();
    PrintMap(cur_x , cur_y);
}

void Reset()
{
    int reset[chance] = {};
    int temp = 0;
    for ( int i = 0 ; i < chance ; ++i )
    {
        reset[i] = 11; // 初始化临时数组
    }
    for ( int i = 0 ; i < chance ; ++i )
    {
        if ( chosen_card[i] != 11 )
        {
            reset[temp++] = chosen_card[i]; // 把有意义的卡牌全往前移，保证不间隔空
        }
    }
    for ( int i = 0 ; i < chance ; ++i )
    {
        chosen_card[i] = reset[i]; // 再把临时数组复制给卡槽
    }
}

void Rank()
{ // 把已选的牌从小到大排序
    for ( int i = 0 ; i < num ; ++i )
    {
        int min = chosen_card[i];
        for ( int j = i ; j < num ; ++j )
        {
            if ( min > chosen_card[j] )
            {
                int temp = min;
                min = chosen_card[j];
                chosen_card[j] = temp;
            }
        }
        chosen_card[i] = min;
    }
}

void Check()
{
    for ( int i = 0 ; i < 13 ; ++i )
    {
        check[i] = 0;
    }
    for ( int i = 0 ; i < chance ; ++i )
    {
        check[chosen_card[i]]++; // 遍历卡槽，记录相同卡牌的数量
    }
    for ( int i = 0 ; i < 11 ; ++i ) // 遍历计数的数组,但是要排除空白项
    {
        if ( check[i] == 3 ) // 一旦等于3张
        {
            int temp = i;
            for ( int j = 0 ; j < chance ; ++j ) // 找到这三张
            {
                if ( chosen_card[j] == temp ) // 清空这三张,并重置卡槽
                {
                    chosen_card[j] = 11;
                }
            }
            Reset();
            num -= 3;
            total--;
            return;
        }
    }
    total--;
}

void ChooseCard()
{
    for ( int i = 0 ; i < 3 ; ++i ) // 从上往下找
    {
        if ( position[cur_x][cur_y][i] != NULL )
        {
            chosen_card[num++] = position[cur_x][cur_y][i]->index;
            Check();
            Rank();
            if ( total == 0 )
            {
                printf("You win!\n");
                Sleep(2000); // 程序暂停 2000 毫秒
                result = 0;
                return;
            }
            else if ( num == chance )
            {
                printf("You lose!\n");
                Sleep(2000); // 程序暂停 2000 毫秒
                result = 1;
                return;
            }
            else // 继续游戏
            {
                result = 2;
                reserve.copy = position[cur_x][cur_y][i]; // 备份被选中的牌
                reserve.x = cur_x;
                reserve.y = cur_y;
                reserve.z = i;
                position[cur_x][cur_y][i] = NULL; // 去掉被选中的牌
                switch ( i )                        // 更新当前三层地图分别还剩多少牌
                {
                    case 0:
                        a_size--;
                        break;
                    case 1:
                        b_size--;
                        break;
                    case 2:
                        c_size--;
                        break;
                    default:
                        break;
                }
                // 更新，重新标红一张牌
                TopMap();
                while ( top[cur_x][cur_y] == NULL ) // 找到第一张顶层牌
                {
                    if ( cur_y == column - 1 )
                    {
                        if ( cur_x == row - 1 ) // 到达右下角后转到左上角继续寻找
                        {
                            cur_x = 0;
                            cur_y = 0;
                            continue;
                        }
                        else
                        {
                            cur_x++;
                            cur_y = 0;
                            continue;
                        }
                    }
                    cur_y++;
                }
                MarkCard(cur_x , cur_y); // 初始状态，把它标红
                return;
            }
        }
    }
}

void MarkCard( int x , int y )
{
    system("cls");
    InitPrintMap();
    PrintMap(x , y);
}

void GetInput()
{
    cur_x = 0 , cur_y = 0;
    while ( top[cur_x][cur_y] == NULL ) // 找到第一张顶层牌
    {
        if ( cur_y == column - 1 )
        {
            if ( cur_x == row - 1 )
            {
                cur_x = 0;
            }
            else
            {
                cur_x++;
            }
            cur_y = 0;
        }
        else
        {
            cur_y++;
        }
    }
    MarkCard(cur_x , cur_y); // 初始状态，把它标红
    while ( 1 )
    {
        while ( kbhit() != 0 )
        {
            char input = getch();
            if ( input == 'A' )
            {
                if ( cur_y == 0 )
                {
                    cur_y = column - 1;
                }
                else
                    cur_y--;
                while ( top[cur_x][cur_y] == NULL )
                {
                    if ( cur_y == 0 )
                    {
                        cur_y = column - 1;
                        if ( cur_x == row - 1 )
                        {
                            cur_x = 0;
                            continue;
                        }
                        else
                        {
                            cur_x++;
                            continue;
                        }
                    }
                    cur_y--;
                }
                MarkCard(cur_x , cur_y);
            }
            else if ( input == 'D' )
            {
                if ( cur_y == column - 1 )
                {
                    cur_y = 0;
                }
                else
                {
                    cur_y++;
                }
                while ( top[cur_x][cur_y] == NULL )
                {
                    if ( cur_y == column - 1 )
                    {
                        cur_y = 0;
                        if ( cur_x == row - 1 )
                        {
                            cur_x = 0;
                            continue;
                        }
                        else
                        {
                            cur_x++;
                            continue;
                        }
                    }
                    cur_y++;
                }
                MarkCard(cur_x , cur_y);
            }
            else if ( input == 'W' )
            {
                if ( cur_x == 0 )
                {
                    cur_x = row - 1;
                }
                else
                    cur_x--;
                while ( top[cur_x][cur_y] == NULL )
                {
                    if ( cur_x == 0 )
                    {
                        cur_x = row - 1;
                        if ( cur_y == column - 1 )
                        {
                            cur_y = 0;
                            continue;
                        }
                        else
                        {
                            cur_y++;
                            continue;
                        }
                    }
                    cur_x--;
                }
                MarkCard(cur_x , cur_y);
            }
            else if ( input == 'S' )
            {
                if ( cur_x == row - 1 )
                {
                    cur_x = 0; // 到底端就重新返回第一行开始
                }
                else
                    cur_x++;
                while ( top[cur_x][cur_y] == NULL )
                {
                    if ( cur_x == row - 1 )
                    {
                        cur_x = 0;
                        if ( cur_y == column - 1 )
                        {
                            cur_y = 0;
                            continue; // 到最右侧就重新返回第一列开始
                        }
                        else
                        {
                            cur_y++;
                            continue;
                        }
                    }
                    cur_x++;
                }
                MarkCard(cur_x , cur_y);
            }
            else if ( input == 'G' )
            {
                ChooseCard(); // 选牌
            }
            else if ( input == 'P' )
            {
                Pause(); // 暂停
            }
            else if ( input == 'C' )
            {
                Cancel(); // 撤销
            }
            else if ( input == 'V' )
            {
                DealCard(); // 洗牌
            }
            else if ( input == 'B' )
            {
                Move(); // 移出卡槽中现有卡牌
            }
            else
                continue;
            Sleep(100); // 程序暂停 100 毫秒
            if ( result == 0 || result == 1 )
            {
                return;
            }
        }
    }
}

void ScreenInit()
{
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);  // 初始化这个操作器
    CONSOLE_SCREEN_BUFFER_INFO csbi;                  // 屏幕的信息
    GetConsoleScreenBufferInfo(consoleHandle , &csbi); // 获取屏幕信息
    w = csbi.dwSize.X;                                // 宽度
    h = csbi.dwSize.Y;                                // 得到高度
    // 游戏里面，如果一直有输入的光标，就有点不好看。我们可以让它不显示
    CONSOLE_CURSOR_INFO cci; // 光标信息
    cci.dwSize = 100;
    cci.bVisible = FALSE;                      // 不可见
    SetConsoleCursorInfo(consoleHandle , &cci); // 将光标特性应用到控制台
    // 到这里，闪烁的光标就消失了。
}

void InitPrintMap()
{
    for ( int i = 0 ; i < map_size ; ++i )
    {
        for ( int j = 0 ; j < map_size ; ++j )
        {
            print_map[i][j] = '\0';
        }
    }
    for ( int i = 0 ; i < map_size ; ++i ) // 边框
    {
        print_map[0][i] = '+';
        print_map[map_size - 1][i] = '+';
        print_map[i][0] = '|';
        print_map[i][map_size - 1] = '|';
    }
    for ( int i = 0 ; i < row ; ++i ) // 先打印最底层的地图
    {
        for ( int j = 0 ; j < column ; ++j )
        {
            if ( position[i][j][2] != NULL ) // c层这里有卡牌
            {
                for ( int k = 0 ; k < 10 ; ++k )
                {
                    for ( int l = 0 ; l < 5 ; ++l ) // 把位点的结构体里存的卡牌画到位点在地图上对应的位置上
                    {
                        print_map[i * 10 + 1 + k][j * 5 + 1 + l] = position[i][j][2]->card[k][l];
                    }
                }
            }
        }
    }
    for ( int i = 0 ; i < row ; ++i ) // 逐层向上覆盖
    {
        for ( int j = 0 ; j < column ; ++j )
        {
            if ( position[i][j][1] != NULL ) // b层这里有牌
            {
                if ( j == 0 && i != (row - 1) ) // 最左列且不包含左下角
                {
                    for ( int k = 0 ; k < 10 ; ++k )
                    {
                        for ( int l = 2 ; l < 5 ; ++l )
                        {
                            print_map[i * 10 + 5 + k][j * 5 - 1 +
                                                      l] = position[i][j][1]->card[k][l];
                        }
                    }
                }
                else if ( i == row - 1 ) // 最后一行
                {
                    if ( j == 0 ) // 左下角
                    {
                        for ( int k = 0 ; k < 6 ; ++k )
                        {
                            for ( int l = 2 ; l < 5 ; ++l )
                            {
                                print_map[i * 10 + 5 + k][j * 5 - 1 +
                                                          l] = position[i][j][1]->card[k][l];
                            }
                        }
                    }
                    else
                    {
                        for ( int k = 0 ; k < 6 ; ++k )
                        {
                            for ( int l = 0 ; l < 5 ; ++l )
                            {
                                print_map[i * 10 + 5 + k][j * 5 - 1 +
                                                          l] = position[i][j][1]->card[k][l];
                            }
                        }
                    }
                }
                else
                {
                    for ( int k = 0 ; k < 10 ; ++k )
                    {
                        for ( int l = 0 ; l < 5 ; ++l )
                        {
                            print_map[i * 10 + 5 + k][j * 5 - 1 +
                                                      l] = position[i][j][1]->card[k][l];
                        }
                    }
                }
            }
        }
    }
    for ( int i = 0 ; i < row ; ++i )
    {
        for ( int j = 0 ; j < column ; ++j )
        {
            if ( position[i][j][0] != NULL ) // a层这里有卡牌
            {
                for ( int k = 0 ; k < 10 ; ++k )
                {
                    for ( int l = 0 ; l < 5 ; ++l )
                    {
                        print_map[i * 10 + 1 + k][j * 5 + 1 + l] = position[i][j][0]->card[k][l];
                    }
                }
            }
        }
    }
}

void TopMap() // 从上往下标记地图中位于顶层的位置
{
    for ( int i = 0 ; i < row ; ++i )
    {
        for ( int j = 0 ; j < column ; ++j )
        {
            top[i][j] = NULL;
        }
    }
    for ( int i = 0 ; i < map_size ; ++i )
    {
        for ( int j = 0 ; j < map_size ; ++j )
        {
            map[i][j] = 'f';
        }
    }
    for ( int i = 0 ; i < row ; ++i )
    {
        for ( int j = 0 ; j < column ; ++j )
        {
            if ( position[i][j][0] != NULL ) // a一定在顶层
            {
                top[i][j] = position[i][j][0];
                for ( int k = 0 ; k < 10 ; ++k )
                {
                    for ( int l = 0 ; l < 5 ; ++l )
                    {
                        map[i * 10 + k + 1][j * 5 + l + 1] = a;
                    }
                }
            }
            else if ( position[i][j][1] != NULL ) // b层
            {
                if ( j == 0 ) // 最左列
                {
                    if ( i == row - 1 ) // 左下角
                    {
                        top[i][j] = position[i][j][1];
                        for ( int k = 0 ; k < 6 ; ++k )
                        {
                            for ( int l = 2 ; l < 5 ; ++l )
                            {
                                map[i * 10 + 4 + k + 1][j * 5 - 2 + l + 1] = b;
                            }
                        }
                    }
                    else if ( position[i + 1][j][0] == NULL )
                    {
                        top[i][j] = position[i][j][1];
                        for ( int k = 0 ; k < 10 ; ++k )
                        {
                            for ( int l = 2 ; l < 5 ; ++l )
                            {
                                map[i * 10 + 4 + k + 1][j * 5 - 2 + l + 1] = b;
                            }
                        }
                    }
                }
                else if ( i == row - 1 ) // 最下一行
                {
                    if ( position[i][j - 1][0] == NULL )
                    {
                        top[i][j] = position[i][j][1];
                        for ( int k = 0 ; k < 6 ; ++k )
                        {
                            for ( int l = 0 ; l < 5 ; ++l )
                            {
                                map[i * 10 + 4 + k + 1][j * 5 - 2 + l + 1] = b;
                            }
                        }
                    }
                }
                else
                {
                    if ( position[i + 1][j - 1][0] == NULL && position[i][j - 1][0] == NULL &&
                         position[i + 1][j][0] == NULL )
                    {
                        top[i][j] = position[i][j][1];
                        for ( int k = 0 ; k < 10 ; ++k )
                        {
                            for ( int l = 0 ; l < 5 ; ++l )
                            {
                                map[i * 10 + 4 + k + 1][j * 5 - 2 + l + 1] = b;
                            }
                        }
                    }
                }
            }
            else if ( position[i][j][2] != NULL ) // c层
            {
                if ( i == 0 ) // 第一行
                {
                    if ( j == column - 1 ) // 右上角
                    {
                        top[i][j] = position[i][j][2];
                        for ( int k = 0 ; k < 10 ; ++k )
                        {
                            for ( int l = 0 ; l < 5 ; ++l )
                            {
                                map[i * 10 + k + 1][j * 5 + l + 1] = c;
                            }
                        }
                    }
                    else if ( position[i][j + 1][1] == NULL )
                    {
                        top[i][j] = position[i][j][2];
                        for ( int k = 0 ; k < 10 ; ++k )
                        {
                            for ( int l = 0 ; l < 5 ; ++l )
                            {
                                map[i * 10 + k + 1][j * 5 + l + 1] = c;
                            }
                        }
                    }
                }
                else if ( j == column - 1 ) // 最右列
                {
                    if ( position[i - 1][j][1] == NULL )
                    {
                        top[i][j] = position[i][j][2];
                        for ( int k = 0 ; k < 10 ; ++k )
                        {
                            for ( int l = 0 ; l < 5 ; ++l )
                            {
                                map[i * 10 + k + 1][j * 5 + l + 1] = c;
                            }
                        }
                    }
                }
                else
                {
                    if ( position[i][j + 1][1] == NULL && position[i - 1][j + 1][1] == NULL &&
                         position[i - 1][j][1] == NULL )
                    {
                        top[i][j] = position[i][j][2];
                        for ( int k = 0 ; k < 10 ; ++k )
                        {
                            for ( int l = 0 ; l < 5 ; ++l )
                            {
                                map[i * 10 + k + 1][j * 5 + l + 1] = c;
                            }
                        }
                    }
                }
            }
        }
    }
}

void PrintRule( int x )
{
    switch ( x )
    {
        case 0:
            printf("use 'WASD' to choose different cards");
            break;
        case 1:
            printf("use 'G' to remove this card; use 'C' to cancel your last choice");
            break;
        case 2:
            printf("use 'P' to pause the game");
            break;
        case 3:
            printf("use 'V' to rearrange the cards");
            break;
        case 4:
            printf("use 'B' to move the cards out from the list");
            break;
        default:
            break;
    }
}

void PrintMap( int temp_x , int temp_y )
{
    TopMap();
    for ( int i = 0 ; i < map_size ; ++i ) // 打印地图
    {
        for ( int j = 0 ; j < map_size ; ++j )
        {
            if ( print_map[i][j] != '\0' ) // 非空格，需要更换颜色
            {
                // int k = ((i - 1) / 10) * 10 + (j - 1) / 5;//从坐标对应回位点编号
                if ( i == 0 || i == map_size - 1 || j == 0 || j == map_size - 1 ) // 边框
                {
                    color(6);
                }
                else if ( map[i][j] != 'f' ) // 说明这时打印的是最上层的牌
                {
                    if ( ((i - 1) / 10 == temp_x && (j - 1) / 5 == temp_y && (map[i][j] == a || map[i][j] == c)) ||
                         ((i - 5) / 10 == temp_x && (j + 1) / 5 == temp_y &&
                          map[i][j] == b) ) // 表示现在打印的这个坐标在我们所选标红卡牌范围内)
                    {
                        color(4); // 被选中的为红色
                    }
                    else
                    {
                        color(15); // 最上层为黑色
                    }
                }
                else
                {
                    color(14); // 下层为浅黄色，表示遮挡关系
                }
                printf("%c" , print_map[i][j]);
                if ( i % 10 == 0 && j == map_size - 1 ) // 打印游戏规则
                {
                    PrintRule(i / 10);
                }
            }
            else
                printf(" ");
        }
        printf("\n");
    }
    printf("\n");
    for ( int i = 0 ; i < map_size ; ++i )
    {
        for ( int j = 0 ; j < map_size ; ++j )
        {
            map[i][j] = 'f';
        }
    }
    // 打印卡槽
    for ( int i = 0 ; i < chance ; ++i )
    {
        if ( chosen_card[i] == 11 )
        {
            printf("_  ");
        }
        else
        {
            printf("%d  " , chosen_card[i]);
        }
    }
    printf("\n");
    // printf("%d %d %d", total, num, result);
}

void PaintCard( Card *cur )
{
    for ( int i = 0 ; i < 5 ; ++i )
    {
        cur->card[0][i] = cur->card[9][i] = '-';
    }
    for ( int j = 1 ; j < 9 ; ++j )
    {
        cur->card[j][0] = cur->card[j][4] = '|';
    }
    cur->card[5][2] = num_pool[pool_index] + 48;
    cur->index = num_pool[pool_index];
}

void Initialize()
{ // 初始化变量
    num = pool_index = 0;
    total = 0;
    result = 3;
    reserve.z = reserve.y = reserve.x = 0;
    reserve.copy = malloc(sizeof(Card));
    for ( int k = 0 ; k < map_num ; ++k )
    {
        for ( int i = 0 ; i < row ; ++i )
        {
            for ( int j = 0 ; j < column ; ++j )
            {
                position[i][j][k] = NULL;
                top[i][j] = NULL;
            }
        }
    }
    for ( int i = 0 ; i < map_size ; ++i )
    {
        for ( int j = 0 ; j < map_size ; ++j )
        {
            map[i][j] = 'f';
        }
    }
    for ( int i = 0 ; i < chance ; ++i )
    {
        chosen_card[i] = 11;
    }
    for ( int i = 0 ; i < 12 ; ++i )
    {
        check[i] = 0;
    }
    // map_initialize
    for ( int i = 0 ; i < spot_num ; ++i )
    {
        position_a[i] = position_b[i] = position_c[i] = false; // 没放牌
    }
    for ( int i = 0 ; i < 96 ; ++i )
    {
        num_pool[i] = 0;
    }
    // 每张地图分别画几张卡牌(25-35)
    srand(( unsigned ) time(NULL));
    total = rand() % 21 + 75; // 75~95
    while ( total % 3 != 0 )
    {
        total = rand() % 21 + 75; // 保证总数是3的倍数
    }
    a_size = rand() % 6 + 25; // 25~30
    b_size = rand() % 6 + 30; // 30~35
    c_size = total - a_size - b_size;
    for ( int i = 0 ; i < total / 3 ; i++ )
    {
        int temp = rand() % 10;
        num_pool[i * 3] = num_pool[i * 3 + 1] = num_pool[i * 3 + 2] = temp;
    }
    // 在地图上随机位点生成size张卡牌
    int counter = 0;
    for ( ; counter < a_size ; )
    {
        Card *cur_card = new_card();
        int k = rand() % spot_num; // k表示位点数（第几个位点）
        if ( !position_a[k] )        // 表示空位点
        {
            position[k / 10][k % 10][0] = cur_card;
            PaintCard(cur_card);
            counter++;
            pool_index++;
            position_a[k] = true;
        }
        else
            continue;
    }
    counter = 0;
    for ( ; counter < b_size ; )
    {
        Card *cur_card = new_card();
        int k = rand() % spot_num; // k表示位点数（第几个位点）
        if ( !position_b[k] )
        {
            position[k / 10][k % 10][1] = cur_card;
            PaintCard(cur_card);
            counter++;
            pool_index++;
            position_b[k] = true;
        }
        else
            continue;
    }
    counter = 0;
    for ( ; counter < c_size ; )
    {
        Card *cur_card = new_card();
        int k = rand() % spot_num; // k表示位点数（第几个位点）
        if ( !position_c[k] )
        {
            position[k / 10][k % 10][2] = cur_card;
            PaintCard(cur_card);
            counter++;
            pool_index++;
            position_c[k] = true;
        }
        else
            continue;
    }
    InitPrintMap();
    PrintMap(row , column);
}