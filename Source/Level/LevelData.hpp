#ifndef GEC_LEVEL_DATA_HPP
#define GEC_LEVEL_DATA_HPP


// STL headers.
#include <iosfwd>
#include <string>
#include <vector>


/// <summary>
/// An enum containing a representation of each tile type.
/// </summary>
enum class TileType : char
{
    Terrain,        //!< Normal passable terrain.
    OutOfBounds,    //!< Unpassable terrain.
    Tree,           //!< A tree, not passable.
    Swamp,          //!< A passable swamp.
    Water           //!< Terrain that is only traversable via sea or the skies.
};


/// <summary>
/// Represents a loaded level, this contains the dimensions and tiles of a level which can be used for AI algorithms.
/// </summary>
class LevelData final
{
    public:

        /////////////////////////////////
        // Constructors and destructor //
        /////////////////////////////////

        /// <summary> Constructs a LevelData object from the given file containing level information. Exceptions can be thrown. </summary>
        /// <param name="file"> The file to load from. </param>
        LevelData (const std::string& file);
        
        LevelData (LevelData&& move);
        LevelData& operator= (LevelData&& move);

        LevelData (const LevelData& copy)               = default;
        LevelData& operator= (const LevelData& copy)    = default;
        ~LevelData()                                    = default;


        /////////////////////////
        // Getters and setters //
        /////////////////////////

        /// <summary> Gets the tile width of the level. </summary>
        unsigned int getWidth() const               { return m_width; }

        /// <summary> Gets the tile height of the level. </summary>
        unsigned int getHeight() const              { return m_height; }

        /// <summary> Gets the total number of loaded tiles in the level. </summary>
        unsigned int getTileCount() const           { return m_tileData.size(); }

        /// <summary> Gets the file location of the loaded level data. </summary>
        const std::string& getFileLocation() const  { return m_mapFile; }

        /// <summary> Obtains the type for the given tile. </summary>
        /// <param name="index"> The index of the tile. </param>
        TileType getTile (const unsigned int index) const;

        /// <summary> Obtains the type for the tile at the given co-ordinate. </summary>
        /// <param name="x"> The X co-ordinate of the tile. </param>
        /// <param name="y"> The Y co-ordinate of the tile. </param>
        TileType getTile (const unsigned int x, const unsigned int y) const;

        /// <summary> Load level data from a file at the given location. If an error occurs an exception will be thrown. </summary>
        /// <param name="file"> The file location to load from. </param>
        void loadFromFile (const std::string& file);

    private:

        ////////////////////
        // Implementation //
        ////////////////////

        /// <summary> Reads the header of a map file and prepares the tile data accordingly. Throws exceptions if the header is invalid. </summary>
        /// <param name="stream"> An open file stream to read from. </param>
        void readHeader (std::ifstream& stream);

        /// <summary> Reads the tile data of a level from a given stream. Throws exceptions upon errors. </summary>
        /// <param name="stream"> An open file stream to read from. </param>
        void readLevel (std::ifstream& stream);

        /// <summary> Determines the TileType which corresponds to the given character. </summary>
        /// <param name="tile"> The character which represents a tile value. </param>
        /// <returns> The correct TileType, throws an exception if the character is invalid. </returns>
        TileType determineTileType (const char tile) const;


        ///////////////////
        // Internal data //
        ///////////////////

        unsigned int            m_width     { 0 };  //!< The number of tiles that make up the level width.
        unsigned int            m_height    { 0 };  //!< The number of tiles that make up the level height.

        std::string             m_mapFile   = "";   //!< The file location where the level data was loaded from.
        std::vector<TileType>   m_tileData  { };    //!< The type of every tile on the level.
};

#endif