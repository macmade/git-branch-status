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
 * @header      Branch.hpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#ifndef GIT_BRANCH_HPP
#define GIT_BRANCH_HPP

#include <string>
#include <memory>
#include <algorithm>
#include <optional>
#include <git2.h>
#include "Commit.hpp"

namespace Git
{
    class Repository;
    
    class Branch
    {
        public:
            
            Branch( git_reference * ref, const Repository & repos );
            Branch( const Branch & o );
            ~Branch( void );
            
            Branch & operator =( Branch o );
            
            operator git_reference *() const;
            
            bool operator ==( const Branch & o ) const;
            bool operator !=( const Branch & o ) const;
            
            bool operator >( const Branch & o ) const;
            bool operator <( const Branch & o ) const;
            
            std::string             name( void )                 const;
            bool                    isHead( void )               const;
            bool                    isAhead( const Branch & o )  const;
            bool                    isBehind( const Branch & o ) const;
            std::optional< Commit > lastCommit( void )           const;
            
            friend void swap( Branch & o1, Branch & o2 );
            
        private:
            
            class IMPL;
            
            std::shared_ptr< IMPL > impl;
    };
}

#endif /* GIT_BRANCH_HPP */
