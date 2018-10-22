################################################################################
# The MIT License (MIT)
# 
# Copyright (c) 2018 Jean-David Gadina - www-xs-labs.com
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
################################################################################

# @file        Makefile
# @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com

DIR_TMP := ./tmp/
DIR_SRC := ../../Submodules/libgit2/

MAC_TARGET   := 10.13
FLAGS_C      := -arch x86_64 -mmacosx-version-min=$(MAC_TARGET)
FLAGS_STDLIB := -stdlib=libc++

all:
	@cd $(DIR_TMP) && cmake -DMACOSX_DEPLOYMENT_TARGET=$(MAC_TARGET) -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DTHREADSAFE=ON -DBUILD_CLAR=OFF -DCMAKE_C_FLAGS="$(FLAGS_C) $(FLAGS_STDLIB)" -DCMAKE_CXX_FLAGS="$(FLAGS_C) $(FLAGS_STDLIB)" $(DIR_SRC)
	@cd $(DIR_TMP) && make CFLAGS="$(FLAGS_C)" CXXFLAGS="$(FLAGS_C) $(FLAGS_STDLIB)" CPPFLAGS="$(FLAGS_C) $(FLAGS_STDLIB)" LDFLAGS="$(FLAGS_STDLIB)"
	@cp $(DIR_TMP)libgit2.a .

clean:
	@rm -rf $(DIR_TMP)*
