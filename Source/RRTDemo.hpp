#ifndef GEC_RRT_DEMO_HPP
#define GEC_RRT_DEMO_HPP


// Memory leak detection.
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


// STL headers.
#include <iostream>
#include <memory>
#include <string>


// External headers.
#include <SFML/System/Vector2.hpp>


// Forward declarations.
namespace sf { class RenderWindow; }
class LevelData;
class LevelViewer;
class RRT;



/// <summary>
/// A demo application which loads in level data from a file and uses the RRT algorithm to implement
/// path finding.
/// </summary>
class RRTDemo final
{
    public:

        /////////////////////////////////
        // Constructors and destructor //
        /////////////////////////////////
        
        RRTDemo()                                   = default;
        ~RRTDemo()                                  = default;

        RRTDemo (RRTDemo&& move);
        RRTDemo& operator= (RRTDemo&& move);
        
        RRTDemo (const RRTDemo& copy)               = delete;
        RRTDemo& operator= (const RRTDemo& copy)    = delete;


        //////////////////////
        // Public interface //
        //////////////////////

        /// <summary> Runs the demo application. </summary>
        /// <returns> The exit code. </returns>
        int run();

    private:

        ////////////////////
        // Implementation //
        ////////////////////

        /// <summary> Obtain the desired file location of the level data to load. </summary>
        /// <returns> A string with the desired file location. </returns>
        std::string obtainDataFile();

        /// <summary> Stretches the LevelViewer to take up the entire window and resizes the window. </summary>
        void scaleView();

        /// <summary> Enters a basic game loop. </summary>
        void gameLoop();

        /// <summary> Process all window events. </summary>
        /// <returns> Whether the application should exit. </returns>
        bool pollEvents();

        /// <summary> Handles mouse input. </summary>
        void handleInput();


        ///////////////////
        // Internal data //
        ///////////////////

        std::shared_ptr<LevelData>          m_data      { nullptr };    //!< The level data.
        std::unique_ptr<LevelViewer>        m_viewer    { nullptr };    //!< The visual representation of the data.
        std::unique_ptr<sf::RenderWindow>   m_window    { nullptr };    //!< The window displaying the GUI of the application.
        std::unique_ptr<RRT>                m_rrt       { };            //!< An RRT object which will create a Tree based on the LevelData.

        const unsigned int                  m_width     { 1600 };       //!< The maximum number of pixels wide the window can be.
        const unsigned int                  m_height    { 900 };        //!< The maximum number of pixels tall the window can be.

        sf::Vector2i                        m_start     { };            //!< The start point of the RRT.
        sf::Vector2i                        m_end       { };            //!< The end point for the RRT.
};


/// <summary> 
/// The main function which starts the application. 
/// </summary>
/// <returns> The exit code of the application. </returns>
int main()
{
    // Enable memory leak detection.
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

    // Create the application.
    const auto application = std::make_unique<RRTDemo>();

    // Run the application.
    const auto code = application->run();
    
    // Wait for input to close.
    std::cout << std::endl << "Press the Enter key to close." << std::endl;
    
    std::string bob = "";
    std::getline (std::cin, bob);
}

#endif