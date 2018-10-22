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
 * @file        Commit.cpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#include <stdexcept>
#include "Commit.hpp"
#include "Repository.hpp"

namespace Git
{
    class Commit::IMPL
    {
        public:
            
            IMPL( const git_oid * ref, const Repository & repos );
            IMPL( const IMPL & o );
            ~IMPL( void );
            
            const git_oid    * _oid;
            const Repository & _repos;
            git_commit       * _commit;
    };
    
    Commit::Commit( const git_oid * oid, const Repository & repos ): impl( std::make_shared< IMPL >( oid, repos ) )
    {}
    
    Commit::Commit( const Commit & o ): impl( std::make_shared< IMPL >( *( o.impl ) ) )
    {}
    
    Commit::~Commit( void )
    {}
    
    Commit & Commit::operator =( Commit o )
    {
        swap( *( this ), o );
        
        return *( this );
    }
    
    Commit::operator git_commit * () const
    {
        return this->impl->_commit;
    }
    
    Commit::operator const git_oid * () const
    {
        return this->impl->_oid;
    }
    
    bool Commit::operator ==( const Commit & o ) const
    {
        return this->impl->_repos == o.impl->_repos && this->hash() == o.hash();
    }
    
    bool Commit::operator !=( const Commit & o ) const
    {
        return !operator ==( o );
    }
    
    std::string Commit::hash( void ) const
    {
        char s[ GIT_OID_HEXSZ + 1 ];
        
        memset( s, 0, sizeof( s ) );
        
        if( git_oid_tostr( s, sizeof( s ), this->impl->_oid ) == nullptr )
        {
            return {};
        }
        
        return s;
    }

    std::string Commit::body( void ) const
    {
        const char * s( git_commit_body( this->impl->_commit ) );
        
        return ( s == nullptr ) ? "" : s;
    }
    
    std::string Commit::message( void ) const
    {
        const char * s( git_commit_message( this->impl->_commit ) );
        
        return ( s == nullptr ) ? "" : s;
    }
    
    std::string Commit::summary( void ) const
    {
        const char * s( git_commit_summary( this->impl->_commit ) );
        
        return ( s == nullptr ) ? "" : s;
    }
    
    time_t Commit::time( void ) const
    {
        return git_commit_time( this->impl->_commit );
    }
    
    void swap( Commit & o1, Commit & o2 )
    {
        using std::swap;
        
        swap( o1.impl, o2.impl );
    }
    
    Commit::IMPL::IMPL( const git_oid * oid, const Repository & repos ):
        _oid( oid ),
        _repos( repos )
    {
        if( oid == nullptr )
        {
            throw std::runtime_error( "Cannot initialize with a NULL git oid" );
        }
        
        if( git_commit_lookup( &( this->_commit ), repos, oid ) != 0 || this->_commit == nullptr )
        {
            throw std::runtime_error( "Cannot lookup commit" );
        }
    }
    
    Commit::IMPL::IMPL( const IMPL & o ): IMPL( o._oid, o._repos )
    {}
    
    Commit::IMPL::~IMPL( void )
    {}
}
