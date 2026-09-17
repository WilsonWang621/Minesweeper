#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <random>
#include <vector>
#include <cstring>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.
char client_map[35][35];  // 存储当前可见的地图状态
bool visited[35][35];     // 记录哪些格子已经访问过
bool marked[35][35];      // 记录哪些格子已经标记为地雷
int first_row, first_column;

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
  std::memset(client_map, '?', sizeof(client_map));
  std::memset(visited, 0, sizeof(visited));
  std::memset(marked, 0, sizeof(marked));
  first_row = 0;
  first_column = 0;

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

bool ValidVisit1(int pos_x , int pos_y) {
  return (pos_x >= 0 && pos_x < rows && pos_y >= 0 && pos_y < columns);
}

bool FindSafeMove() {
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      // 跳过未访问的格子或不是数字的格子
      if (!visited[i][j] || client_map[i][j] < '1' || client_map[i][j] > '8') {
        continue;
      }

      int num = client_map[i][j] - '0';
      int unknown_count = 0;
      int marked_count = 0;
      std::vector<std::pair<int, int>> unknown_cells;

      // 统计周围情况
      for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
          if (dx == 0 && dy == 0) continue;
          int ni = i + dx, nj = j + dy;
          if (ValidVisit1(ni, nj)) {
            if (!visited[ni][nj] && !marked[ni][nj]) {
              unknown_count++;
              unknown_cells.push_back({ni, nj});
            } else if (marked[ni][nj]) {
              marked_count++;
            }
          }
        }
      }

      // 策略1: 如果未知格子数等于剩余地雷数，那么这些未知格子都是地雷
      if (unknown_count > 0 && unknown_count == (num - marked_count)) {
        for (auto& cell : unknown_cells) {
          if (!marked[cell.first][cell.second]) {
            Execute(cell.first, cell.second, 1);  // 标记为地雷
            return true;
          }
        }
      }

      // 策略2: 如果标记数等于数字，那么剩下的未知格子都是安全的
      if (marked_count == num && unknown_count > 0) {
        for (auto& cell : unknown_cells) {
          if (!visited[cell.first][cell.second]) {
            Execute(cell.first, cell.second, 0);  // 访问安全格子
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool FindAutoExplore() {
  // 策略1: 优先自动探索数字为0的格子
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (visited[i][j] && client_map[i][j] == '0') {
        Execute(i, j, 2);
        return true;
      }
    }
  }

  // 策略2: 其他可自动探索的格子
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (visited[i][j] && client_map[i][j] >= '1' && client_map[i][j] <= '8') {
        int num = client_map[i][j] - '0';
        int marked_count = 0;

        // 统计周围标记的地雷数
        for (int dx = -1; dx <= 1; ++dx) {
          for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int ni = i + dx, nj = j + dy;
            if (ValidVisit1(ni, nj) && marked[ni][nj]) {
              marked_count++;
            }
          }
        }

        // 如果标记数等于数字，执行自动探索
        if (marked_count == num) {
          Execute(i, j, 2);
          return true;
        }
      }
    }
  }
  return false;
}


void MakeLeastRiskyRandomMove() {
  std::vector<std::pair<int, int>> candidates;  // 靠近数字的格子
  std::vector<std::pair<int, int>> fallback;    // 其他未知格子

  // 收集所有未访问且未标记的格子
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (!visited[i][j] && !marked[i][j]) {
        // 检查是否靠近数字（有更高的信息价值）
        bool near_number = false;
        for (int dx = -1; dx <= 1; ++dx) {
          for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int ni = i + dx, nj = j + dy;
            if (ValidVisit1(ni, nj) && visited[ni][nj] &&
                client_map[ni][nj] >= '1' && client_map[ni][nj] <= '8') {
              near_number = true;
              break;
                }
          }
          if (near_number) break;
        }

        if (near_number) {
          candidates.push_back({i, j});
        } else {
          fallback.push_back({i, j});
        }
      }
    }
  }

  // 优先选择靠近数字的格子
  if (!candidates.empty()) {
    int idx = std::rand() % candidates.size();
    Execute(candidates[idx].first, candidates[idx].second, 0);
  }
  // 其次选择其他未知格子
  else if (!fallback.empty()) {
    int idx = std::rand() % fallback.size();
    Execute(fallback[idx].first, fallback[idx].second, 0);
  }
  // 最后的选择（理论上不会执行到这里）
  else {
    // 遍历找到第一个未访问的格子
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < columns; ++j) {
        if (!visited[i][j] && !marked[i][j]) {
          Execute(i, j, 0);
          return;
        }
      }
    }
    // 如果真的找不到，选择(0,0)
    Execute(0, 0, 0);
  }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
void Decide() {
  // 策略优先级：确定推理 > 自动探索 > 概率选择

  // 1. 寻找确定的安全移动（标记地雷或访问安全格子）
  if (FindSafeMove()) {
    return;
  }

  // 2. 寻找可以自动探索的格子
  if (FindAutoExplore()) {
    return;
  }

  // 3. 最小风险随机选择
  MakeLeastRiskyRandomMove();
}


#endif