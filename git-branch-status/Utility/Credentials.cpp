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
 * @file        Credentials.hpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#include "Credentials.hpp"

#ifdef __APPLE__
#include <Security/Security.h>
#endif

namespace Utility
{
    class Credentials::IMPL
    {
        public:
            
            IMPL( void );
            IMPL( const IMPL & o );
            ~IMPL( void );
    };

    Credentials::Credentials( void ): impl( std::make_shared< IMPL >() )
    {}

    Credentials::Credentials( const Credentials & o ): impl( std::make_shared< IMPL >( *( o.impl ) ) )
    {}

    Credentials::~Credentials( void )
    {}

    Credentials & Credentials::operator =( Credentials o )
    {
        swap( *( this ), o );
        
        return *( this );
    }
    
    bool Credentials::retrieve( const std::string & name, std::string & user, std::string & password )
    {
        user     = "";
        password = "";
        
        if( name.length() == 0 )
        {
            return false;
        }
        
        #ifdef __APPLE__
        
        {
            SecKeychainRef          keychain( nullptr );
            CFStringRef             itemName;
            CFMutableDictionaryRef  query;
            SecKeychainItemRef      item( nullptr );
            std::string             u;
            std::string             p;
            
            if( SecKeychainCopyDefault( &keychain ) != errSecSuccess || keychain == nullptr )
            {
                return false;
            }
            
            if( SecKeychainUnlock( keychain, 0, nullptr, false ) != errSecSuccess )
            {
                CFRelease( keychain );
                
                return false;
            }
            
            itemName = CFStringCreateWithCString( nullptr, name.c_str(), kCFStringEncodingUTF8 );
            query    = CFDictionaryCreateMutable( nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );
            
            CFDictionarySetValue( query, kSecMatchLimit, kSecMatchLimitOne );
            CFDictionarySetValue( query, kSecClass, kSecClassGenericPassword );
            CFDictionarySetValue( query, kSecAttrLabel, itemName );
            
            if( SecItemCopyMatching( query, static_cast< CFTypeRef * >( const_cast< const void ** >( reinterpret_cast< void ** >( &item ) ) ) ) == errSecSuccess && item != nullptr )
            {
                do
                {
                    {
                        SecKeychainAttributeInfo * info( nullptr );
                        SecKeychainAttributeList * list;
                        UInt32                     length;
                        void                     * data;
                        
                        if( item == nullptr || CFGetTypeID( item ) != SecKeychainItemGetTypeID() )
                        {
                            break;
                        }
                        
                        if( SecKeychainAttributeInfoForItemID( keychain, CSSM_DL_DB_RECORD_GENERIC_PASSWORD, &info ) != errSecSuccess || info == nullptr )
                        {
                            break;
                        }
                        
                        if( SecKeychainItemCopyAttributesAndData( item, info, NULL, &list, &length, &data ) != errSecSuccess )
                        {
                            break;
                        }
                        
                        for( UInt32 i = 0; i < list->count; i++ )
                        {
                            if( list->attr[ i ].tag == kSecAccountItemAttr )
                            {
                                u = std::string( static_cast< const char * >( list->attr[ i ].data ), list->attr[ i ].length );
                            }
                        }
                        
                        p = std::string( static_cast< const char * >( data ), length );
                        
                        SecKeychainItemFreeAttributesAndData( list, data );
                    }
                }
                while( 0 );
            }
            
            if( item != nullptr )
            {
                CFRelease( item );
            }
            
            CFRelease( query );
            CFRelease( itemName );
            CFRelease( keychain );
            
            if( u.length() > 0 && p.length() > 0 )
            {
                user     = u;
                password = p;
                
                return true;
            }
            
            return false;
        }
        
        #else
        
        return false;
        
        #endif
    }
    
    void swap( Credentials & o1, Credentials & o2 )
    {
        using std::swap;
        
        swap( o1.impl, o2.impl );
    }

    Credentials::IMPL::IMPL( void )
    {}

    Credentials::IMPL::IMPL( const IMPL & o ): IMPL()
    {
        ( void )o;
    }

    Credentials::IMPL::~IMPL( void )
    {}
}
