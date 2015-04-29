#include "LevelViewer.hpp"


// Application headers.
#include <Level/LevelData.hpp>


// External headers.
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>



//////////////////
// Constructors //
//////////////////

LevelViewer::LevelViewer (const LevelData& data)
{
    createView (data);
}


LevelViewer::LevelViewer (LevelViewer&& move)
{
    *this = std::move (move);
}


LevelViewer& LevelViewer::operator= (LevelViewer&& move)
{
    if (this != &move)
    {
        // Move our data.
        m_view  = std::move (move.m_view);
        m_scale = std::move (move.m_scale);
    }

    return *this;
}


/////////////
// Setters //
/////////////

void LevelViewer::createView (const LevelData& data)
{
    // Construct an image and set each tile type to a different colour.
    auto image = sf::Image();
    image.create (data.getWidth(), data.getHeight());

    for (auto y = 0U; y < data.getHeight(); ++y)
    {
        for (auto x = 0U; x < data.getWidth(); ++x)
        {
            // Determine the tile and colour of the current pixel.
            const auto tile = data.getTile (x, y);
            image.setPixel (x, y, determineColour (tile));
        }
    }

    // Update the texture with our new image.
    m_view.loadFromImage (image);
}


//////////////////////
// Game integration //
//////////////////////

sf::Color LevelViewer::determineColour (const TileType tile) const
{
    // Terrain  = green,
    // Tree     = brown,
    // Swamp    = mossy green,
    // Water    = blue,
    // OOB      = grey.
    const sf::Color green   { 1, 142, 14 },
                    brown   { 95, 80, 29 },
                    moss    { 130, 148, 71 },
                    blue    { 43, 71, 62 },
                    grey    { 40, 40, 40 };

    switch (tile)
    {
        case TileType::Terrain:
            return green;

        case TileType::Tree:
            return brown;

        case TileType::Swamp:
            return moss;

        case TileType::Water:
            return blue;
        
        case TileType::OutOfBounds:
            return grey;

        default:
            throw std::logic_error ("LevelViewer::determineColour(), TileType not implemented.");
    }
}


void LevelViewer::draw (sf::RenderTarget& drawTo)
{
    // Create a sprite from our texture and draw it.
    sf::Sprite sprite { m_view };

    sprite.setScale (m_scale);
    drawTo.draw (sprite);
}