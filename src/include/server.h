#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>

/*
 * You may need to define some global variables for the information of the game map here.
 * Although we don't encourage to use global variables in real cpp projects, you may have to use them because the use of
 * class is not taught yet. However, if you are member of A-class or have learnt the use of cpp class, member functions,
 * etc., you're free to modify this structure.
 */
int rows = 0;         // The count of rows of the game map. You MUST NOT modify its name.
int columns = 0;      // The count of columns of the game map. You MUST NOT modify its name.
int total_mines = 0;  // The count of mines of the game map. You MUST NOT modify its name. You should initialize this
                  // variable in function InitMap. It will be used in the advanced task.
int game_state = 0;  // The state of the game, 0 for continuing, 1 for winning, -1 for losing. You MUST NOT modify its name.
int target_num = 0;         //目标访问数
int target_mine = 0;        //目标地雷数
int visit_count = 0;        //访问格子数
int marked_mine_count = 0;  //标记地雷数
char map[35][35];
int vis[35][35];
/**
 * @brief The definition of function InitMap()
 *
 * @details This function is designed to read the initial map from stdin. For example, if there is a 3 * 3 map in which
 * mines are located at (0, 1) and (1, 2) (0-based), the stdin would be
 *     3 3
 *     .X.
 *     ...
 *     ..X
 * where X stands for a mine block and . stands for a normal block. After executing this function, your game map
 * would be initialized, with all the blocks unvisited.
 */
void InitMap() {
  std::cin >> rows >> columns;                   //输入棋盘大小
  // TODO (student): Implement me!
  for (int row = 0 ; row < rows ; row++ ) {
    for (int col = 0 ; col < columns ; col++ ) {
      std::cin >> map[row][col];                 //每个格子是什么
      vis[row][col] = 0;                         //初始化未访问
      if (map[row][col] == '.') {                //记录目标访问数
        target_num++;
      }
    }
  }
  target_mine = rows * columns - target_num;     //记录目标地雷数
}

bool ValidVisit(int pos_x , int pos_y) {         //有效访问，有没有出棋盘
    return (pos_x < rows && pos_x >= 0 && pos_y < columns && pos_y >= 0);
}

int MineNum(int pos_x , int pos_y) {             //返回周围有几个mine
  int ans = 0;
  for(int i = -1 ; i <= 1 ; i++ ){
    for(int j = -1 ; j <= 1 ; j++){
      if(i == 0 && j == 0)  continue;            // 跳过自身
      if(ValidVisit( pos_x + i , pos_y + j )){
        if(map[pos_x + i][pos_y + j] == 'X'){
          ans++;
        }
      }
    }
  }
  return ans;
}
/**
 * @brief The definition of function VisitBlock(int, int)
 *
 * @details This function is designed to visit a block in the game map. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call VisitBlock(0, 0), the return value would be 0 (game continues), and the game map would
 * be
 *     1??
 *     ???
 *     ???
 * If you call VisitBlock(0, 1) after that, the return value would be -1 (game ends and the players loses) , and the
 * game map would be
 *     1X?
 *     ???
 *     ???
 * If you call VisitBlock(0, 2), VisitBlock(2, 0), VisitBlock(1, 2) instead, the return value of the last operation
 * would be 1 (game ends and the player wins), and the game map would be
 *     1@1
 *     122
 *     01@
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void VisitBlock(int r, int c) {                //用于访问的函数
  // TODO (student): Implement me!
  if (vis[r][c] == 0 && map[r][c] == 'X') {    //访问到雷 game over
    game_state = -1;
    vis[r][c] = 2;                             //错误标记，结束时输出X
  }
  if (vis[r][c] == 0 && map[r][c] == '.') {    //访问到未访问过非雷的格子
    vis[r][c] = 1;
    visit_count++;
    if (visit_count == target_num) {           //如果达成目标数 win!
      game_state = 1;
    }
    if (MineNum(r , c) == 0) {      //如果安全，把周围的全访问一边，并递归
      for (int i = -1 ; i < 2 ; ++i) {
        for (int j = -1 ; j < 2 ; ++j) {
          if (ValidVisit(r + i , c + j)){
            VisitBlock(r + i , c + j);
          }
        }
      }
    }
  }
}

/**
 * @brief The definition of function MarkMine(int, int)
 *
 * @details This function is designed to mark a mine in the game map.
 * If the block being marked is a mine, show it as "@".
 * If the block being marked isn't a mine, END THE GAME immediately. (NOTE: This is not the same rule as the real
 * game) And you don't need to
 *
 * For example, if we use the same map as before, and the current state is:
 *     1?1
 *     ???
 *     ???
 * If you call MarkMine(0, 1), you marked the right mine. Then the resulting game map is:
 *     1@1
 *     ???
 *     ???
 * If you call MarkMine(1, 0), you marked the wrong mine(There's no mine in grid (1, 0)).
 * The game_state would be -1 and game ends immediately. The game map would be:
 *     1?1
 *     X??
 *     ???
 * This is different from the Minesweeper you've played. You should beware of that.
 *
 * @param r The row coordinate (0-based) of the block to be marked.
 * @param c The column coordinate (0-based) of the block to be marked.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void MarkMine(int r, int c) {         //标雷
  // TODO (student): Implement me!
  if (vis[r][c] == 0) {               //如果没访问过
    if (map[r][c] == 'X') {           //确实是雷
      vis[r][c] = 1;
      marked_mine_count++;
      if (marked_mine_count == target_mine) {
        game_state = 1;
      }
    }
    else {                            //标错了 game over
      game_state = -1;
      vis[r][c] = 2;                  //错误标记，结束时输出X
    }
  }
}

/**
 * @brief The definition of function AutoExplore(int, int)
 *
 * @details This function is designed to auto-visit adjacent blocks of a certain block.
 * See README.md for more information
 *
 * For example, if we use the same map as before, and the current map is:
 *     ?@?
 *     ?2?
 *     ??@
 * Then auto explore is available only for block (1, 1). If you call AutoExplore(1, 1), the resulting map will be:
 *     1@1
 *     122
 *     01@
 * And the game ends (and player wins).
 */
void AutoExplore(int r, int c) {               //自动探索
  // TODO (student): Implement me!
  int mark_num = 0;                            //周围有多少标记的地雷
  if (vis[r][c] == 1 && map[r][c] == '.') {    //已被访问的非雷
    for (int i = -1 ; i < 2 ; ++i) {
      for (int j = -1 ; j < 2 ; ++j) {
        if (ValidVisit(r + i , c + j) && vis[r+i][c+j] == 1 && map[r+i][c+j] == 'X') {
          mark_num++;                          //记录已被成功标记的雷数
        }
      }
    }
    if (mark_num == MineNum(r , c)) { //判断是否安全
      for (int i = -1 ; i < 2 ; ++i) {
        for (int j = -1 ; j < 2 ; ++j) {
          if (ValidVisit(r + i , c + j)){
            VisitBlock(r + i, c + j);
          }
        }
      }
    }
  }
}

/**
 * @brief The definition of function ExitGame()
 *
 * @details This function is designed to exit the game.
 * It outputs a line according to the result, and a line of two integers, visit_count and marked_mine_count,
 * representing the number of blocks visited and the number of marked mines taken respectively.
 *
 * @note If the player wins, we consider that ALL mines are correctly marked.
 */
void ExitGame() {
  // TODO (student): Implement me!
  if (game_state == -1) {
    std::cout << "GAME OVER!" << std::endl;
    std::cout << visit_count << " " << marked_mine_count;
  }
  else {
    std::cout << "YOU WIN!" << std::endl;
    std::cout << target_num << " " << target_mine;
  }

  exit(0);  // Exit the game immediately
}

/**
 * @brief The definition of function PrintMap()
 *
 * @details This function is designed to print the game map to stdout. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call PrintMap(), the stdout would be
 *    ???
 *    ???
 *    ???
 * If you call VisitBlock(2, 0) and PrintMap() after that, the stdout would be
 *    ???
 *    12?
 *    01?
 * If you call VisitBlock(0, 1) and PrintMap() after that, the stdout would be
 *    ?X?
 *    12?
 *    01?
 * If the player visits all blocks without mine and call PrintMap() after that, the stdout would be
 *    1@1
 *    122
 *    01@
 * (You may find the global variable game_state useful when implementing this function.)
 *
 * @note Use std::cout to print the game map, especially when you want to try the advanced task!!!
 */
void PrintMap() {
  // TODO (student): Implement me!
  if (game_state != 1){                 //一共有四种输出的可能
    for (int row = 0 ; row < rows ; row++ ) {
    for (int col = 0 ; col < columns ; col++ ) {
      if (vis[row][col] == 0) {
        std::cout << '?';
      }
      else if (vis[row][col] == 1){
        if (map[row][col] == '.') {
          std::cout << MineNum(row , col);
        }
        else if(map[row][col] == 'X') {
          std::cout << '@';
        }
      }
      else {
          std::cout << 'X';
      }
    }
    std::cout << std::endl;
    }
  }
  else {                                      // 赢了 ，全部打印
    for (int row = 0 ; row < rows ; row++ ) {
      for (int col = 0 ; col < columns ; col++ ) {
        if (map[row][col] == '.') {
          std::cout << MineNum(row , col);
        }
        else {
          std::cout << '@';
        }
      }
      std::cout << std::endl;
    }
  }
}


#endif
