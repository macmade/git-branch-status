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
 * @header      Optional.hpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 * @discussion  std::optional is only available in macOS 10.14 and later...
 */

#ifndef UTILITY_OPTIONAL_HPP
#define UTILITY_OPTIONAL_HPP

#include <algorithm>
#include <stdexcept>
#include <memory>

namespace Utility
{
    template< typename _T_ >
    class Optional
    {
        public:
            
            class BadAccessException: public std::exception
            {};
            
            Optional( void )
            {}
            
            Optional( const Optional< _T_ > & o ):
                _hasValue( o._hasValue )
            {
                if( o._hasValue )
                {
                    new ( this->_data )_T_( reinterpret_cast< const _T_ & >( o._data ) );
                }
            }
            
            Optional( const _T_ & o ):
                _hasValue( true )
            {
                new ( this->_data )_T_( o );
            }
            
            ~Optional( void )
            {
                if( this->_hasValue )
                {
                    ( reinterpret_cast< _T_ * >( this->_data ) )->~_T_();
                }
            }
            
            Optional & operator =( Optional< _T_ > o )
            {
                swap( *( this ), o );
                
                return *( this );
            }
            
            Optional & operator =( const _T_ & o )
            {
                swap( *( this ), Optional< _T_ >( o ) );
                
                return *( this );
            }
            
            bool hasValue( void ) const
            {
                return this->_hasValue;
            }
            
            const _T_ * operator ->( void ) const
            {
                if( this->_hasValue == false )
                {
                    throw BadAccessException();
                }
                
                return reinterpret_cast< const _T_ * >( this->_data );
            }
            
            _T_ * operator ->( void )
            {
                if( this->_hasValue == false )
                {
                    throw BadAccessException();
                }
                
                return reinterpret_cast< _T_ * >( this->_data );
            }
            
            const _T_ & operator *( void ) const &
            {
                return this->value();
            }
            
            _T_ & operator *( void ) &
            {
                return this->value();
            }
            
            _T_ & value( void ) &
            {
                return *( this->operator->() );
            }
            
            const _T_ & value( void ) const &
            {
                return *( this->operator->() );
            }
            
            _T_ valueOr( _T_ && defaultValue ) const &
            {
                return ( this->hasValue() ) ? this->value() : defaultValue;
            }
            
            _T_ valueOr( _T_ && defaultValue ) &&
            {
                return ( this->hasValue() ) ? this->value() : defaultValue;
            }
            
            void reset( void )
            {
                Optional< _T_ > o;
                
                swap( *( this ), o );
            }
            
            friend void swap( Optional< _T_ > & o1, Optional< _T_ > & o2 )
            {
                using std::swap;
                
                swap( o1._data,     o2._data );
                swap( o1._hasValue, o2._hasValue );
            }
            
        private:
            
            uint8_t _data[ sizeof( _T_ ) ] = {};
            bool    _hasValue              = false;
    };
    
    template< typename _T_ >
    bool operator ==( const Optional< _T_ > & o1, const Optional< _T_ > & o2 )
    {
        if( o1.hasValue() == false || o2.hasValue() == false )
        {
            return false;
        }
        
        return o1.value() == o2.value();
    }
    
    template< typename _T_ >
    bool operator ==( const _T_ & o1, const _T_ & o2 )
    {
        return o1 == o2;
    }
    
    template< typename _T_ >
    bool operator ==( const Optional< _T_ > & o1, const _T_ & o2 )
    {
        if( o1.hasValue() == false )
        {
            return false;
        }
        
        return o1.value() == o2;
    }
    
    template< typename _T_ >
    bool operator ==( const _T_ & o1, const Optional< _T_ > & o2 )
    {
        if( o2.hasValue() == false )
        {
            return false;
        }
        
        return o1 == o2.value();
    }
    
    template< typename _T_ >
    bool operator !=( const Optional< _T_ > & o1, const Optional< _T_ > & o2 )
    {
        return !operator ==( o1, o2 );
    }
    
    template< typename _T_ >
    bool operator !=( const _T_ & o1, const _T_ & o2 )
    {
        return !operator ==( o1, o2 );
    }
    
    template< typename _T_ >
    bool operator !=( const Optional< _T_ > & o1, const _T_ & o2 )
    {
        return !operator ==( o1, o2 );
    }
    
    template< typename _T_ >
    bool operator !=( const _T_ & o1, const Optional< _T_ > & o2 )
    {
        return !operator ==( o1, o2 );
    }
}

#endif /* UTILITY_OPTIONAL_HPP */
