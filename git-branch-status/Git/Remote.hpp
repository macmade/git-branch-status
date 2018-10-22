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
 * @header      Remote.hpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#ifndef GIT_REMOTE_HPP
#define GIT_REMOTE_HPP

#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <git2.h>

namespace Git
{
    class Repository;
    
    class Remote
    {
        public:
            
            Remote( git_remote * remote, const Repository & repos );
            Remote( const Remote & o );
            ~Remote( void );
            
            Remote & operator =( Remote o );
            
            operator git_remote * () const;
            
            bool operator ==( const Remote & o ) const;
            bool operator !=( const Remote & o ) const;
            
            std::string name( void ) const;
            std::string url(  void ) const;
            
            bool fetch( const std::vector< std::string > & refspecs = {}, const std::string & reflogMessage = "" ) const;
            
            friend void swap( Remote & o1, Remote & o2 );
            
        private:
            
            class IMPL;
            
            std::shared_ptr< IMPL > impl;
    };
}

#endif /* GIT_REMOTE_HPP */
