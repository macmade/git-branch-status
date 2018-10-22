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
 * @header      Repository.hpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#ifndef GIT_REPOSITORY_HPP
#define GIT_REPOSITORY_HPP

#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <optional>
#include <git2.h>
#include "Branch.hpp"
#include "Remote.hpp"

namespace Git
{
    class Repository
    {
        public:
            
            Repository( const std::string & path );
            Repository( const Repository & o );
            ~Repository( void );
            
            Repository & operator =( Repository o );
            
            operator git_repository *() const;
            
            bool operator ==( const Repository & o ) const;
            bool operator !=( const Repository & o ) const;
            
            std::string             path( void )     const;
            std::vector< Branch >   branches( void ) const;
            std::vector< Remote >   remotes( void )  const;
            std::optional< Branch > head( void )     const;
            
            friend void swap( Repository & o1, Repository & o2 );
            
        private:
            
            class IMPL;
            
            std::shared_ptr< IMPL > impl;
    };
}

#endif /* GIT_REPOSITORY_HPP */
