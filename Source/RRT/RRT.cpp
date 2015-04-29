#include "RRT.hpp"


// STL headers.
#include <cassert>
#include <ctime>
#include <functional>
#include <random>
#include <utility>


// Application headers.
#include <Level/LevelData.hpp>



//////////////////
// Constructors //
//////////////////

RRT::RRT (const float sampleDistance, const float branchDistance)
    : m_sampleDistance (sampleDistance), m_branchDistance (branchDistance)
{
    // Ensure we have valid values.
    assert (sampleDistance > 0.f && branchDistance >= 1.f);

    m_tree = std::make_shared<RRTTree>();
}


RRT::RRT (RRT&& move)
{
    *this = std::move (move);
}


RRT& RRT::operator= (RRT&& move)
{
    if (this != &move)
    {
        // Move the data.
        m_data              = std::move (move.m_data);
        m_start             = std::move (move.m_start);
        m_end               = std::move (move.m_end);

        m_sampleDistance    = move.m_sampleDistance;
        m_branchDistance    = move.m_branchDistance;

        m_nodes             = std::move (move.m_nodes);
        m_tree              = std::move (move.m_tree);

        // Reset primitives.
        move.m_sampleDistance = 0.f;
        move.m_branchDistance = 0.f;
    }

    return *this;
}


///////////////
// Rendering //
///////////////

void RRT::draw (sf::RenderTarget& drawTo, const sf::Vector2f& scale)
{
    // We need a draw function for each part of the tree.
    const std::function<void (const RRTTree::Branch)> draw = [&] (const RRTTree::Branch branch)
    {
        // Cache the branch count.
        const auto branches = branch->getBranchCount();

        for (auto i = 0U; i < branches; ++i)
        {
            // Perform a depth-first draw algorithm.
            const auto child = branch->getBranch(i);

            if (child)
            {
                draw (child);
            }
            
            // Only draw if we aren't a root.
            if (!branch->isRoot())
            {
                // Obtain each position.
                const auto& position = branch->getData();
                const auto& parent   = branch->getParent()->getData();

                // Create the vertices to connect the line.
                sf::Vertex line[] = 
                {  
                    { sf::Vector2f (position.x * scale.x, position.y * scale.y) },
                    { sf::Vector2f (parent.x * scale.x, parent.y * scale.y) }
                };

                // Finally draw the line.
                drawTo.draw (line, 2, sf::Lines);
            }
        }
    };

    draw (m_tree.get());
}


/////////////////////
// Tree management //
/////////////////////

bool RRT::hasFinished() const
{
    // Obtain the width.
    const auto width = m_data->getWidth();

    // Determine the start and end nodes.
    const auto start = m_start.x + m_start.y * width,
               end   = m_end.x + m_end.y * width;

    // Ensure that both values have a valid pointer, if so then we have finished.
    return m_nodes[start] && m_nodes[end];
}


void RRT::prepareTree (const std::shared_ptr<LevelData>& data, const sf::Vector2i& start, const sf::Vector2i& end)
{
    // Pre-condition: The start and end values are valid.
    assert (start.x >= 0 && start.x < (int) data->getWidth() && end.x >= 0 && end.y < (int) data->getHeight());
    
    // Reset the node pointers and the tree itself.
    m_nodes.clear();
    m_tree->clear();
    
    m_data = data;
    m_nodes.resize (m_data->getTileCount());
    m_nodes.shrink_to_fit();
    
    // Assign the new start and end point.
    m_start = start;
    m_end   = end;
    m_tree.reset (new RRTTree ());
    m_tree->setData (m_start);
    m_nodes[start.x + start.y * data->getWidth()] = m_tree.get();

    // Reseed rand().
    srand ((unsigned int) time (0));
}


void RRT::generateBranch()
{
    // Don't bother if we've already finished.
    if (!hasFinished() && m_start != m_end)
    {
        // Cache the width and height values of the level data.
        const auto width  = m_data->getWidth(),
                   height = m_data->getHeight();

        // Calculate the nearest node to a generated random point if the random point is valid.
        const auto random  = sf::Vector2i (rand() % width, rand() % height);

        if (!m_nodes[random.x + random.y * width])
        {
            const auto nearest = determineNearest (random);
        
            // Obtain the data of the nearest branch and calculate new branch.
            const auto& nearData = nearest->getData();
            const auto  branch   = calculateBranch (nearData, random);

            // An invalid branch will be returned in a new branch couldn't be generated.
            if (branch)
            {
                // Cache the new data.
                const auto& newData  = branch->getData();
                const auto  index    = newData.x + newData.y * width;

                // Don't overwrite any nodes.
                if (!m_nodes[index])
                {
                    // Add it to the tree.
                    nearest->addBranch (branch);
                    m_nodes[index] = branch;
                }

                else
                {
                    delete branch;
                }
            }
        }
    }
}


bool RRT::isValidTile (const sf::Vector2i& position, const TileType base) const
{
    // Obtain the type of the given tile.
    const auto type = m_data->getTile ((unsigned int) position.x, (unsigned int) position.y);

    switch (base)
    {
        case TileType::OutOfBounds:
        case TileType::Tree:
            return type != TileType::OutOfBounds && type != TileType::Tree;

        case TileType::Water:
            return type == TileType::Water;

        default:
            return type == TileType::Terrain || type == TileType::Swamp;
    }
}


RRTTree::Branch RRT::determineNearest (const sf::Vector2i& position) const
{
    // Set some unlikely values as the starting points.
    auto closest      = m_tree.get();
    auto nearDistance = std::numeric_limits<int>::max();

    for (const auto branch : m_nodes)
    {
        // Ensure the branch actually exists.
        if (branch)
        {
            // Cache the data of the current branch.
            const auto data       = branch->getData(),
                       difference = data - position;

            // Treat each value as an entire tile to traverse.
            const auto distance = (std::abs (difference.x) + std::abs (difference.y));

            if (distance < nearDistance)
            {
                closest      = branch;
                nearDistance = distance;
            }
        }
    }

    return closest;
}


RRTTree::Branch RRT::calculateBranch (const sf::Vector2i& start, const sf::Vector2i& end) const
{
    // We'll need to lerp between vectors.
    const auto& lerp = [] (const sf::Vector2i& start, const sf::Vector2i& end, const float delta)
    {
        return sf::Vector2i ((sf::Vector2f) start + (sf::Vector2f) (end - start) * delta);
    };

    // Firstly we need to type of the current terrain to test where we can go.
    const auto startType = m_data->getTile ((unsigned int) start.x, (unsigned int) start.y);

    // We need the magnitude between the vectors so we can start sampling the distance.
    const auto difference = end - start;
    const auto magnitude  = std::sqrt ((float) (difference.x * difference.x + difference.y * difference.y));

    // We're going to sample at different points to test we can move to the desired end point.
    RRTTree::Branch branch  = nullptr;
    auto            current = 0.f;
    auto            valid   = start;

    while (current < magnitude && current < m_branchDistance)
    {
        // Increment the current sample.
        current = std::fmin (std::fmin (current + m_sampleDistance, magnitude), m_branchDistance);
        
        // Check if the current position is valid.
        const auto inc = lerp (start, end, current / magnitude);

        if (inc != start)
        {
            // We can break early if we've hit an unpassable bit of terrain.
            if (isValidTile (inc, startType))
            {
                // Ensure we allocate some memory to store the crap.
                if (!branch)
                {
                    branch = new RRTTree();
                }
            
                branch->setData (inc);        
            }

            else
            {
                break;
            }
        }
    }

    // Return the calculated branch.
    return branch;
}