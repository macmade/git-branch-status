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
 * @file        Repository.cpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#include <stdexcept>
#include "Repository.hpp"

namespace Git
{
    class Repository::IMPL
    {
        public:
            
            IMPL( const std::string & path );
            IMPL( const IMPL & o );
            ~IMPL( void );
            
            std::string                    _path;
            git_repository               * _repos;
            std::vector< git_reference * > _branches;
            std::vector< git_remote    * > _remotes;
    };
    
    Repository::Repository( const std::string & path ): impl( std::make_shared< IMPL >( path ) )
    {}
    
    Repository::Repository( const Repository & o ): impl( std::make_shared< IMPL >( *( o.impl ) ) )
    {}
    
    Repository::~Repository( void )
    {}
    
    Repository & Repository::operator =( Repository o )
    {
        swap( *( this ), o );
        
        return *( this );
    }
    
    Repository::operator git_repository *() const
    {
        return this->impl->_repos;
    }
    
    bool Repository::operator ==( const Repository & o ) const
    {
        return this->impl->_repos == o.impl->_repos;
    }
    
    bool Repository::operator !=( const Repository & o ) const
    {
        return !operator ==( o );
    }
    
    std::string Repository::path( void ) const
    {
        return this->impl->_path;
    }
    
    std::vector< Branch > Repository::branches( void ) const
    {
        std::vector< Branch > branches;
        
        for( const auto & ref: this->impl->_branches )
        {
            try
            {
                branches.push_back( { ref, *( this ) } );
            }
            catch( ... )
            {}
        }
        
        std::sort
        (
            std::begin( branches ),
            std::end( branches ),
            []( const Branch & b1, const Branch & b2 )
            {
                return b1.name() < b2.name();
            }
        );
        
        return branches;
    }
    
    std::vector< Remote > Repository::remotes( void ) const
    {
        std::vector< Remote > remotes;
        
        for( const auto & remote: this->impl->_remotes )
        {
            try
            {
                remotes.push_back( { remote, *( this ) } );
            }
            catch( ... )
            {}
        }
        
        return remotes;
    }
    
    std::optional< Branch > Repository::head( void ) const
    {
        for( const auto & b: this->branches() )
        {
            if( b.isHead() )
            {
                return b;
            }
        }
        
        return {};
    }
    
    void swap( Repository & o1, Repository & o2 )
    {
        using std::swap;
        
        swap( o1.impl, o2.impl );
    }
    
    Repository::IMPL::IMPL( const std::string & path ):
        _path( path ),
        _repos( nullptr )
    {
        git_libgit2_init();
        
        {
            git_branch_iterator * it( nullptr );
            
            if( git_repository_open( &( this->_repos ), path.c_str() ) != 0 || this->_repos == nullptr )
            {
                throw std::runtime_error( "Cannot open Git repository: " + path );
            }
            
            if( git_branch_iterator_new( &it, this->_repos, GIT_BRANCH_ALL ) != 0 || it == nullptr )
            {
                throw std::runtime_error( "Cannot iterate branches" );
            }
            
            {
                git_reference * ref( nullptr );
                git_branch_t    type;
                
                while( git_branch_next( &ref, &type, it ) == 0 )
                {
                    if( ref != nullptr )
                    {
                        this->_branches.push_back( ref );
                    }
                }
            }
            
            git_branch_iterator_free( it );
        }
        
        {
            git_strarray names;
            
            memset( &names, 0, sizeof( git_strarray ) );
            
            if( git_remote_list( &names, this->_repos ) == 0 )
            {
                for( size_t i = 0; i < names.count; i++ )
                {
                    {
                        git_remote * remote( nullptr );
                        
                        if( git_remote_lookup( &remote, this->_repos, names.strings[ i ] ) == 0 && remote != nullptr )
                        {
                            this->_remotes.push_back( remote );
                        }
                    }
                }
            }
        }
    }
    
    Repository::IMPL::IMPL( const IMPL & o ): IMPL( o._path )
    {}
    
    Repository::IMPL::~IMPL( void )
    {
        for( const auto & reference: this->_branches )
        {
            git_reference_free( reference );
        }
        
        for( const auto & remote: this->_remotes )
        {
            git_remote_free( remote );
        }
        
        if( this->_repos != nullptr )
        {
            git_repository_free( this->_repos );
        }
    }
}
