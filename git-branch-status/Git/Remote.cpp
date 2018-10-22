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
 * @file        Remote.cpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#include <stdexcept>
#include "Remote.hpp"
#include "Repository.hpp"
#include "Credentials.hpp"
#include "Arguments.hpp"

namespace Git
{
    class Remote::IMPL
    {
        public:
            
            IMPL( git_remote * remote, const Repository & repos );
            IMPL( const IMPL & o );
            ~IMPL( void );
            
            static int credentials( git_cred ** cred, const char * url, const char * usernameFromURL, unsigned int allowedTypes, void * payload );
            
            git_remote       * _remote;
            const Repository & _repos;
    };
    
    Remote::Remote( git_remote * remote, const Repository & repos ): impl( std::make_shared< IMPL >( remote, repos ) )
    {}
    
    Remote::Remote( const Remote & o ): impl( std::make_shared< IMPL >( *( o.impl ) ) )
    {}
    
    Remote::~Remote( void )
    {}
    
    Remote & Remote::operator =( Remote o )
    {
        swap( *( this ), o );
        
        return *( this );
    }
    
    Remote::operator git_remote * () const
    {
        return this->impl->_remote;
    }
    
    bool Remote::operator ==( const Remote & o ) const
    {
        return this->impl->_repos == o.impl->_repos && this->name() == o.name() && this->url() == o.url();
    }
    
    bool Remote::operator !=( const Remote & o ) const
    {
        return !operator ==( o );
    }
    
    std::string Remote::name( void ) const
    {
        const char * s( git_remote_name( this->impl->_remote ) );
        
        return ( s == nullptr ) ? "" : s;
    }
    
    std::string Remote::url(  void ) const
    {
        const char * s( git_remote_url( this->impl->_remote ) );
        
        return ( s == nullptr ) ? "" : s;
    }
    
    bool Remote::fetch( const std::vector< std::string > & refspecs, const std::string & reflogMessage ) const
    {
        git_strarray      array;
        int               status;
        git_fetch_options options = GIT_FETCH_OPTIONS_INIT;
        
        memset( &array, 0, sizeof( git_strarray ) );
        
        options.callbacks.credentials = IMPL::credentials;
        
        if( refspecs.size() > 0 )
        {
            array.count   = refspecs.size();
            array.strings = new char *[ refspecs.size() ];
            
            for( size_t i = 0; i < refspecs.size(); i++ )
            {
                array.strings[ i ] = new char[ refspecs[ i ].length() + 1 ];
                
                strcpy( array.strings[ i ], refspecs[ i ].c_str() );
            }
        }
        
        status = git_remote_fetch
        (
            this->impl->_remote,
            ( refspecs.size() == 0 ) ? nullptr : &array,
            &options,
            ( reflogMessage.length() == 0 ) ? nullptr : reflogMessage.c_str()
        );
        
        if( refspecs.size() > 0 )
        {
            for( size_t i = 0; i < refspecs.size(); i++ )
            {
                delete [] array.strings[ i ];
            }
            
            delete [] array.strings;
        }
        
        return status == 0;
    }
    
    void swap( Remote & o1, Remote & o2 )
    {
        using std::swap;
        
        swap( o1.impl, o2.impl );
    }
    
    Remote::IMPL::IMPL( git_remote * remote, const Repository & repos ):
        _remote( remote ),
        _repos( repos )
    {
        if( remote == nullptr )
        {
            throw std::runtime_error( "Cannot initialize with a NULL git remote" );
        }
    }
    
    Remote::IMPL::IMPL( const IMPL & o ): IMPL( o._remote, o._repos )
    {}
    
    Remote::IMPL::~IMPL( void )
    {}
    
    int Remote::IMPL::credentials( git_cred ** cred, const char * url, const char * usernameFromURL, unsigned int allowedTypes, void * payload )
    {
        Utility::Credentials c;
        std::string          user;
        std::string          password;
        
        if( c.retrieve( Utility::Arguments::sharedInstance().keychainItem(), user, password ) )
        {
            if( git_cred_userpass_plaintext_new( cred, user.c_str(), password.c_str() ) == 0 )
            {
                return 0;
            }
            
            *( cred ) = nullptr;
        }
        
        return -1;
    }
}
