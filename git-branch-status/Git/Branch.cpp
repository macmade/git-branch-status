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
 * @file        Branch.cpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#include <stdexcept>
#include "Branch.hpp"
#include "Repository.hpp"

namespace Git
{
    class Branch::IMPL
    {
        public:
            
            IMPL( git_reference * ref, const Repository & repos );
            IMPL( const IMPL & o );
            ~IMPL( void );
            
            bool graph( size_t & ahead, size_t & behind, const Branch & branch );
            
            git_reference    * _ref;
            const Repository & _repos;
            std::string        _name;
            bool               _head;
    };
    
    Branch::Branch( git_reference * ref, const Repository & repos ): impl( std::make_shared< IMPL >( ref, repos ) )
    {}
    
    Branch::Branch( const Branch & o ): impl( std::make_shared< IMPL >( *( o.impl ) ) )
    {}
    
    Branch::~Branch( void )
    {}
    
    Branch & Branch::operator =( Branch o )
    {
        swap( *( this ), o );
        
        return *( this );
    }
    
    Branch::operator git_reference *() const
    {
        return this->impl->_ref;
    }
    
    bool Branch::operator ==( const Branch & o ) const
    {
        return this->impl->_repos == o.impl->_repos && this->impl->_name == o.impl->_name;
    }
    
    bool Branch::operator !=( const Branch & o ) const
    {
        return !operator ==( o );
    }
    
    bool Branch::operator >( const Branch & o ) const
    {
        return this->isAhead( o );
    }
    
    bool Branch::operator <( const Branch & o ) const
    {
        return this->isBehind( o );
    }
    
    std::string Branch::name( void ) const
    {
        return this->impl->_name;
    }
    
    bool Branch::isHead( void ) const
    {
        return this->impl->_head;
    }
    
    bool Branch::isAhead( const Branch & o ) const
    {
        size_t ahead( 0 );
        size_t behind( 0 );
        
        if( *( this ) == o )
        {
            return false;
        }
        
        this->impl->graph( ahead, behind, o );
        
        return ahead > 0;
    }
    
    bool Branch::isBehind( const Branch & o ) const
    {
        size_t ahead( 0 );
        size_t behind( 0 );
        
        if( *( this ) == o )
        {
            return false;
        }
        
        this->impl->graph( ahead, behind, o );
        
        return behind > 0;
    }
    
    std::optional< Commit > Branch::lastCommit( void ) const
    {
        const git_oid * oid( nullptr );
        
        oid = git_reference_target( this->impl->_ref );
        
        if( oid == nullptr )
        {
            {
                git_reference * ref( nullptr );
                
                if( git_reference_resolve( &ref, this->impl->_ref ) != 0 || ref == nullptr )
                {
                    throw std::runtime_error( "Cannot resolve reference" );
                }
                
                oid = git_reference_target( ref );
            }
        }
        
        if( oid != nullptr )
        {
            return Commit( oid, this->impl->_repos );
        }
        
        return {};
    }
    
    void swap( Branch & o1, Branch & o2 )
    {
        using std::swap;
        
        swap( o1.impl, o2.impl );
    }
    
    Branch::IMPL::IMPL( git_reference * ref, const Repository & repos ):
        _ref( ref ),
        _repos( repos ),
        _head( false )
    {
        const char * name( nullptr );
        
        if( ref == nullptr )
        {
            throw std::runtime_error( "Cannot initialize with a NULL git reference" );
        }
        
        if( git_branch_name( &name, ref ) != 0 || name == nullptr )
        {
            throw std::runtime_error( "Cannot retrieve branch name" );
        }
        
        this->_name = name;
        this->_head = git_branch_is_head( ref );
    }
    
    Branch::IMPL::IMPL( const IMPL & o ): IMPL( o._ref, o._repos )
    {}
    
    Branch::IMPL::~IMPL( void )
    {}
    
    bool Branch::IMPL::graph( size_t & ahead, size_t & behind, const Branch & branch )
    {
        const git_oid * oid1( nullptr );
        const git_oid * oid2( nullptr );
        
        oid1 = git_reference_target( this->_ref );
        oid2 = git_reference_target( branch.impl->_ref );
        
        if( oid1 == nullptr )
        {
            {
                git_reference * ref( nullptr );
                
                if( git_reference_resolve( &ref, this->_ref ) != 0 || ref == nullptr )
                {
                    throw std::runtime_error( "Cannot resolve reference" );
                }
                
                oid1 = git_reference_target( ref );
            }
        }
        
        if( oid2 == nullptr )
        {
            {
                git_reference * ref( nullptr );
                
                if( git_reference_resolve( &ref, branch.impl->_ref ) != 0 || ref == nullptr )
                {
                    throw std::runtime_error( "Cannot resolve reference" );
                }
                
                oid2 = git_reference_target( ref );
            }
        }
        
        if( oid1 == nullptr || oid2 == nullptr )
        {
            throw std::runtime_error( "Cannot get reference target" );
        }
        
        return git_graph_ahead_behind( &ahead, &behind, this->_repos, oid1, oid2 ) == 0;
    }
}
