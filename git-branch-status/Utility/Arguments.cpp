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
 * @file        Arguments.hpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#include "Arguments.hpp"
#include <exception>

namespace Utility
{
    class Arguments::IMPL
    {
        public:
            
            IMPL( int argc, char * argv[] );
            IMPL( const IMPL & o );
            ~IMPL( void );
            
            bool        _help;
            bool        _fetchOrigin;
            std::string _path;
            std::string _keychainItem;
    };
    
    static Arguments * instance = nullptr;
    
    Arguments & Arguments::sharedInstance( void )
    {
        if( instance == nullptr )
        {
            throw std::runtime_error( "Shared instance not available" );
        }
        
        return *( instance );
    }
    
    Arguments::Arguments( int argc, char * argv[] ): impl( std::make_shared< IMPL >( argc, argv ) )
    {
        if( instance == nullptr )
        {
            instance = new Arguments( *( this ) );
        }
    }

    Arguments::Arguments( const Arguments & o ): impl( std::make_shared< IMPL >( *( o.impl ) ) )
    {}

    Arguments::~Arguments( void )
    {}

    Arguments & Arguments::operator =( Arguments o )
    {
        swap( *( this ), o );
        
        return *( this );
    }

    bool Arguments::help( void ) const
    {
        return this->impl->_help;
    }

    bool Arguments::fetchOrigin( void ) const
    {
        return this->impl->_fetchOrigin;
    }

    std::string Arguments::path( void ) const
    {
        return this->impl->_path;
    }

    std::string Arguments::keychainItem( void ) const
    {
        return this->impl->_keychainItem;
    }

    void swap( Arguments & o1, Arguments & o2 )
    {
        using std::swap;
        
        swap( o1.impl, o2.impl );
    }

    Arguments::IMPL::IMPL( int argc, char * argv[] ):
        _help( false ),
        _fetchOrigin( false )
    {
        for( int i = 1; i < argc; i++ )
        {
            if( std::string( argv[ i ] ) == "--help" || std::string( argv[ i ] ) == "-h" )
            {
                this->_help = true;
            }
            else if( std::string( argv[ i ] ) == "--fetch-origin" )
            {
                this->_fetchOrigin = true;
            }
            else if( std::string( argv[ i ] ) == "--keychain-item" )
            {
                if( i + 1 < argc )
                {
                    this->_keychainItem = argv[ ++i ];
                }
            }
            else
            {
                this->_path = argv[ i ];
                
                break;
            }
        }
    }

    Arguments::IMPL::IMPL( const IMPL & o ):
        _help( o._help ),
        _fetchOrigin( o._fetchOrigin ),
        _path( o._path ),
        _keychainItem( o._keychainItem )
    {}

    Arguments::IMPL::~IMPL( void )
    {}
}
