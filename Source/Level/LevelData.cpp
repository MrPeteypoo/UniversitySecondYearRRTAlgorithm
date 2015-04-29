#include "LevelData.hpp"


// STL headers.
#include <cassert>
#include <fstream>
#include <stdexcept>
#include <utility>



//////////////////
// Constructors //
//////////////////

LevelData::LevelData (const std::string& file)
{
    loadFromFile (file);
}


LevelData::LevelData (LevelData&& move)
{
    *this = std::move (move);
}


LevelData& LevelData::operator= (LevelData&& move)
{
    if (this != &move)
    {
        // Move thy data bruv.
        m_width     = move.m_width;
        m_height    = move.m_height;

        m_mapFile   = std::move (move.m_mapFile);
        m_tileData  = std::move (move.m_tileData);

        // Reset primitives.
        move.m_width    = 0;
        move.m_height   = 0;
    }

    return *this;
}


/////////////////////////
// Getters and setters //
/////////////////////////

TileType LevelData::getTile (const unsigned int index) const
{
    // Pre-condition: The index must be valid.
    assert (index < getTileCount());

    return m_tileData[index];
}


TileType LevelData::getTile (const unsigned int x, const unsigned int y) const
{
    // Pre-condition: The X and Y don't exceed the width or height.
    assert (x < m_width && y < m_height);

    return m_tileData[x + y * m_width];
}


void LevelData::loadFromFile (const std::string& file)
{
    // Create the input stream we'll be using.
    auto stream = std::ifstream (file);

    // Ensure the file is valid.
    if (!stream)
    {
        throw std::invalid_argument ("LevelData::loadFromFile(), file location given is invalid. \"" + file + "\".");
    }

    // Keep the file location up to date.
    m_mapFile = file;

    // Now read in the header and level data.
    readHeader (stream);
    readLevel (stream);

    // Close the stream since we no longer need it.
    stream.close();
}


////////////////////
// Implementation //
////////////////////

void LevelData::readHeader (std::ifstream& stream)
{
    /// The header is in the following format:
    /// "type blah", ignore this.
    /// "height blah", gives us the height value.
    /// "width blah", gives us the width value.
    /// "map", ignore this.

    // Pre-condition: The stream is open.
    assert (stream.is_open());

    // Lets make an ignore string, reserve some storage space to speed the process up.
    std::string ignore = "";
    ignore.reserve (256);

    if (!std::getline (stream, ignore) ||               // Ignore the first line.
        
        !(stream >> ignore) || !(stream >> m_height) || // Ignore "height" and obtain the height value.
        
        !(stream >> ignore) || !(stream >> m_width) ||  // Ignore "width" and obtain the width value.
        
        !std::getline (stream, ignore) ||               // Move to the next line.
        
        !std::getline (stream, ignore))                 // Ignore the last line.
    {
        throw std::runtime_error ("LevelData::readHeader(), given file didn't have a valid header.");
    }

    // Now test the width and height values are valid.
    if (m_width  == 0 || m_width  >= (unsigned int) std::numeric_limits<int>::max() || 
        m_height == 0 || m_height >= (unsigned int) std::numeric_limits<int>::max())
    {
        throw std::runtime_error ("LevelData::readHeader(), width and height values stored in the loaded data is invalid.");
    }

    // Clear the current data and reserve enough space.
    m_tileData.clear();
    m_tileData.shrink_to_fit();
    m_tileData.reserve (m_width * m_height);
}


void LevelData::readLevel (std::ifstream& stream)
{
    // Pre-condition: the stream is open.
    assert (stream.is_open());

    // We'll need to read in a character at a time.
    char tile { 0 };

    // Loop until we're at the end of the file.
    while (stream >> tile)
    {
        m_tileData.push_back (determineTileType (tile));
    }

    // If the file isn't valid then the size of the vector then the file is invalid.
    if (m_tileData.size() != m_tileData.capacity())
    {
        throw std::runtime_error ("LevelData::readLevel(), given file contains an invalid amount of tiles for the specified width * height.");
    }
}


TileType LevelData::determineTileType (const char tile) const
{
    // The characters are as follows:
    // '.' || 'G'   == Terrain.
    // '@' || 'O'   == Out of bounds.
    // 'T'          == Tree.
    // 'S'          == Swamp.
    // 'W'          == Water.
    switch (tile)
    {
        case '.':
        case 'G':
            return TileType::Terrain;

        case '@':
        case 'O':
            return TileType::OutOfBounds;

        case 'T':
            return TileType::Tree;

        case 'S':
            return TileType::Swamp;

        case 'W':
            return TileType::Water;

        default:
            throw std::invalid_argument ("LevelData::determineTileType(), invalid character given. \"" + std::to_string (tile) + "\"");
    }
}