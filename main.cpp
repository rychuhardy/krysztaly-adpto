#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <array>
#include <climits>
#include <set>
#include <map>

using namespace std;


#define LOGBUILD

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

    Position(const Position &) = default;

    Position(Position &&) = default;

    Position &operator=(const Position &) = default;
    Position &operator=(Position &&) = default;

    bool operator==(const Position &other) const {
        return row == other.row and col == other.col;
    }

    bool operator!=(const Position &other) const {
        return !(*this == other);
    }
};

struct MirrorPos {
    char type;
    Position position;

    MirrorPos(const char type, const Position &position) : type(type), position(position) {}

    MirrorPos(const MirrorPos& ) = default;
    MirrorPos(MirrorPos&& ) = default;

    MirrorPos& operator=(const MirrorPos& ) = default;
    MirrorPos& operator=(MirrorPos&& ) = default;
};

enum class Direction {
    Up = 0,
    Down,
    Left,
    Right
};

struct PathCombinations {
    static constexpr size_t Size = 4;
    pair<unsigned, vector<MirrorPos>> tab[Size][Size];
/* = {
            {{UINT_MAX, {}}, {UINT_MAX, {}}, {UINT_MAX, {}}, {UINT_MAX, {}}},
            {{UINT_MAX, {}}, {UINT_MAX, {}}, {UINT_MAX, {}}, {UINT_MAX, {}}},
            {{UINT_MAX, {}}, {UINT_MAX, {}}, {UINT_MAX, {}}, {UINT_MAX, {}}},
            {{UINT_MAX, {}}, {UINT_MAX, {}}, {UINT_MAX, {}}, {UINT_MAX, {}}}
    }; */

    PathCombinations() {
        for(size_t i = 0; i<4; ++i) {
            for(size_t j=0; j<4; ++j) {
                tab[i][j].first = UINT_MAX;
            }
        }
    }   
};

struct Path {
    unsigned mirrorsUsed;
    Direction direction;
    Position current;
    vector<MirrorPos> mirrorsPoses;

    Path(unsigned int mirrorsUsed, Direction direction, const Position &current, const vector<MirrorPos> &mirrorPoses = std::vector<MirrorPos>())
            : mirrorsUsed(mirrorsUsed),
              direction(direction),
              current(current),
              mirrorsPoses(mirrorPoses) {}

    Path(const Path &) = default;

    Path(Path &&) = default;

    Path &operator=(const Path &) = default;

    bool operator>(const Path &other) const {
        return mirrorsUsed > other.mirrorsUsed;
    }

    void putMirror(char type, const Position &position) {
        mirrorsPoses.emplace_back(type, position);
    }
};

struct MazeDescription {
    vector<vector<char>> maze;
    vector<Position> crystalPositions;
    unsigned mirrors;

    MazeDescription(const vector<vector<char>> &maze, const vector<Position> &crystalPositions, unsigned int mirrors)
            : maze(maze), crystalPositions(crystalPositions), mirrors(mirrors) {}

    MazeDescription(MazeDescription &&) = default;

    size_t getHeight() const {
        return maze.size();
    }

    size_t getWidth() const {
        return maze[0].size();
    }

    unsigned maxMirrors() const {
        return mirrors;
    }
};


vector<Path> getPathToNeighbours(const Path &path, const MazeDescription &description, bool isFieldCrystal);

bool isCrystal(const Position &position, const MazeDescription &description);

Position advancePos(Position position, Direction direction);

bool needsTurning(Direction direction, Direction direction1);

size_t indexOfCrystal(const vector<Position> &positions, const Position &toFind);

Direction oppositeDirection(Direction direction);

vector<Direction> getPossibleTurns(const MazeDescription &description, const Position &position);

void combinePaths(MazeDescription &description, const vector<vector<PathCombinations>> &pathCombinations, const vector<int>& minCostForRemainingNodes);

void
expandPath(const vector<vector<PathCombinations>> &pathCombinations, const set<size_t>& remainingCrystals, size_t lastPoint,
           size_t cost,
           Direction lastDirection, unsigned maxCost, MazeDescription& description, const vector<MirrorPos>& allMirrors, const vector<int>& minCostForRemainingNodes);

char mirrorType(Direction from, Direction to);

bool willCollide(const vector<MirrorPos>& newMirrors, size_t start, unsigned long end,
                 const MazeDescription &description);

vector<int> calculateMinCostForRemainingNodes(const vector<vector<PathCombinations>> &shortestPaths, size_t howMuch);

template<class T>
vector<vector<T>> get2DVector(size_t width, size_t height) {
    vector<vector<T>> tmp(height);
    for_each(tmp.begin(), tmp.end(), [width](vector<T> &arg) {
        arg.resize(width);
    });
    return tmp;
}

vector<Direction> getPossibleStartDirections(const MazeDescription &mazeDescription, Position position) {
    // To have possible out direction the corresponding field on the other side has to be free as well (except for the starting point)
    auto &maze = mazeDescription.maze;
    auto width = mazeDescription.getWidth();
    auto height = mazeDescription.getHeight();

    vector<Direction> result;
    result.reserve(4);

    if (position.row != 0 and maze.at(position.row - 1).at(position.col) != Sym::Block and
        position.row + 1 != height and maze.at(position.row + 1).at(position.col) != Sym::Block) {
        result.push_back(Direction::Up);
        result.push_back(Direction::Down);
    }

    if (position.col != 0 and maze.at(position.row).at(position.col - 1) != Sym::Block and
        position.col + 1 != width and maze.at(position.row).at(position.col + 1) != Sym::Block) {
        result.push_back(Direction::Left);
        result.push_back(Direction::Right);
    }

    return result;
}

vector<vector<PathCombinations>> findShortestPaths(const MazeDescription &description) {
    auto size = description.crystalPositions.size() + 1; // includes starting point as the last index
    auto shortestPaths = get2DVector<PathCombinations>(size, size);


    for (size_t i = 0; i < size; ++i) {
        Position start(-1, -1);
        vector<Direction> startDirections;
        if (i < description.crystalPositions.size()) {
            start = description.crystalPositions[i];
            startDirections = getPossibleStartDirections(description, start);
        } else {
            start = Position{1, 0};
            startDirections.push_back(Direction::Left);
        }

        for (auto direction: startDirections) {
            priority_queue<Path, vector<Path>, greater<Path>> toVisit;
            auto visited = get2DVector<bool>(description.getWidth(), description.getHeight());

            toVisit.push(Path{0, direction, start});
            while (!toVisit.empty()) {
                auto current = toVisit.top();
                toVisit.pop();
                visited[current.current.row][current.current.col] = true;

                bool isFieldCrystal = isCrystal(current.current, description);
                if (current.current != start and isFieldCrystal) {
                    auto &currentShortest = shortestPaths[i][indexOfCrystal(description.crystalPositions,
                                                                            current.current)].tab[(size_t) direction][(size_t) current.direction];
                    if (current.mirrorsUsed < currentShortest.first) {
                        currentShortest.first = current.mirrorsUsed;
                        currentShortest.second = current.mirrorsPoses;
                    }
                }

                auto neighbours = getPathToNeighbours(current, description, isFieldCrystal);
                for_each(neighbours.begin(), neighbours.end(),
                         [&toVisit, &visited, &description](const Path &path) mutable {
                             if (/*(not visited[path.current.row][path.current.col] or
                                  isCrystal(path.current, description)) and */
                                 path.mirrorsUsed <= description.maxMirrors()) {
                                 toVisit.push(path);
                             }
                         });
            }
        }
    }
    return shortestPaths;
}

size_t indexOfCrystal(const vector<Position> &positions, const Position &toFind) {
    size_t i = 0;
    while (i < positions.size() and toFind != positions[i]) {
        ++i;
    }
    return i;
}

bool isCrystal(const Position &position, const MazeDescription &description) {
    //We don't need to search for paths that end in start
    return description.maze[position.row][position.col] == Sym::Crystal;
}

vector<Path> getPathToNeighbours(const Path &path, const MazeDescription &description, bool isFieldCrystal) {
    vector<Path> neighbours;
    neighbours.reserve(4);
    auto out = getPossibleTurns(description, path.current);

    for (auto direction: out) {
        if (direction != path.direction) {
            int cost = 0;
            if (needsTurning(direction, path.direction)) {
                ++cost;
            }
            if (not isFieldCrystal or cost == 0) {
                neighbours.emplace_back(path.mirrorsUsed + cost, oppositeDirection(direction),
                                        advancePos(path.current, direction), path.mirrorsPoses);
                if(cost > 0) {
                    neighbours.back().putMirror(mirrorType(path.direction, direction), path.current);
                }
            }
        }
    }
    return neighbours;
}

char mirrorType(Direction from, Direction to) {
    switch (from) {
        case Direction::Up:
            return to == Direction::Left ? Sym::Mirror45 : Sym::Mirror135;
        case Direction::Down:
            return to == Direction::Left ? Sym::Mirror135 : Sym::Mirror45;
        case Direction::Left:
            return  to==Direction::Up ? Sym::Mirror45 : Sym::Mirror135;
        case Direction::Right:
            return  to==Direction::Up ? Sym::Mirror135 : Sym::Mirror45;
    }
    return 'x'; //dummy return all cases covered
}

vector<Direction> getPossibleTurns(const MazeDescription &description, const Position &position) {
    auto &maze = description.maze;
    auto width = description.getWidth();
    auto height = description.getHeight();

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

Direction oppositeDirection(Direction direction) {
    switch (direction) {
        case Direction::Up:
            return Direction::Down;
        case Direction::Down:
            return Direction::Up;
        case Direction::Left:
            return Direction::Right;
        case Direction::Right:
            return Direction::Left;
    }
    return Direction::Up; //dummy return all cases covered
}

bool needsTurning(Direction direction, Direction direction1) {
    switch (direction) {
        case Direction::Up:
        case Direction::Down:
            return direction1 == Direction::Left or direction1 == Direction::Right;
        case Direction::Left:
        case Direction::Right:
            return direction1 == Direction::Up or direction1 == Direction::Down;
    }
    return false; //dummy return all cases covered
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
    return Position(-1, -1); //dummy return all cases covered
}

void printSolution(const MazeDescription &mazeDescription) {
#ifdef LOGBUILD
    char i = '0';
#endif //LOGBUILD
    const auto &maze = mazeDescription.maze;
    cout << maze.size() << ' ' << maze[0].size() << '\n' << mazeDescription.mirrors << '\n';
    for (const auto &row: maze) {
        for (const auto &el: row) {
#ifdef LOGBUILD
            if (el == '*') {
                cout << i;
                ++i;
            }
            else
#endif //LOGBUILD
            cout << el;
        }
        cout << '\n';
    }
}

MazeDescription readInput() {

    unsigned height, width, mirrors;

    cin >> height >> width >> mirrors;

    auto maze = get2DVector<char>(width, height);
    vector<Position> crystalPositions;
    crystalPositions.reserve(mirrors); // it's just a guess

    cin.ignore(); // newline after mirrors
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            cin >> noskipws >> maze[i][j];
            if (maze[i][j] == Sym::Crystal) {
                crystalPositions.emplace_back(i, j);
            }
        }
        cin.ignore(); // newline after a row
    }

    return MazeDescription{maze, crystalPositions, mirrors};
}

string directionToString(size_t val) {
    switch (val) {
        case 0:
            return "Up";
        case 1:
            return "Down";
        case 2:
            return "Left";
        case 3:
            return "Right";
    }
    return "No such direction";
}

int main() {
    auto mazeDescription = readInput();
    auto shortestPaths = findShortestPaths(mazeDescription);

#ifdef LOGBUILD
    for(size_t i =0; i<shortestPaths.size(); ++i) {
        for(size_t j=0; j<shortestPaths[i].size(); ++j) {
            for(size_t k =0; k< 4; ++k) {
                for(size_t l =0; l<4; ++l) {
                    if(shortestPaths[i][j].tab[k][l].first != UINT_MAX) {
                        cout << "C1 " << i << " C2: " << j << " D1: " << directionToString(k) << " D2: " << directionToString(l) << " val: " << shortestPaths[i][j].tab[k][l].first << endl;
                    }
                }
            }
        }
    }
#endif // LOGBUILD

    auto minCostForRemainingNodes = calculateMinCostForRemainingNodes(shortestPaths, mazeDescription.crystalPositions.size());
    combinePaths(mazeDescription, shortestPaths, minCostForRemainingNodes);

    printSolution(mazeDescription);

    return 0;
}

vector<int> calculateMinCostForRemainingNodes(const vector<vector<PathCombinations>> &shortestPaths, size_t howMuch) {
    map<pair<int, int>, int> minCostForNode;
    vector<int> minCost;

    for(size_t row = 0; row < shortestPaths.size(); ++row) {
        for(size_t col = 0; col < shortestPaths[0].size(); ++col) {
            for(int i = 0; i<4; ++i) {
                for (int j = 0; j < 4; ++j) {
		            auto pr = make_pair(row, col);
                    if (shortestPaths[row][col].tab[i][j].first != UINT_MAX) {
                    try {
                        auto& val = minCostForNode.at(pr);
                        if (shortestPaths[row][col].tab[i][j].first < (size_t)val) {
                            val = shortestPaths[row][col].tab[i][j].first;
                        }
                    } catch(out_of_range&) {
                        minCostForNode[pr] = shortestPaths[row][col].tab[i][j].first;
                    }
                    }
                }
            }
        }
    }
    minCost.resize(minCostForNode.size());
    transform(minCostForNode.cbegin(), minCostForNode.cend(), minCost.begin(), [](const pair<pair<int, int>, int>& arg) {  return arg.second; });
    partial_sort(minCost.begin(), next(minCost.begin(), howMuch), minCost.end());

    minCost.resize(howMuch);
    for(size_t i = 1; i < minCost.size(); ++i) {
        minCost[i] += minCost[i-1];
    }
    return minCost;
}

bool pathFound;

void combinePaths(MazeDescription &description, const vector<vector<PathCombinations>> &pathCombinations, const vector<int>& minCostForRemainingNodes) {
    size_t lastPoint = description.crystalPositions.size(); // the starting position
    set<size_t> remainingCrystals;
    for (size_t i = 0; i < description.crystalPositions.size(); ++i) {
        remainingCrystals.insert(i);
    }

    size_t currentCost = 0;
    vector<MirrorPos> allMirrors;

    expandPath(pathCombinations, remainingCrystals, lastPoint, currentCost, Direction::Left, description.maxMirrors(), description, allMirrors, minCostForRemainingNodes);


}

int checkForReusedMirrors(const MazeDescription& description, const vector<MirrorPos>& newMirrors) {
    return count_if(newMirrors.cbegin(), newMirrors.cend(), [&description] (const MirrorPos& pos) {
        auto sym = description.maze[pos.position.row][pos.position.col];
        return sym == Sym::Mirror45 or sym == Sym::Mirror135;
    }); 
}

void expandPath(const vector<vector<PathCombinations>> &pathCombinations, const set<size_t>& remainingCrystals, size_t lastPoint,
           size_t cost,
           Direction lastDirection, unsigned maxCost, MazeDescription& description, const vector<MirrorPos>& allMirrors, const vector<int>& minCostForRemainingNodes) {
    if (remainingCrystals.empty()) {
        pathFound = true;
    }

    for (auto i: remainingCrystals) {
        for (int j = 0; j < 4; ++j) { // in directions
            auto path = pathCombinations[lastPoint][i].tab[(size_t) lastDirection][j];
            auto segmentCost = path.first;
            int reusedMirrors = checkForReusedMirrors(description, path.second);
            segmentCost -= reusedMirrors;
            // size() - 2 because we already included current crystal in segment cost
            int minCostForRemaining = remainingCrystals.size() > 1 ? minCostForRemainingNodes[remainingCrystals.size() - 2] : 0;
            if (segmentCost + cost + minCostForRemaining >  maxCost) {
                continue;
            } else {

                if(!willCollide(path.second, lastPoint, i, description)) {

                    for (const auto &mir: path.second) {
                        description.maze[mir.position.row][mir.position.col] = mir.type;
                    }

                    auto newRemaining = remainingCrystals;
                    newRemaining.erase(i);

//                auto newMirrors = allMirrors;
//                newMirrors.insert(allMirrors.end(), path.second.begin(), path.second.end());

                    expandPath(pathCombinations, newRemaining, i, cost + segmentCost, ((Direction) j), maxCost,
                               description, allMirrors, minCostForRemainingNodes);
                    if (pathFound) {
#ifdef LOGBUILD
                        cout << i << '\n';
#endif //LOGBUILD
                        //for(const auto& mir: path.second) {
//                        description.maze[mir.position.row][mir.position.col] = mir.type;
                        //}
                        return;
                    }

                    for (const auto &mir: path.second) {
                        description.maze[mir.position.row][mir.position.col] = Sym::Blank;
                    }
                }
            }
        }
    }

}



bool willCollide(const vector<MirrorPos>& newMirrors, size_t start, unsigned long end,
                 const MazeDescription &description) {

    const auto &startPos = start == description.crystalPositions.size() ? Position{1, 0} : description.crystalPositions[start];
    const auto &endPos = description.crystalPositions[end];

    for (int i = -1; i < (int)newMirrors.size(); ++i) {
        const Position *currStart = nullptr, *currEnd = nullptr;
        if (i == -1) {
            currStart = &startPos;
        } else {
            currStart = &newMirrors[i].position;
        }
        if (i + 1 == (int)newMirrors.size()) {
            currEnd = &endPos;
        } else {
            currEnd = &newMirrors[i + 1].position;
        }

        if (currStart->row == currEnd->row) {
            if (currStart->col > currEnd->col) {
                for (size_t k = currStart->col - 1; k > currEnd->col; --k) {
                    if (description.maze[currStart->row][k] != Sym::Blank and description.maze[currStart->row][k] != Sym::Crystal)
                        return true;
                }
            } else {
                for (size_t k = currStart->col + 1; k < currEnd->col; ++k) {
                    if (description.maze[currStart->row][k] != Sym::Blank and description.maze[currStart->row][k] != Sym::Crystal)
                        return true;
                }
            }
        } else {
            if (currStart->row > currEnd->row) {
                for (size_t k = currStart->row - 1; k > currEnd->row; --k) {
                    if (description.maze[k][currStart->col] != Sym::Blank and description.maze[k][currStart->col] != Sym::Crystal)
                        return true;
                }
            } else {
                for (size_t k = currStart->row + 1; k < currEnd->row; ++k) {
                    if (description.maze[k][currStart->col] != Sym::Blank and description.maze[k][currStart->col] != Sym::Crystal)
                        return true;
                }
            }
        }
    }
    return false;
}
