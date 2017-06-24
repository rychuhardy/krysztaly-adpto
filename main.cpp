#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <array>

using namespace std;

namespace Sym {
    static constexpr char Crystal = '*';
    static constexpr char Blank = ' ';
    static constexpr char Block = '#';
    static constexpr char Mirror45 = '/';
    static constexpr char Mirror135 = '\\';

}

struct Position {
    unsigned row, col;

    Position(unsigned int row, unsigned int col) : row(row), col(col) {}

    bool operator==(const Position& other) {
        return row == other.row and col == other.col;
    }

    bool operator!=(const Position& other) {
        return !(*this == other);
    }
};

enum class Direction {
    Up = 0,
    Down,
    Left,
    Right,
};

struct PathCombinations {
    static constexpr size_t Size = 4;
    unsigned tab[Size][Size] = {};
};

struct Path {
    unsigned mirrorsUsed;
    Direction direction;
    Position current;

    bool operator>(const Path& other) const {
        return mirrorsUsed > other.mirrorsUsed;
    }
};

struct MazeDescription {
    vector<vector<char>> maze;
    vector<Position> crystalPositions;
    unsigned mirrors;

    MazeDescription(const vector<vector<char>> &maze, const vector<Position> &crystalPositions, unsigned int mirrors)
            : maze(maze), crystalPositions(crystalPositions), mirrors(mirrors) {}

    size_t getHeight() const {
        return maze.size();
    }

    size_t getWidth() const {
        return maze[0].size();
    }
};


vector<Path> getPathToNeighbours(const Path &path, const MazeDescription &description);

bool isCrystal(const Position &position, const MazeDescription &description);

Position advancePos(Position position, Direction direction);

bool needsTurning(Direction direction, Direction direction1);

template<class T>
vector<vector<T>> get2DVector(size_t width, size_t height) {
    vector<vector<T>> tmp(height);
    for_each(tmp.begin(), tmp.end(), [width](vector<T>& arg) {
        arg.resize(width);
    });
    return tmp;
}

vector<Direction> getPossibleOutDirections(const MazeDescription& mazeDescription, Position position) {
    const auto& maze = mazeDescription.maze;
    auto width = mazeDescription.getWidth();
    auto height = mazeDescription.getHeight();

    vector<Direction> result;
    result.reserve(4);

    if (position.row != 0 and maze.at(position.row - 1).at(position.col) != Sym::Block) {
        result.push_back(Direction::Up);
    }

    if (position.row + 1 != height and maze.at(position.row + 1).at(position.col) != Sym::Block) {
        result.push_back(Direction::Down);
    }

    if (position.col != 0 and maze.at(position.row).at(position.col - 1) != Sym::Block) {
        result.push_back(Direction::Left);
    }

    if (position.col + 1 != width and maze.at(position.row).at(position.col + 1) != Sym::Block) {
        result.push_back(Direction::Right);
    }

    return result;
}

void findShortestPaths(const MazeDescription &description) {
    auto size = description.crystalPositions.size() + 1; // includes starting point
    auto shortestPaths = get2DVector<PathCombinations>(size, size);


    for(size_t i = 0; i< size; ++i) {
        const auto& start = i < description.crystalPositions.size() ? description.crystalPositions[i] : Position{1, 0};

        auto outDirections = getPossibleOutDirections(description, start);

        for(auto direction: outDirections) {
                priority_queue<Path, vector<Path>, greater<Path>> toVisit;
                auto visited = get2DVector<bool>(description.getHeight(), description.getWidth());

                toVisit.push(Path{0, direction, start});
                visited[start.row][start.col] = true;
                while(!toVisit.empty()) {
                    auto current = toVisit.top();
                    toVisit.pop();

                    if(current.current != start and isCrystal(current.current, description)) {
                        auto& currentShortest = shortestPaths[i][description.crystalPositions.find(current.current)][direction][current.direction];
                        if (current.mirrorsUsed < currentShortest) {
                            currentShortest = current.mirrorsUsed;
                        }
                    }

                    auto neighbours = getPathToNeighbours(current, description);
                    for_each(neighbours.begin(), neighbours.end(), [&toVisit, &visited] (const Path& path) mutable {
                        toVisit.push(path);
                        visited[path.current.row][path.current.col] = true;
                    });
                }
        }
    }
}

bool isCrystal(const Position &position, const MazeDescription &description) {
    //We don't need to search for paths that end in start
    return description.maze[position.row][position.col] == Sym::Crystal;
}

vector<Path> getPathToNeighbours(const Path &path, const MazeDescription &description) {
    vector<Path> neighbours;

    auto out = getPossibleOutDirections(description, path.current);

    for(auto direction: out) {
        if(direction != path.direction) {
            int cost = 0;
            if(needsTurning(direction, path.direction)) {
                ++cost;
            }
            neighbours.emplace_back(path.mirrorsUsed + cost, direction, advancePos(path.current, direction));
        }
    }
    return neighbours;
}

bool needsTurning(Direction direction, Direction direction1) {
    switch(direction) {
        case Direction::Up:
        case Direction::Down:
            return direction1 == Direction::Left or direction1 == Direction::Right;
        case Direction::Left:
        case Direction::Right:
            return direction1 == Direction::Up or direction1 == Direction::Down;

    }
}

Position advancePos(Position position, Direction direction) {
    switch (direction) {
        case Direction::Up:
            return {position.row - 1, position.col};
        case Direction::Down:
            return {position.row + 1, position.col};
        case Direction::Left:
            return {position.row, position.col - 1};
        case Direction::Right:
            return {position.row, position.col + 1};
    }
}

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

    unsigned height, width, mirrors;

    cin >> height >> width >> mirrors;

    auto maze = get2DVector<char>(height, width);
    vector<Position> crystalPositions;
    crystalPositions.reserve(mirrors); // it's just a guess

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
    findShortestPaths(mazeDescription);


    cout << "Done\n";
    printSolution(mazeDescription);
//    char c;
//    cin >> noskipws >> c;
//    cout << "X" << c << "X" << endl;
    return 0;
}