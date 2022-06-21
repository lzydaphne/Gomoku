#include "play_AB.hpp"
#include "config.hpp"

#include <string.h>
#include <climits>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <list>
#include <array>
#define DEBUG 1

class State;
int player;
typedef std::array<int, SIZE> Row;
typedef std::array<Row, SIZE> Board; //Board
Board board;

const int RANGE[8][2] = {
    {1, 0},
    {-1, 0},
    {0, 1},
    {0, -1},
    {1, 1},
    {-1, 1},
    {1, -1},
    {-1, -1},
};

std::vector<Point> move_list;

const int weight[17] = {0, 100000, -1000000, 5000, -10000, 40, -10000, 40, -800, 20, -50, 20, -50, 1, -3, 1, -3};

void move_list_init()
{
  for (int i = 0; i < SIZE; ++i)
  {
    for (int j = 0; j < SIZE; ++j)
    {
      move_list.push_back(Point(i, j));
    }
  }
}

State::State(Board bd, int player) : board(bd), self_side(player)
{

  for (int i = 0; i < SIZE; i++)
  {
    for (int j = 0; j < SIZE; j++)
    {
      board[i][j] = bd[i][j];
      /*if (board[i][j] == player)
      {
        myCnt += 1;
      }*/
    }
  }

  this->init_tuple6type(player);
  this->get_possible_actions();
};

State::State(State &state)
{
  for (int i = 0; i < SIZE; i++)
    for (int j = 0; j < SIZE; j++)
      board[i][j] = state.board[i][j];
  self_side = state.self_side;
}

bool State::checkBound(int x, int y)
{
  if (x >= 0 && x < 15 && y >= 0 && y < 15)
    return true;
  else
    return false;
}
bool State::checkEmpty(int x, int y)
{
  if (board[x][y] == C_NONE)
    return true;
  else
    return false;
}

//Get legal actions around the existing discs
bool flag = true;

void State::get_possible_actions(void)
{

  std::vector<Point> actions;
  Board bd; //單張棋盤

  if (actions.empty() && flag)
  {
    actions.push_back(Point(SIZE / 2, SIZE / 2));
  }

  for (auto move : move_list)
  {

    if (board[move.x][move.y] != 0) //沒有空格了
    {
      std::cout << "move-list" << std::endl;
      flag = false;
      for (auto m : RANGE)
      {
        int x = move.x + m[0];
        int y = move.y + m[1];
        if (!checkBound(x, y) || !checkEmpty(x, y))
          continue;
        actions.push_back(Point(x, y));
        bd[x][y] = 1;
      }
    }
  }

  possible_actions = actions;
};

int State::tupleScoreGreedy(int black, int white, int C_ME)

{
  //连5
  if (C_ME == C_BLACK && black == 5)
    return 9999999;
  if (C_ME == C_WHITE && white == 5)
    return 9999999;
  //全空
  if (black == 0 && white == 0)
    return 7;
  //polluted
  else if (black >= 1 && white >= 1)
    return 0;
  else if (C_ME == C_BLACK)
  {
    if (black == 1 && white == 0)
      return 35;
    else if (black == 2 && white == 0)
      return 800;
    else if (black == 3 && white == 0)
      return 15000;
    else if (black == 4 && white == 0)
      return 800000;
    else if (black == 0 && white == 1)
      return 15;
    else if (black == 0 && white == 2)
      return 400;
    else if (black == 0 && white == 3)
      return 1800;
    else
      return 100000;
  }
  else
  {
    if (black == 1 && white == 0)
      return -15;
    else if (black == 2 && white == 0)
      return -400;
    else if (black == 3 && white == 0)
      return -1800;
    else if (black == 4 && white == 0)
      return -100000;
    else if (black == 0 && white == 1)
      return -35;
    else if (black == 0 && white == 2)
      return -800;
    else if (black == 0 && white == 3)
      return -15000;
    else
      return -800000;
  }
}
//用tupleScoreGreedy求出某個位置的分數
int State::calcOnePosGreedy(Board board, int player)
{
  int sum = 0;
  int playerA = 0, playerB = 0;
  for (int x = 0; x < 15; ++x)
  { //四个方向
    for (int y = 0; y < 15; ++y)
    { //每个方向上最多5个五元组
      if (checkBound(x, y))
      {
        if (board[x][y] == player)
          playerA++;
        else if (board[x][y] == 3 - player)
          playerB++;
      }
    }
    sum += tupleScoreGreedy(playerA, playerB, player);
  }
  return sum;
}

void State::init_tuple6type(int player)
{
  memset(tuple6type, 0, sizeof(tuple6type)); //全部設為0
  //連五
  tuple6type[player][player][player][player][player][player] = WIN;
  tuple6type[player][player][player][player][player][0] = WIN;
  tuple6type[0][player][player][player][player][player] = WIN;
  tuple6type[player][player][player][player][player][3 - player] = WIN;
  tuple6type[3 - player][player][player][player][player][player] = WIN;
  tuple6type[3][player][player][player][player][player] = WIN;
  tuple6type[player][player][player][player][player][3] = WIN;
  //對方連五
  tuple6type[3 - player][3 - player][3 - player][3 - player][3 - player][3 - player] = LOSE;
  tuple6type[3 - player][3 - player][3 - player][3 - player][3 - player][0] = LOSE;
  tuple6type[0][3 - player][3 - player][3 - player][3 - player][3 - player] = LOSE;
  tuple6type[3 - player][3 - player][3 - player][3 - player][3 - player][player] = LOSE;
  tuple6type[player][3 - player][3 - player][3 - player][3 - player][3 - player] = LOSE;
  tuple6type[3][3 - player][3 - player][3 - player][3 - player][3 - player] = LOSE;
  tuple6type[3 - player][3 - player][3 - player][3 - player][3 - player][3] = LOSE;
  //活4
  tuple6type[0][player][player][player][player][0] = FLEX4;
  //對方活4
  tuple6type[0][3 - player][3 - player][3 - player][3 - player][0] = flex4;
  //活3
  tuple6type[0][player][player][player][0][0] = FLEX3;
  tuple6type[0][0][player][player][player][0] = FLEX3;
  tuple6type[0][player][0][player][player][0] = FLEX3;
  tuple6type[0][player][player][0][player][0] = FLEX3;
  //對方活3
  tuple6type[0][3 - player][3 - player][3 - player][0][0] = flex3;
  tuple6type[0][0][3 - player][3 - player][3 - player][0] = flex3;
  tuple6type[0][3 - player][0][3 - player][3 - player][0] = flex3;
  tuple6type[0][3 - player][3 - player][0][3 - player][0] = flex3;
  //活2
  tuple6type[0][player][player][0][0][0] = FLEX2;
  tuple6type[0][player][0][player][0][0] = FLEX2;
  tuple6type[0][player][0][0][player][0] = FLEX2;
  tuple6type[0][0][player][player][0][0] = FLEX2;
  tuple6type[0][0][player][0][player][0] = FLEX2;
  tuple6type[0][0][0][player][player][0] = FLEX2;
  //對方活2
  tuple6type[0][3 - player][3 - player][0][0][0] = flex2;
  tuple6type[0][3 - player][0][3 - player][0][0] = flex2;
  tuple6type[0][3 - player][0][0][3 - player][0] = flex2;
  tuple6type[0][0][3 - player][3 - player][0][0] = flex2;
  tuple6type[0][0][3 - player][0][3 - player][0] = flex2;
  tuple6type[0][0][0][3 - player][3 - player][0] = flex2;
  //活1
  tuple6type[0][player][0][0][0][0] = FLEX1;
  tuple6type[0][0][player][0][0][0] = FLEX1;
  tuple6type[0][0][0][player][0][0] = FLEX1;
  tuple6type[0][0][0][0][player][0] = FLEX1;
  //對方活1
  tuple6type[0][3 - player][0][0][0][0] = flex1;
  tuple6type[0][0][3 - player][0][0][0] = flex1;
  tuple6type[0][0][0][3 - player][0][0] = flex1;
  tuple6type[0][0][0][0][3 - player][0] = flex1;

  int p1, p2, p3, p4, p5, p6, x, y, ix, iy; //x:左5中黑個數,y:左5中白個數,ix:右5中黑個數,iy:右5中白個數
  for (p1 = 0; p1 < 4; ++p1)
  {
    for (p2 = 0; p2 < 3; ++p2)
    {
      for (p3 = 0; p3 < 3; ++p3)
      {
        for (p4 = 0; p4 < 3; ++p4)
        {
          for (p5 = 0; p5 < 3; ++p5)
          {
            for (p6 = 0; p6 < 4; ++p6)
            {
              x = y = ix = iy = 0;

              if (p1 == 3 - player)
                x++;
              else if (p1 == player)
                y++;

              if (p2 == 3 - player)
              {
                x++;
                ix++;
              }
              else if (p2 == player)
              {
                y++;
                iy++;
              }

              if (p3 == 3 - player)
              {
                x++;
                ix++;
              }
              else if (p3 == player)
              {
                y++;
                iy++;
              }

              if (p4 == 3 - player)
              {
                x++;
                ix++;
              }
              else if (p4 == player)
              {
                y++;
                iy++;
              }

              if (p5 == 3 - player)
              {
                x++;
                ix++;
              }
              else if (p5 == player)
              {
                y++;
                iy++;
              }

              if (p6 == 3 - player)
                ix++;
              else if (p6 == player)
                iy++;

              if (p1 == 3 || p6 == 3)
              { //有邊界
                if (p1 == 3 && p6 != 3)
                { //左邊界
                  //死4
                  if (ix == 0 && iy == 4)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                  }
                  //死4
                  if (ix == 4 && iy == 0)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                  }
                  //眠3
                  if (ix == 0 && iy == 3)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                  }
                  //眠3
                  if (ix == 3 && iy == 0)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                  }
                  //眠2
                  if (ix == 0 && iy == 2)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                  }
                  //眠2
                  if (ix == 2 && iy == 0)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                  }
                }
                else if (p6 == 3 && p1 != 3)
                { //右邊界
                  //死4
                  if (x == 0 && y == 4)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                  }
                  //死4
                  if (x == 4 && y == 0)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                  }
                  //眠3
                  if (x == 3 && y == 0)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                  }
                  //眠3
                  if (x == 0 && y == 3)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                  }
                  //眠2
                  if (x == 2 && y == 0)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                  }
                  //眠2
                  if (x == 0 && y == 2)
                  {
                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                      tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                  }
                }
              }
              else
              { //無邊界
                //死4
                if ((x == 0 && y == 4) || (ix == 0 && iy == 4))
                {
                  if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                    tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                }
                //死4
                if ((x == 4 && y == 0) || (ix == 4 && iy == 0))
                {
                  if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                    tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                }
                //眠3
                if ((x == 0 && y == 3) || (ix == 0 && iy == 3))
                {
                  if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                    tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                }
                //眠3
                if ((x == 3 && y == 0) || (ix == 3 && iy == 0))
                {
                  if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                    tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                }
                //眠2
                if ((x == 0 && y == 2) || (ix == 0 && iy == 2))
                {
                  if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                    tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                }
                //眠2
                if ((x == 2 && y == 0) || (ix == 2 && iy == 0))
                {
                  if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                    tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                }
              }
            }
          }
        }
      }
    }
  }
}

//!state評分+輸贏情況
EVALUATION State::evaluate(Board board)
{

  int i, j, type;
  int stat[4][17]; //統計4方向上每種棋型的個數

  memset(stat, 0, sizeof(stat));

  int A[17][17]; //包括邊界的虛擬大棋盤,board[i][j]=A[i-1][j-1],3表示邊界
  for (int i = 0; i < 17; ++i)
  {
    A[i][0] = 3;
    A[i][16] = 3;
    A[0][i] = 3;
    A[16][i] = 3;
  }
  for (int i = 0; i < 15; ++i)
    for (int j = 0; j < 15; ++j)
      A[i + 1][j + 1] = board[i][j];

  //判断横向棋型
  for (i = 1; i <= 15; ++i)
  {
    for (j = 0; j < 12; ++j)
    {
      type = tuple6type[A[i][j]][A[i][j + 1]][A[i][j + 2]][A[i][j + 3]][A[i][j + 4]][A[i][j + 5]];
      stat[0][type]++;
    }
  }

  //判断縱向棋型
  for (j = 1; j <= 15; ++j)
  {
    for (i = 0; i < 12; ++i)
    {
      type = tuple6type[A[i][j]][A[i + 1][j]][A[i + 2][j]][A[i + 3][j]][A[i + 4][j]][A[i + 5][j]];
      stat[1][type]++;
    }
  }
  //判断左上至右下棋型
  for (i = 0; i < 12; ++i)
  {
    for (j = 0; j < 12; ++j)
    {
      type = tuple6type[A[i][j]][A[i + 1][j + 1]][A[i + 2][j + 2]][A[i + 3][j + 3]][A[i + 4][j + 4]][A[i + 5][j + 5]];
      stat[2][type]++;
    }
  }
  //判断右上至左下棋型
  for (i = 0; i < 12; ++i)
  {
    for (j = 5; j < 17; ++j)
    {
      type = tuple6type[A[i][j]][A[i + 1][j - 1]][A[i + 2][j - 2]][A[i + 3][j - 3]][A[i + 4][j - 4]][A[i + 5][j - 5]];
      stat[3][type]++;
    }
  }

  EVALUATION eval;
  memset(eval.STAT, 0, sizeof(eval.STAT));
  int score = 0;
  for (i = 1; i < 17; ++i)
  {
    score += (stat[0][i] + stat[1][i] + stat[2][i] + stat[3][i]) * weight[i]; //初步计分

    int count = stat[0][i] + stat[1][i] + stat[2][i] + stat[3][i]; //統計所有方向上部分棋型的個數
    if (i == WIN)
      eval.STAT[WIN] = count;
    else if (i == LOSE)
      eval.STAT[LOSE] = count;
    else if (i == FLEX4)
      eval.STAT[FLEX4] = count;
    else if (i == BLOCK4)
      eval.STAT[BLOCK4] = count;
    else if (i == FLEX3)
      eval.STAT[FLEX3] = count;
    else if (i == BLOCK3)
      eval.STAT[BLOCK3] = count;
    else if (i == FLEX2)
      eval.STAT[FLEX2] = count;
    else if (i == BLOCK2)
      eval.STAT[BLOCK2] = count;
  }

  eval.status = DRAW;
  //赢
  if (eval.STAT[WIN] > 0)
  {
    eval.result = player;
    game_state = FINISH;
  }
  //輸
  else if (eval.STAT[LOSE] > 0)
  {
    eval.result = 3 - player;
    game_state = FINISH;
  }

  eval.score = score;
  return eval;
}

EVALUATION State::evaluate(Board board, State &state)
{

  int i, j, type;
  int stat[4][17];

  memset(stat, 0, sizeof(stat));

  int A[17][17];
  for (int i = 0; i < 17; ++i)
  {
    A[i][0] = 3;
    A[i][16] = 3;
    A[0][i] = 3;
    A[16][i] = 3;
  }
  for (int i = 0; i < 15; ++i)
    for (int j = 0; j < 15; ++j)
      A[i + 1][j + 1] = board[i][j];

  for (i = 1; i <= 15; ++i)
  {
    for (j = 0; j < 12; ++j)
    {
      type = tuple6type[A[i][j]][A[i][j + 1]][A[i][j + 2]][A[i][j + 3]][A[i][j + 4]][A[i][j + 5]];
      stat[0][type]++;
    }
  }

  for (j = 1; j <= 15; ++j)
  {
    for (i = 0; i < 12; ++i)
    {
      type = tuple6type[A[i][j]][A[i + 1][j]][A[i + 2][j]][A[i + 3][j]][A[i + 4][j]][A[i + 5][j]];
      stat[1][type]++;
    }
  }
  for (i = 0; i < 12; ++i)
  {
    for (j = 0; j < 12; ++j)
    {
      type = tuple6type[A[i][j]][A[i + 1][j + 1]][A[i + 2][j + 2]][A[i + 3][j + 3]][A[i + 4][j + 4]][A[i + 5][j + 5]];
      stat[2][type]++;
    }
  }
  for (i = 0; i < 12; ++i)
  {
    for (j = 5; j < 17; ++j)
    {
      type = tuple6type[A[i][j]][A[i + 1][j - 1]][A[i + 2][j - 2]][A[i + 3][j - 3]][A[i + 4][j - 4]][A[i + 5][j - 5]];
      stat[3][type]++;
    }
  }

  EVALUATION eval;
  memset(eval.STAT, 0, sizeof(eval.STAT));
  int score = 0;
  for (i = 1; i < 17; ++i)
  {
    score += (stat[0][i] + stat[1][i] + stat[2][i] + stat[3][i]) * weight[i];

    int count = stat[0][i] + stat[1][i] + stat[2][i] + stat[3][i];
    if (i == WIN)
      eval.STAT[WIN] = count;
    else if (i == LOSE)
      eval.STAT[LOSE] = count;
    else if (i == FLEX4)
      eval.STAT[FLEX4] = count;
    else if (i == BLOCK4)
      eval.STAT[BLOCK4] = count;
    else if (i == FLEX3)
      eval.STAT[FLEX3] = count;
  }

  eval.status = DRAW;
  //白赢
  if (eval.STAT[WIN] > 0)
  {
    state.game_state = FINISH;
    EVALUATION eval1;
    eval1.status = FINISH;
    eval1.score = 9999999;
    return eval1;
  }
  //黑赢
  else if (eval.STAT[LOSE] > 0)
  {
    state.game_state = FINISH;
    EVALUATION eval1;
    eval1.status = FINISH;
    eval1.score = -9999999;
    return eval1;
  }

  eval.score = score;
  return eval;
}

// bool initSeek = true;

//!用來找目前局面時最佳的幾個落子點的位置以及落子之後的分數
POINTS State::seekPoints(Board board)
{
  bool B[15][15]; //标记数组
  int worth[15][15];
  POINTS best_points;

  memset(B, 0, sizeof(B));

  for (int i = 0; i < 15; ++i)
  { //每個非空點附近8個方向延伸2(或3)個深度,若不越界則標記為可走
    for (int j = 0; j < 15; ++j)
    {
      if (board[i][j] != C_NONE)
      {
        for (int k = -2; k <= 2; ++k)
        {
          if (i + k >= 0 && i + k < 15)
          {
            B[i + k][j] = true;
            if (j + k >= 0 && j + k < 15)
              B[i + k][j + k] = true;
            if (j - k >= 0 && j - k < 15)
              B[i + k][j - k] = true;
          }
          if (j + k >= 0 && j + k < 15)
            B[i][j + k] = true;
        }
      }
    }
  }

  for (int i = 0; i < 15; ++i)
  {
    for (int j = 0; j < 15; ++j)
    {
      worth[i][j] = -INT_MAX;
      if (board[i][j] == C_NONE && B[i][j] == true)
      {
        board[i][j] = 3 - player;
        worth[i][j] = calcOnePosGreedy(board, player);
      }
    }
  }

  int w;
  for (int k = 0; k < 10; ++k)
  {
    w = -INT_MAX;
    for (int i = 0; i < 15; ++i)
    {
      for (int j = 0; j < 15; ++j)
      {
        if (worth[i][j] > w)
        {
          w = worth[i][j];
          Point tmp(i, j);
          best_points.pos[k] = tmp;
        }
      }
    }

    int x = best_points.pos[k].x, y = best_points.pos[k].y;

    board[x][y] = player;
    best_points.score[k] = evaluate(board).score;
    // board[x][y] = C_NONE;

    worth[best_points.pos[k].x][best_points.pos[k].y] = -INT_MAX; //清除掉上一點,計算下一點的位置和分數
  }

  return best_points;
}

void State::copyBoard(Board boardA, Board &boardB)
{
  for (int i = 0; i < 15; ++i)
  {
    for (int j = 0; j < 15; ++j)
    {
      if (boardA[i][j] == C_NONE)
        boardB[i][j] = C_NONE;
      else if (boardA[i][j] == C_BLACK)
        boardB[i][j] = C_BLACK;
      else
        boardB[i][j] = C_WHITE;
    }
  }
}
void State::reverseBoard(Board boardA, Board &boardB)
{
  for (int i = 0; i < 15; ++i)
  {
    for (int j = 0; j < 15; ++j)
    {
      if (boardA[i][j] == C_NONE)
        boardB[i][j] = C_NONE;
      else if (boardA[i][j] == C_BLACK)
        boardB[i][j] = C_WHITE;
      else
        boardB[i][j] = C_BLACK;
    }
  }
}

//!AlphaBeta::eval minimaxAB 計算出分數
int State::miniAlphaBeta(State *state, int depth, int alpha, int beta)
{
  gameStatus nowState = (*state).game_state;
  if (nowState == FINISH)
  {
    delete state;
    return -INF;
  }
  if (nowState == DRAW)
  {
    delete state;
    return 0;
  }

  EVALUATION EVAL = evaluate(state->board);
  // std::cout << "EVAL: " << EVAL.score << std::endl;
  if (depth == 0 || EVAL.status != DRAW)
  { //抵達最深層
    return EVAL.score;
  }
  else if (depth % 2 == 0)
  { //max層,我方決策

    POINTS P = seekPoints(state->board);
    int v = -INF;
    for (auto move : P.pos)
    {

      State next_state(*state);
      next_state.board[move.x][move.y] = 3 - player;

      int a = miniAlphaBeta(&next_state, depth - 1, alpha, beta);
      v = std::max(v, a);
      if (v > alpha)
        alpha = v;
      if (beta <= alpha)
        break; //剪枝
    }

    return alpha;
  }
  else
  { //min層,敵方決策

    Board rBoard;
    reverseBoard(state->board, rBoard);
    POINTS P = seekPoints(rBoard); //找對於黑子的最佳位置,需要將棋盤不同顏色反轉,因為seekPoint是求我方的最佳位置
    int v = INF;
    for (auto move : P.pos)
    {
      State next_state(*state);
      next_state.board[move.x][move.y] = player;
      // sameBoard[P.pos[i].x][P.pos[i].y] = player; //模擬己方落子,不能用board,否則可能改變board
      int a = miniAlphaBeta(&next_state, depth - 1, alpha, beta);
      // next_state.board[move.x][move.y] = C_NONE;
      v = std::min(v, a);
      if (v < beta)
        beta = v;
      if (beta <= alpha)
        break; //剪枝
    }

    return beta;
  }
}

Point getNextMove(State &state)
{
  Point bestMove;
  int bestScore = -INF;
  Point InitMove;
  // Board bd;
  // state.copyBoard(state.board, bd);

  for (auto move : state.possible_actions)
  {
    if (board[move.x][move.y] != 0)
      continue;

    int score = 0;
    State next_state(state);
    next_state.board[move.x][move.y] = player;

    std::cout << "x , y " << move.x << " " << move.y << std::endl;

    score = state.miniAlphaBeta(&next_state, DEPTH - 1, -INF, INF);
    std::cout
        << "score: " << score << std::endl;
    // std::cout << "score: " << eval.score << std::endl;
    // std::cout << "test1" << std::endl;
    if (score > bestScore)
    {
      bestScore = score;
      bestMove = move;
    }
  }
  if (bestMove != Point(-1, -1))
  {
    if (DEBUG)
      std::cout << "BestScore: " << bestScore << '\n';
    if (DEBUG)
      std::cout << "BestMove: {" << bestMove.x << "," << bestMove.y << "}\n";
  }
  state.board[bestMove.x][bestMove.y] = 1;
  board[bestMove.x][bestMove.y] = 1;

  return bestMove;
}

void init_board(Board &bd)
{
  for (int i = 0; i < 15; ++i)
    for (int j = 0; j < 15; ++j)
      bd[i][j] = C_NONE;
}

void read_board(std::ifstream &fin)
{
  fin >> player;
  std::cout << player << '\n';
  for (int i = 0; i < SIZE; i++)
  {
    for (int j = 0; j < SIZE; j++)
    {
      fin >> board[i][j];
    }
  }
}

void write_valid_spot(std::ofstream &fout, State &state, int player)
{

  // std::cout << "test" << std::endl;
  int init_score = state.calcOnePosGreedy(state.board, player);
  // if (DEBUG)
  std::cout << "Initial Score: " << init_score << '\n';

  Point move = getNextMove(state);
  if (DEBUG)
    std::cout << "Final Move: {" << move.x << "," << move.y << "}\n";
  fout << move.x << " " << move.y << '\n';
  fout.flush();

  return;
}

int main(int, char **argv)
{
  srand(RANDOM_SEED);
  std::ifstream fin(argv[1]);
  std::ofstream fout(argv[2]);
  read_board(fin);
  // std::cout << "main" << std::endl;

  move_list_init();
  State start(board, player);

  write_valid_spot(fout, start, player);
  // std::cout << "board[SIZE / 2][SIZE / 2]  " << board[SIZE / 2][SIZE / 2] << std::endl;
  fin.close();
  fout.close();
  return 0;
}
