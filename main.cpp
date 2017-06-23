#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

namespace Sym {
    static constexpr char Crystal = '*';
    static constexpr char Blank = ' ';
    static constexpr char Block = '#';
    static constexpr char Mirror45 = '/';
    static constexpr char Mirror135 = '\\';

}

struct Position {
    unsigned x,y;

    Position(unsigned int x, unsigned int y) : x(x), y(y) {}
};

struct MazeDescription {
    vector<vector<char>> maze;
    vector<Position> crystalPositions;
    unsigned mirrors;

    MazeDescription(const vector<vector<char>> &maze, const vector<Position> &crystalPositions, unsigned int mirrors)
            : maze(maze), crystalPositions(crystalPositions), mirrors(mirrors) {}
};

void printSolution(const MazeDescription& mazeDescription) {
    const auto& maze = mazeDescription.maze;
    cout << maze.size() << ' ' << maze[0].size() << '\n' << mazeDescription.mirrors << '\n';
    for(const auto& row: maze) {
        for(const auto& el: row) {
            cout << el;
        }
        cout << '\n';
    }
}

MazeDescription readInput() {
    vector<vector<char>> maze;
    vector<Position> crystalPositions;
    unsigned height, width, mirrors;

    cin >> height >> width >> mirrors;

    maze.resize(height);
    crystalPositions.reserve(mirrors); // It's just a guess
    for_each(maze.begin(), maze.end(), [width](vector<char>& row) {row.resize(width);});

    cin.ignore(); // newline after mirrors
    for(size_t i = 0; i < height; ++i) {
        for(size_t j = 0; j < width; ++j) {
            cin >> noskipws >> maze[i][j];
            if (maze[i][j] == Sym::Crystal) {
                crystalPositions.emplace_back(i, j);
            }
        }
        cin.ignore(); // newline after a row
    }

    return MazeDescription{maze, crystalPositions, mirrors};
}

int main() {
    auto mazeDescription = readInput();



    cout << "Done\n";
    printSolution(mazeDescription);
//    char c;
//    cin >> noskipws >> c;
//    cout << "X" << c << "X" << endl;
    return 0;
}