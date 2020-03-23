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
#include <iomanip>
#include <sstream>
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
                    Git::Repository                  repos( ( args.path().length() > 0 ) ? args.path() : "." );
                    Utility::Optional< Git::Branch > head( repos.head() );
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
                    
                    if( head.hasValue() == false )
                    {
                        throw std::runtime_error( "Cannot get head" );
                    }
                    
                    printBranchInfo( *( head ), repos, screen, y++ );
                    
                    {
                        std::vector< Git::Branch > branches( repos.branches()  );
                        
                        std::sort
                        (
                            std::begin( branches ),
                            std::end( branches ),
                            [ & ]( const Git::Branch & b1, const Git::Branch & b2 )
                            {
                                if( b1.name() == "origin/" + head->name() )
                                {
                                    return true;
                                }
                                
                                return b1.name() < b2.name();
                            }
                        );
                        
                        for( const auto & branch: branches )
                        {
                            if( branch == head )
                            {
                                continue;
                            }
                            
                            printBranchInfo( branch, repos, screen, y++ );
                        }
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
    
    if( y >= screen.height() )
    {
        return;
    }
    
    ::move( y, 0 );
    
    if( screen.supportsColors() )
    {
        ::init_pair( 1, COLOR_GREEN,   COLOR_BLACK );
        ::init_pair( 2, COLOR_GREEN,   COLOR_BLACK );
        ::init_pair( 3, COLOR_BLUE,    COLOR_BLACK );
        ::init_pair( 4, COLOR_RED,     COLOR_BLACK );
        ::init_pair( 5, COLOR_MAGENTA, COLOR_BLACK );
        ::init_pair( 6, COLOR_YELLOW,  COLOR_BLACK );
        ::init_pair( 7, COLOR_CYAN,    COLOR_BLACK );
        ::init_pair( 8, COLOR_WHITE,   COLOR_BLACK );
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
            symbol = "%";
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
        else if( branch.lastCommit().hasValue() && repos.head()->lastCommit().hasValue() && branch.lastCommit()->hash() == repos.head()->lastCommit()->hash() )
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
    
    ::attron( attr );
    
    {
        std::string info( symbol + " " + branch.name() );
        
        if( info.length() > screen.width() )
        {
            info = info.substr( 0, screen.width() );
        }
        
        if( branch == repos.head() )
        {
            ::printw( "%s", info.c_str() );
        }
        else
        {
            ::printw( "  %s", info.c_str() );
        }
    }
    
    ::attroff( attr );
    
    {
        std::vector< std::pair< std::string, unsigned long long > > info;
        size_t                                                      longestBranch( 0 );
        size_t                                                      longestAuthor( 0 );
        
        for( const auto & b: repos.branches() )
        {
            if( b.name().size() > longestBranch )
            {
                longestBranch = b.name().size();
            }
            
            if( b.lastCommit().hasValue() )
            {
                {
                    std::string author;
                    
                    if( b.lastCommit()->author().hasValue() )
                    {
                        author = b.lastCommit()->author()->name();
                    }
                    else if( b.lastCommit()->committer().hasValue() )
                    {
                        author = b.lastCommit()->committer()->name();
                    }
                    
                    if( author.size() > longestAuthor )
                    {
                        longestAuthor = author.size();
                    }
                }
            }
        }
        
        longestBranch += 4;
        
        if( branch.lastCommit().hasValue() )
        {
            info.push_back( { branch.lastCommit()->hash( 8 ), COLOR_PAIR( 6 ) } );
            
            if( branch.lastCommit()->time() > 0 )
            {
                {
                    std::tm           tm;
                    std::stringstream ss;
                    time_t            t( branch.lastCommit()->time() );
                    
                    memset( &tm, 0, sizeof( std::tm ) );
                    localtime_r( &t, &tm );
                    
                    ss << std::put_time( &tm, "%x %X" );
                    
                    if( ss.str().length() > 0 )
                    {
                        info.push_back( { ss.str(), COLOR_PAIR( 7 ) } );
                    }
                }
            }
            
            {
                std::stringstream ss;
                
                ss << std::setw( static_cast< int >( longestAuthor ) );
                
                if( branch.lastCommit()->author().hasValue() )
                {
                    ss << branch.lastCommit()->author()->name();
                }
                else if( branch.lastCommit()->committer().hasValue() )
                {
                    ss << branch.lastCommit()->committer()->name();
                }
                
                info.push_back( { ss.str(), COLOR_PAIR( 8 ) } );
            }
            
            {
                std::string message( branch.lastCommit()->message() );
                
                if( message.find( "\n" ) != std::string::npos )
                {
                    message = message.substr( 0, message.find( "\n" ) );
                }
                
                info.push_back( { message, COLOR_PAIR( 6 ) } );
            }
        }
        
        {
            int x( static_cast< int >( longestBranch ) );
            
            for( const auto & p: info )
            {
                if( x + p.first.length() >= screen.width() )
                {
                    break;
                }
                
                ::move( y, x );
                ::attron( p.second );
                ::printw( " %s", p.first.c_str() );
                ::attroff( p.second );
                
                x += p.first.length() + 1;
            }
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
