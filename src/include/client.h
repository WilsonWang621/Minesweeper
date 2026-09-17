#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <ctime>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.
char client_map[35][35];  // 存储当前可见的地图状态
bool visited[35][35];     // 记录哪些格子已经访问过
bool marked[35][35];      // 记录哪些格子已经标记为地雷

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the game. It should be called at the beginning of the game, which
 * will read the scale of the game map and the first step taken by the server (see README).
 */
void InitGame() {
  // TODO (student): Initialize all your global variables!
  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}


/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 * Since the client (or player) can only get the limited information of the game map, so if there is a 3 * 3 map as
 * above and only the block (2, 0) has been visited, the stdin would be
 *     ???
 *     12?
 *     01?
 */
void ReadMap() {
  // TODO (student): Implement me!
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      std::cin >> client_map[i][j];

      // 更新访问状态和标记状态
      if (client_map[i][j] != '?') {
        visited[i][j] = 1;
        if (client_map[i][j] == '@') {
          marked[i][j] = 1;
        } else {
          marked[i][j] = 0;
        }
      } else {
        visited[i][j] = 0;
        marked[i][j] = 0;
      }
    }
  }
}

inline bool ValidVisit1(int pos_x , int pos_y) {
    return pos_x < rows && pos_x >= 0 && pos_y < columns && pos_y >= 0;
  }

/**
 * @brief The definition of function Decide()
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
void Decide() {
  // TODO (student): Implement me!
  // 策略1: 首先寻找可以确定的安全格子
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (visited[i][j] && client_map[i][j] >= '1' && client_map[i][j] <= '9') {
        int num = client_map[i][j] - '0';
        int unknown_count = 0;               //周围未知格子数
        int marked_count = 0;                //周围标记雷数

        // 统计周围的未知格子和标记的地雷数
        for (int dx = -1; dx <= 1; ++dx) {
          for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int ni = i + dx, nj = j + dy;
            if (ValidVisit1(ni , nj)) {
              if (!visited[ni][nj] && !marked[ni][nj]) {
                unknown_count++;
              } else if (marked[ni][nj]) {
                marked_count++;
              }
            }
          }
        }

        // 如果标记数等于数字，那么剩下的未知格子都是安全的
        if (marked_count == num && unknown_count > 0) {
          Execute(i, j, 2);        // 访问安全格子
          return;
        }

        // 如果未知格子数等于剩余地雷数，那么这些未知格子都是地雷
        if (unknown_count == (num - marked_count) && unknown_count > 0) {
          for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
              if (dx == 0 && dy == 0) continue;
              int ni = i + dx, nj = j + dy;
              if (ValidVisit1(ni , nj) && !visited[ni][nj] ) {
                  Execute(ni, nj, 1);       // 标记地雷
                  return;
              }
            }
          }
        }
      }
    }
  }

  // 策略2: 对于数字为0的格子，自动探索周围的格子
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (visited[i][j] && client_map[i][j] == '0') {
        // 检查周围是否有未访问的格子
        for (int dx = -1; dx <= 1; ++dx) {
          for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int ni = i + dx, nj = j + dy;
            if (ValidVisit1(ni , nj) && !visited[ni][nj] ) {
                Execute(i, j, 2);  // 自动探索
                return;
            }
          }
        }
      }
    }
  }

  //策略3 随机法
  std::srand(std::time(0));
  while (1) {
    int r = std::rand() % rows , c = std::rand() % columns;
    if (client_map[r][c] == '?') {
      Execute(r , c , 0);
      break;
    }
  }
  return;
}

#endif