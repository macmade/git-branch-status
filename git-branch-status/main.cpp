/*******************************************************************************
 * The MIT License (MIT)
 * 
 * Copyright (c) 2018 Jean-David Gadina - www-xs-labs.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/*!
 * @file        main.cpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#include <stdexcept>
#include <iostream>
#include <ncurses.h>
#include "Arguments.hpp"
#include "Git/Repository.hpp"
#include "UI/Screen.hpp"

static void printBranchInfo( const Git::Branch & branch, const Git::Repository & repos, const UI::Screen & screen, unsigned int y );
static void showHelp( void );

int main( int argc, char * argv[] )
{
    Utility::Arguments args( argc, argv );
    
    if( args.help() )
    {
        showHelp();
        
        return EXIT_SUCCESS;
    }
    
    {
        UI::Screen screen;
        
        screen.onKeyPress
        (
            [ & ]( const UI::Screen & s, int key )
            {
                if( key == 'q' )
                {
                    screen.stop();
                }
            }
        );
        
        screen.onUpdate
        (
            [ & ]( const UI::Screen & s )
            {
                try
                {
                    Git::Repository              repos( ( args.path().length() > 0 ) ? args.path() : "." );
                    std::optional< Git::Branch > head( repos.head() );
                    int                          y( 0 );
                    
                    if( args.fetchOrigin() )
                    {
                        for( const auto & remote: repos.remotes() )
                        {
                            if( remote.name() == "origin" )
                            {
                                remote.fetch();
                            }
                        }
                    }
                    
                    if( head.has_value() == false )
                    {
                        throw std::runtime_error( "Cannot get head" );
                    }
                    
                    printBranchInfo( *( head ), repos, screen, y++ );
                    
                    for( const auto & branch: repos.branches() )
                    {
                        if( branch == head )
                        {
                            continue;
                        }
                        
                        printBranchInfo( branch, repos, screen, y++ );
                    }
                }
                catch( const std::exception & e )
                {
                    ::clear();
                    ::move( 0, 0 );
                    ::printw( ( std::string( "Error: " ) + e.what() ).c_str() );
                }
            }
        );
        
        screen.start();
    }
    
    return EXIT_SUCCESS;
}

void printBranchInfo( const Git::Branch & branch, const Git::Repository & repos, const UI::Screen & screen, unsigned int y )
{
    std::string        symbol;
    unsigned long long attr( 0 );
    
    if( screen.width() < 10 )
    {
        return;
    }
    
    ::move( y, 0 );
    
    if( screen.supportsColors() )
    {
        ::init_pair( 1, COLOR_WHITE,   COLOR_GREEN );
        ::init_pair( 2, COLOR_GREEN,   COLOR_BLACK );
        ::init_pair( 3, COLOR_BLUE,    COLOR_BLACK );
        ::init_pair( 4, COLOR_RED,     COLOR_BLACK );
        ::init_pair( 5, COLOR_MAGENTA, COLOR_BLACK );
        
        if( branch == repos.head() )
        {
            ::attron( COLOR_PAIR( 1 ) );
            ::hline( ' ', static_cast< int >( screen.width() ) );
            ::attroff( COLOR_PAIR( 1 ) );
            ::move( y, 0 );
        }
    }
    
    if( branch == repos.head() )
    {
        ::attron( COLOR_PAIR( 1 ) );
        
        symbol = "@";
    }
    else
    {
        if( repos.head()->isAhead( branch ) && repos.head()->isBehind( branch ) )
        {
            symbol = "%%";
            attr   = COLOR_PAIR( 5 );
        }
        else if( repos.head()->isAhead( branch ) )
        {
            symbol = ">";
            attr   = COLOR_PAIR( 3 );
        }
        else if( repos.head()->isBehind( branch ) )
        {
            symbol = "<";
            attr   = COLOR_PAIR( 4 );
        }
        else if( branch.lastCommit().has_value() && repos.head()->lastCommit().has_value() && branch.lastCommit()->hash() == repos.head()->lastCommit()->hash() )
        {
            symbol = "=";
            attr   = COLOR_PAIR( 2 );
        }
        else
        {
            symbol = "?";
            attr   = COLOR_PAIR( 5 );
        }
    }
    
    if( branch != repos.head() )
    {
        ::attron( attr );
    }
    
    {
        std::string info( symbol + " " + branch.name() );
        
        if( info.length() > screen.width() )
        {
            info = info.substr( 0, screen.width() );
        }
        
        ::printw( "%s", info.c_str() );
    }
    
    if( branch != repos.head() )
    {
        ::attroff( attr );
    }
    
    {
        std::vector< std::string > info;
        size_t                     longestName( 0 );
        
        for( const auto & b: repos.branches() )
        {
            if( b.name().size() > longestName )
            {
                longestName = b.name().size();
            }
        }
        
        longestName += 2;
        
        if( branch.lastCommit().has_value() )
        {
            info.push_back( branch.lastCommit()->hash() );
        }
        
        if( info.size() > 0 )
        {
            ::move( y, static_cast< int >( longestName ) + 1 );
            ::printw( info[ 0 ].c_str() );
        }
    }
    
    ::attroff( COLOR_PAIR( 1 ) );
    ::attroff( COLOR_PAIR( 2 ) );
    ::attroff( COLOR_PAIR( 3 ) );
    ::attroff( COLOR_PAIR( 4 ) );
    ::attroff( COLOR_PAIR( 5 ) );
}

static void showHelp( void )
{
    std::cout << "Usage: git-branch-status [OPTIONS] [PATH]"
              << std::endl
              << std::endl
              << "Options:"
              << std::endl
              << std::endl
              << "    --help             Shows this help dialog"
              << std::endl
              << "    --fetch-origin     Automatically fetches changes from origin"
              << std::endl
              << "    --keychain-item    The name of a keychain item containing Git credentials"
              << std::endl;
}
