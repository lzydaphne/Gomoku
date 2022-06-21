#ifndef play_AB_H
#define play_AB_H

#define C_NONE 0
#define C_BLACK 1
#define C_WHITE 2

#define OTHER 0   //其他
#define WIN 1     //1000000,赢
#define LOSE 2    //-10000000
#define FLEX4 3   //50000,活4
#define flex4 4   //-100000
#define BLOCK4 5  //400
#define block4 6  //-100000
#define FLEX3 7   //400
#define flex3 8   //-8000
#define BLOCK3 9  //20
#define block3 10 //-40
#define FLEX2 11  //20
#define flex2 12  //-40
#define BLOCK2 13 //1
#define block2 14 //-2
#define FLEX1 15  //1
#define flex1 16  //-2
#include <list>
#include <vector>
#include <array>
#include "config.hpp"
struct Point
{
  int x, y;
  Point() : Point(0, 0) {}
  Point(float x, float y) : x(x), y(y) {}
  bool operator==(const Point &rhs) const
  {
    return x == rhs.x && y == rhs.y;
  }
  bool operator!=(const Point &rhs) const
  {
    return !operator==(rhs);
  }
  Point operator+(const Point &rhs) const
  {
    return Point(x + rhs.x, y + rhs.y);
  }
  Point operator-(const Point &rhs) const
  {
    return Point(x - rhs.x, y - rhs.y);
  }
};

enum gameStatus
{
  UNDERWAY,
  FINISH,
  DRAW,
};

struct EVALUATION
{
  int score;
  int result;
  gameStatus status;
  int STAT[16]; //儲存部分棋型的個數
};
struct POINTS
{
  Point pos[20]; //儲存較好的落子位,[0]分數最高,[19]分數最低
  int score[20]; //落子的分數
};
struct DECISION
{
  Point pos; //位置
  int eval;  //對分數的評估
};

extern std::vector<Point> move_list; //discs
extern void move_list_init();        //discs_init

//use bitset to build board
typedef std::array<int, SIZE> Row;
typedef std::array<Row, SIZE> Board; //Board

class State
{
public:
  DECISION decision; //儲存極大極小搜索得到的要走的位置
  Board board;
  int self_side;
  std::vector<Point>
      possible_actions;

private:
  int tuple6type[4][4][4][4][4][4]; //棋型辨識,0無子,1黑子,2白子,3邊界
  POINTS points;                    //!最佳落子位置
  void get_possible_actions(void);

public:
  State(Board, int);
  State(State &);

  gameStatus game_state = UNDERWAY;
  int calcOnePosGreedy(Board board, int C_ME);          //計算某一個位置對於me的分數
  int tupleScoreGreedy(int black, int white, int C_ME); //計算黑白子給定的陣列對於me的分數
  bool checkBound(int x, int y);
  bool checkEmpty(int x, int y);

public:
  // int heuristic(int player);
  // int getPointScore(EVALUATION eval);
  void init_tuple6type(int player);               //對棋型判斷數組賦初值
  POINTS seekPoints(Board board);                 //!生成對於棋子的最佳20個落子位置及分數
  void copyBoard(Board boardA, Board &boardB);    //將A棋盤複製到B棋盤
  void reverseBoard(Board boardA, Board &boardB); //將A棋盤黑白子顛倒結果傳給B棋盤
  EVALUATION evaluate(Board board, State &state); //!對棋局board的黑子的局勢估值函數,還可以判斷輸贏
  EVALUATION evaluate(Board board);
  int miniAlphaBeta(State *state, int depth, int alpha, int beta); //!極大極小搜索加ab剪枝
};

#endif
