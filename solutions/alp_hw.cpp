
#include <cstddef>
#include <cstring>
#include <graphblas.hpp>
#include <assert.h>
constexpr size_t max_fn_size = 255;
typedef char Filename[ max_fn_size ];
void hello_world( const Filename &in, int &out ) {
std::cout << "Hello from " << in << std::endl;
out = 0;
}
int main( int argc, char ** argv ) {
// get input
Filename fn;
(void) std::strncpy( fn, argv[ 0 ], max_fn_size );
// set up output field
int error_code = 100;
// launch hello world program
grb::Launcher< grb::AUTOMATIC > launcher;
assert( launcher.exec( &hello_world, fn, error_code, true )
== grb::SUCCESS );
// return with the hello_world error code
return error_code;
}

