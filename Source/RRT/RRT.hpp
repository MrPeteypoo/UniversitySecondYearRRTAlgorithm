#ifndef GEC_RRT_HPP
#define GEC_RRT_HPP


// STL headers.
#include <memory>


// Application headers.
#include <RRT/Tree.hpp>


// External headers.
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>


// Forward declarations and aliases.
class LevelData;
enum class TileType : char;
using RRTTree = Tree<sf::Vector2i>;


/// <summary>
/// A class which creates an RRT tree when given a map and a start and end point.
/// </summary>
class RRT final
{
    public:

        /////////////////////////////////
        // Constructors and destructor //
        /////////////////////////////////

        /// <summary> Construct an RRT object with the required algorithm parameters. </summary>
        /// <param name="sampleDistance"> The incrementation to use when sampling between branches during collision detection. </param>
        /// <param name="branchDistance"> The maximum distance of a branch. </param>
        RRT (const float sampleDistance = 0.25f, const float branchDistance = 15.f);
        
        RRT (RRT&& move);
        RRT& operator= (RRT&& move);

        RRT (const RRT& copy)               = default;
        RRT& operator= (const RRT& copy)    = default;
        ~RRT()                              = default;


        /////////////
        // Getters //
        /////////////

        /// <summary> Obtains the starting point of the RRT algorithm. </summary>
        const sf::Vector2i& getStart() const    { return m_start; }

        /// <summary> Obtains the end point of the RRT algorithm. </summary>
        const sf::Vector2i& getEnd() const      { return m_end; }


        ///////////////
        // Rendering //
        ///////////////

        /// <summary> Draws the entire RRT tree to the given target. </summary>
        /// <param name="drawTo"> The target to draw the tree to. </param>
        /// <param name="scale"> The scalar to used when determining pixel position. </param>
        void draw (sf::RenderTarget& drawTo, const sf::Vector2f& scale);


        /////////////////////
        // Tree management //
        /////////////////////

        /// <summary> Determines if the goal has been reached. </summary>
        bool hasFinished() const;
        
        /// <summary> Prepares the RRT algorithm for generating nodes. </summary>
        /// <param name="data"> The data to create a tree from. </param>
        /// <param name="start"> The start point of the RRT algorithm. </param>
        /// <param name="end"> The end point of the RRT algorithm. </param>
        void prepareTree (const std::shared_ptr<LevelData>& data, const sf::Vector2i& start, const sf::Vector2i& end);

        /// <summary> Causes the algorithm to produce an extra branch if it hasn't already reached the goal. </summary>
        void generateBranch();

        /// <summary> Checks if the tile at the given position is valid according to the base type. </summary>
        /// <param name="position"> The position to check. </param>
        /// <param name="base"> The base tile, this impacts whether the tile is valid. TileType::OutOfBounds allows any tile. </param>
        /// <returns> A TileType::Water base can only travel on water, the rest can travel on land only. </returns>
        bool isValidTile (const sf::Vector2i& position, const TileType base) const;

    private:

        /// <summary> Determines the Branch closest to the given position. </summary>
        /// <param name="position"> The position to check for. </param>
        /// <returns> The closest Branch. </returns>
        RRTTree::Branch determineNearest (const sf::Vector2i& position) const;

        /// <summary> Calculates a new branch between the start and end point by sampling and checking for collision. </summary>
        /// <param name="start"> The position to start from. </param>
        /// <param name="end"> The target position. </param>
        /// <returns> A new branch, this will be a nullptr if a branch couldn't be generated. </returns>
        RRTTree::Branch calculateBranch (const sf::Vector2i& start, const sf::Vector2i& end) const;
        

        ///////////////////
        // Internal data //
        ///////////////////

        std::shared_ptr<LevelData>      m_data              { };    //!< A pointer to the LevelData which the Tree will be generated with.
        sf::Vector2i                    m_start             { };    //!< The start point of the RRT algorithm.
        sf::Vector2i                    m_end               { };    //!< The end point of the RRT algorithm.

        float                           m_sampleDistance    { 0 };  //!< How much to increment by when sampling the distance.
        float                           m_branchDistance    { 0 };  //!< The maximum distance of a branch.

        std::vector<RRTTree::Branch>    m_nodes             { };    //!< A collection of pointers to each branch in tile order.
        std::shared_ptr<RRTTree>        m_tree              { };    //!< The tree containing each node and its branches.
};

#endif