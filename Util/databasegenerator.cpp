#include "databasegenerator.h"

void DatabaseGenerator::generate(const Goal& goal, Database& database, const Rubiks& baseCube)
{
    Rubiks  cube  = baseCube;
    uint8_t depth = 0;
    Timer   timer;

    timer.set();

    while (!database.full())
    {
        std::cout << "Depth: " << (int)depth << ". ";
        if (databaseSearcher(cube, Rubiks::EMOVE::NO_MOVE, goal, database, 0, depth))
        {
            break;
        }

        std::cout << "visited states: " << database.currentSize() << " / " << database.size() << std::endl;
        depth++;
    }

    // all states visited, write the result to a file
    database.write();
    std::cout << "Finished generating (" << timer.get() / 1000 << " seconds)" << std::endl;
}

bool DatabaseGenerator::databaseSearcher(Rubiks cube, Rubiks::EMOVE lastMove, const Goal& goal, Database& database, uint8_t depth, uint8_t maxDepth) const
{
    // all states visited
    if (database.full())
    {
        return true;
    }

    uint32_t index = database.getInd(cube);
    
    // IDDFS looks at nodes multiple times, so indexing should only
    // be done at leaf level since lower depths have already been visited
    if (depth == maxDepth)
    {
        database.set(index, depth);
    }
    else
    {
        // prune a branch if a state has been visited at an earlier depth
        if (depth <= database[index])
        {
            for (const auto move : goal.legalMoves)
            {
                if (!searchUtil.isRedundant(move, lastMove))
                {
                    cube.performMove(move);

                    if (databaseSearcher(cube, move, goal, database, depth + 1, maxDepth))
                    {
                        return true;
                    }

                    cube.revertMove(move);
                }
            }
        }
    }

    // branch led to a dead end
    return false;
}
