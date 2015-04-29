#ifndef GEC_LEVEL_VIEWER_HPP
#define GEC_LEVEL_VIEWER_HPP


// External headers.
#include <SFML/Graphics/Texture.hpp>


// Forward declaration.
enum class TileType : char;
class LevelData;


/// <summary>
/// A class used to display the contents of a LevelData object on screen.
/// </summary>
class LevelViewer final
{
    public:

        /////////////////////////////////
        // Constructors and destructor //
        /////////////////////////////////

        /// <summary> Creates a LevelViewer object from the data contained in the given LevelData object. </summary>
        /// <param name="data"> Contains the data required to produce a visual representation. </param>
        LevelViewer (const LevelData& data);

        LevelViewer (LevelViewer&& move);
        LevelViewer& operator= (LevelViewer&& move);

        LevelViewer (const LevelViewer& copy)               = default;
        LevelViewer& operator= (const LevelViewer& copy)    = default;
        ~LevelViewer()                                      = default;


        /////////////////////////
        // Getters and setters //
        /////////////////////////

        /// <summary> Gets the visual representation of the given LevelData object. </summary>
        const sf::Texture& getView() const          { return m_view; }

        /// <summary> Gets the scalar used when drawing the LevelData visually. </summary>
        const sf::Vector2f getScale() const         { return m_scale; }

        /// <summary> Sets the scalar used when drawing the LevelData visually. </summary>
        /// <param name="scale"> The new scale values. </param>
        void setScale (const sf::Vector2f& scale)   { m_scale = scale; }

        /// <summary> Creates a representation for the given LevelData object. </summary>
        /// <param name="data"> The data to create a visualisation for. </param>
        void createView (const LevelData& data);

        
        //////////////////////
        // Game integration //
        //////////////////////

        /// <summary> Determines the colour to use based on the given tile. </summary>
        /// <param name="tile"> The tile to retrieve the colour for. </param>
        /// <returns> A colour value. </returns>
        sf::Color determineColour (const TileType tile) const;

        /// <summary> Causes the LevelViewer to render itself to a given target. </summary>
        /// <param name="drawTo"> The object to draw onto. </param>
        void draw (sf::RenderTarget& drawTo);

    private:

        ///////////////////
        // Internal data //
        ///////////////////

        sf::Texture     m_view  { };        //!< The visual representation of the given LevelData object.
        sf::Vector2f    m_scale { 1, 1 };   //!< The scale to use when displaying the LevelData object.
};

#endif