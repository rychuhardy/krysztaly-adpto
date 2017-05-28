#include <iostream>
#include <vector>

using namespace std;

struct MazeDescription {
    vector<vector<char>> maze;
    unsigned mirrors;
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
    unsigned height, width, mirrors;

    cin >> height >> width >> mirrors;

    maze.resize(height);
    cin.ignore(); // newline after mirrors
    for(size_t i = 0; i < height; ++i) {
        maze[i].resize(width);
        for(size_t j = 0; j < width; ++j) {
            cin >> noskipws >> maze[i][j];
        }
        cin.ignore(); // newline after a row
    }

    return MazeDescription{maze, mirrors};
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