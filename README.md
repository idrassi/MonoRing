# MonoRing : Monolithic distribution of the Ring Programming Language

MonoRing is a monolithic build of Ring programming language that embeds popular extensions into the Ring interpreter and library.
MonoRing doesn't provide any external extensions.
The embedded extensions are:
 - cJson
 - ConsoleColors
 - Curl
 - httplib
 - internet
 - libui
 - libuv
 - murmurhash
 - odbc
 - OpenSSL
 - sockets
 - sqlite
 - stbimage
 - threads
 - winapi
 - wincreg
 - zip

A Ring script that runs under MonoRing can be compiled as a native Windows applications that has no extra runtime dependency and so it can be deployed easily.

For details about the Ring programming language: https://github.com/ring-lang/ring
			
# Building from source code

We provide instructions for building on the next platforms:-

(1) [Microsoft Windows](language/README_Windows.md)

# License 

MonoRing is distributed under the same license as the Ring programming language.
The Ring programming language is distributed under the MIT License.
