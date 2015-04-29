#include "RRTDemo.hpp"


// STL headers.
#include <utility>


// Application headers.
#include <Level/LevelData.hpp>
#include <Level/LevelViewer.hpp>
#include <RRT/RRT.hpp>


// External headers.
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>



//////////////////
// Constructors //
//////////////////

RRTDemo::RRTDemo (RRTDemo&& move)
{
    *this = std::move (move);
}


RRTDemo& RRTDemo::operator= (RRTDemo&& move)
{
    if (this != &move)
    {
        // Move thy data captain!
        m_data   = std::move (move.m_data);
        m_viewer = std::move (move.m_viewer);
        m_window = std::move (move.m_window);
        m_rrt    = std::move (move.m_rrt);
    }

    return *this;
}


//////////////////////
// Public interface //
//////////////////////

int RRTDemo::run()
{
    try
    {
        // We need to load the desired level data.
        const auto file = obtainDataFile();
        m_data = std::make_shared<LevelData> (file);

        // Prepare the RRT object.
        m_rrt = std::make_unique<RRT>();

        // Create a visual representation of the loaded level data.
        m_viewer = std::make_unique<LevelViewer> (*m_data);
        
        // Finally create a window and enter the game loop.
        m_window = std::make_unique<sf::RenderWindow>();
        scaleView();
        gameLoop();

        // We've succeeded!
        return 0;
    }

    catch (const std::exception& error)
    {
        std::cerr << "An error was caught during initialisation: " << error.what() << std::endl;   
        return 1;
    }

    catch (...)
    {
        std::cerr << "An unknown error was caught during initialisation." << std::endl;
        return 2;
    }
}


////////////////////
// Implementation //
////////////////////

std::string RRTDemo::obtainDataFile()
{
    // Inform the user to input a file location.
    std::cout << "Please input the location of the map file to load." << std::endl;

    // Take the input.
    std::string input = "";
    std::getline (std::cin, input);

    // Return the brilliant, amazing, beautiful input.
    return input;
}


void RRTDemo::scaleView()
{
    // Determine the smallest scale.
    const auto widthScale  = (float) m_width  / m_data->getWidth(),
               heightScale = (float) m_height / m_data->getHeight();

    auto width  = m_width, 
         height = m_height;

    auto scale = 0.f;

    // Calculate the desired width and height of the window.
    if (widthScale < heightScale)
    {
        // Let's resize the height.
        height -= m_height - (unsigned int) (m_data->getHeight() * widthScale);
        scale  = widthScale;
    }

    else
    {
        width -= m_width - (unsigned int) (m_data->getWidth() * heightScale);
        scale = heightScale;
    }
    
    // Resize the window and set the correct scale on the LevelViewer.
    m_window->create (sf::VideoMode (width, height), "N3053620 AI", sf::Style::Titlebar | sf::Style::Close);
    m_viewer->setScale ({ scale, scale });
}


void RRTDemo::gameLoop()
{
    // Prepare the tree initially.
    m_rrt->prepareTree (m_data, { }, { });

    // The most basic game loop in the world.
    while (!pollEvents())
    {
        // Handle input captain!
        handleInput();

        // Clear the window.
        m_window->clear();

        // Update the RRT algorithm ten times.
        m_rrt->generateBranch();        

        // Draw all objects.
        m_viewer->draw (*m_window);
        m_rrt->draw (*m_window, m_viewer->getScale());

        // Display it on the screen.
        m_window->display();
    }

    m_window->close();
}


bool RRTDemo::pollEvents()
{
    // We need to process the window events to keep the OS happy.
    bool close = false;
    sf::Event event;

    while (m_window->pollEvent (event))
    {
        // We only care about the close event.
        if (event.type == sf::Event::Closed)
        {
            close = true;
        }
    }

    // Determine if we should close.
    return close;
}


void RRTDemo::handleInput()
{
    // Obtain the mouse position.
    const auto mousePos   = sf::Mouse::getPosition (*m_window);
    const auto windowSize = (sf::Vector2i) m_window->getSize();

    // Check the mouse is on screen.
    if (mousePos.x >= 0 && mousePos.x < windowSize.x &&
        mousePos.y >= 0 && mousePos.y < windowSize.y)
    {
        // Determine the current tile.
        const auto& renderScale = m_viewer->getScale();
        const auto  tilePos     = sf::Vector2i ((int) (mousePos.x / renderScale.x), 
                                                (int) (mousePos.y / renderScale.y));

        if (m_rrt->isValidTile (tilePos, TileType::OutOfBounds))
        {
            // The left button corresponds to the start point.
            if (sf::Mouse::isButtonPressed (sf::Mouse::Button::Left))
            {
                m_start = tilePos;
            }

            // The right button corresponds to the end point.
            if (sf::Mouse::isButtonPressed (sf::Mouse::Button::Right))
            {
                m_end = tilePos;
            }

            // Reset the RRT.
            if (m_start != m_rrt->getStart() || m_end != m_rrt->getEnd())
            {
                m_rrt->prepareTree (m_data, m_start, m_end);
            }
        }
    }
}