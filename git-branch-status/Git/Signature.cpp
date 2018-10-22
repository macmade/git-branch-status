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
 * @file        Signature.cpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#include <stdexcept>
#include "Signature.hpp"
#include "Commit.hpp"
#include "Credentials.hpp"
#include "Arguments.hpp"

namespace Git
{
    class Signature::IMPL
    {
        public:
            
            IMPL( const git_signature * signature, const Commit & commit );
            IMPL( const IMPL & o );
            ~IMPL( void );
            
            const  git_signature * _signature;
            const Commit         & _commit;
    };
    
    Signature::Signature( const git_signature * signature, const Commit & commit ): impl( std::make_shared< IMPL >( signature, commit ) )
    {}
    
    Signature::Signature( const Signature & o ): impl( std::make_shared< IMPL >( *( o.impl ) ) )
    {}
    
    Signature::~Signature( void )
    {}
    
    Signature & Signature::operator =( Signature o )
    {
        swap( *( this ), o );
        
        return *( this );
    }
    
    Signature::operator const git_signature * () const
    {
        return this->impl->_signature;
    }
    
    bool Signature::operator ==( const Signature & o ) const
    {
        return this->impl->_commit == o.impl->_commit && this->name() == o.name() && this->email() == o.email();
    }
    
    bool Signature::operator !=( const Signature & o ) const
    {
        return !operator ==( o );
    }
    
    std::string Signature::name( void ) const
    {
        return ( this->impl->_signature->name == nullptr ) ? "" : this->impl->_signature->name;
    }
    
    std::string Signature::email( void ) const
    {
        return ( this->impl->_signature->email == nullptr ) ? "" : this->impl->_signature->email;
    }
    
    void swap( Signature & o1, Signature & o2 )
    {
        using std::swap;
        
        swap( o1.impl, o2.impl );
    }
    
    Signature::IMPL::IMPL( const git_signature * signature, const Commit & commit ):
        _signature( signature ),
        _commit( commit )
    {
        if( signature == nullptr )
        {
            throw std::runtime_error( "Cannot initialize with a NULL git signature" );
        }
    }
    
    Signature::IMPL::IMPL( const IMPL & o ): IMPL( o._signature, o._commit )
    {}
    
    Signature::IMPL::~IMPL( void )
    {}
}
