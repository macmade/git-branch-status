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
 * @header      Commit.hpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#ifndef GIT_COMMIT_HPP
#define GIT_COMMIT_HPP

#include <string>
#include <memory>
#include <algorithm>
#include <git2.h>

namespace Git
{
    class Repository;
    
    class Commit
    {
        public:
            
            Commit( const git_oid * oid, const Repository & repos );
            Commit( const Commit & o );
            ~Commit( void );
            
            Commit & operator =( Commit o );
            
            operator git_commit    * () const;
            operator const git_oid * () const;
            
            bool operator ==( const Commit & o ) const;
            bool operator !=( const Commit & o ) const;
            
            std::string hash( void )    const;
            std::string body( void )    const;
            std::string message( void ) const;
            std::string summary( void ) const;
            time_t      time( void )    const;
            
            friend void swap( Commit & o1, Commit & o2 );
            
        private:
            
            class IMPL;
            
            std::shared_ptr< IMPL > impl;
    };
}

#endif /* GIT_COMMIT_HPP */
