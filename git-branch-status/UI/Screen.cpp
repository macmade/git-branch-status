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
 * @file        Screen.cpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#include "Screen.hpp"
#include <algorithm>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <mutex>
#include <poll.h>
#include <condition_variable>

namespace UI
{
    class Screen::IMPL
    {
        public:
            
            IMPL( void );
            IMPL( const IMPL & o );
            
            std::vector< std::function< void( const Screen & screen ) > >      _onResize;
            std::vector< std::function< void( const Screen & screen, int ) > > _onKeyPress;
            std::vector< std::function< void( const Screen & screen ) > >      _onUpdate;
            
            std::atomic< std::size_t > _width;
            std::atomic< std::size_t > _height;
            bool                       _colors;
            std::atomic< bool >        _running;
            std::mutex                  _mtx;
    };
    
    Screen::Screen( void ):
        impl( std::make_shared< IMPL >() )
    {
        struct winsize s;
        
        ::initscr();
        
        if( ::has_colors() )
        {
            this->impl->_colors = true;
            
            ::start_color();
            ::attron( A_BOLD );
        }
        
        this->clear();
        ::noecho();
        ::cbreak();
        ::keypad( stdscr, true );
        this->refresh();
        
        ::ioctl( STDOUT_FILENO, TIOCGWINSZ, &s );
        
        this->impl->_width  = s.ws_col;
        this->impl->_height = s.ws_row;
    }
    
    Screen::Screen( const Screen & o ):
        impl( std::make_shared< IMPL >( *( o.impl ) ) )
    {}
    
    Screen::Screen( Screen && o ) noexcept:
        impl( std::move( o.impl ) )
    {}
    
    Screen::~Screen( void )
    {
        ::clrtoeol();
        refresh();
        ::endwin();
    }
    
    Screen & Screen::operator =( Screen o )
    {
        swap( *( this ), o );
        
        return *( this );
    }
    
    std::size_t Screen::width( void ) const
    {
        return this->impl->_width;
    }
    
    std::size_t Screen::height( void ) const
    {
        return this->impl->_height;
    }
    
    bool Screen::supportsColors( void ) const
    {
        return this->impl->_colors;
    }
    
    bool Screen::isRunning( void ) const
    {
        return this->impl->_running;
    }
    
    void Screen::clear( void ) const
    {
        ::clear();
    }
    
    void Screen::refresh( void ) const
    {
        ::refresh();
    }
    
    void Screen::start( void )
    {
        if( this->impl->_running )
        {
            return;
        }
        
        this->impl->_running = true;
        
        std::thread
        (
            [ & ]
            {
                while( this->impl->_running )
                {
                    this->clear();
                    
                    {
                        std::lock_guard< std::mutex > l( this->impl->_mtx );
                        
                        for( const auto & f: this->impl->_onUpdate )
                        {
                            f( *( this ) );
                        }
                    }
                    
                    this->refresh();
                    
                    std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
                }
            }
        )
        .detach();
        
        while( this->impl->_running )
        {
            struct winsize s;
            
            ::ioctl( STDOUT_FILENO, TIOCGWINSZ, &s );
            
            if( s.ws_col != this->impl->_width || s.ws_row != this->impl->_height )
            {
                this->impl->_width  = s.ws_col;
                this->impl->_height = s.ws_row;
                
                for( const auto & f: this->impl->_onResize )
                {
                    f( *( this ) );
                }
            }
            
            {
                static struct pollfd p;
                int                  c;
                
                memset( &p, 0, sizeof( p ) );
                
                p.fd      = 0;
                p.events  = POLLIN;
                p.revents = 0;
                
                if( poll( &p, 1, 0 ) > 0 )
                {
                    c = getch();
                    
                    {
                        std::lock_guard< std::mutex > l( this->impl->_mtx );
                        
                        for( const auto & f: this->impl->_onKeyPress )
                        {
                            f( *( this ), c );
                        }
                    }
                }
            }
            
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }
    }
    
    void Screen::stop( void )
    {
        this->impl->_running = false;
    }
    
    void Screen::onResize( const std::function< void( const Screen & screen ) > & f )
    {
        std::lock_guard< std::mutex > l( this->impl->_mtx );
        
        this->impl->_onResize.push_back( f );
    }
    
    void Screen::onKeyPress( const std::function< void( const Screen & screen, int key ) > & f )
    {
        std::lock_guard< std::mutex > l( this->impl->_mtx );
        
        this->impl->_onKeyPress.push_back( f );
    }
    
    void Screen::onUpdate( const std::function< void( const Screen & screen ) > & f )
    {
        std::lock_guard< std::mutex > l( this->impl->_mtx );
        
        this->impl->_onUpdate.push_back( f );
    }
    
    void swap( Screen & o1, Screen & o2 )
    {
        using std::swap;
        
        swap( o1.impl,  o2.impl );
    }
    
    Screen::IMPL::IMPL( void ):
        _width( 0 ),
        _height( 0 ),
        _colors( false ),
        _running( false )
    {}
    
    Screen::IMPL::IMPL( const IMPL & o ):
        _width( o._width.load() ),
        _height( o._height.load() ),
        _colors( o._colors ),
        _running( false )
    {}
}
