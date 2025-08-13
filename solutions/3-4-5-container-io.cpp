
#include <cstddef>
#include <cstring>

#include <graphblas.hpp>
#include <graphblas/utils/parser.hpp>

#include <assert.h>


constexpr size_t max_fn_size = 255;
typedef char Filename[ max_fn_size ];

void hello_world( const Filename &in, int &out ) {
	grb::Vector< bool > x( 497 ), y( 497, 1 );
	grb::Matrix< void > A( 497, 497, 1727 );

	grb::RC rc = grb::set( x, false );
	rc = rc ? rc : grb::setElement( y, true, 200 );
	if( rc != grb::SUCCESS ) {
		out = 10;
		return;
	}

	std::cout << "elements in x: " << grb::nnz( x ) << "\n";
	std::cout << "elements in y: " << grb::nnz( y ) << "\n";
	std::cout << "cacacity of y: " << grb::capacity( y ) << "\n";

	for( const auto &pair : y ) {
		std::cout << "y[ " << pair.first << " ] = " << pair.second << "\n";
	}

	size_t x_nnz = 0;
	for( const auto &pair : x ) {
		(void) ++x_nnz;
		if( pair.second ) {
			std::cerr << "x[ " << pair.first << " ] reads true "
				<< "but false was expected!\n";
			out = 20;
			return;
		}
	}
	if( x_nnz != 497 ) {
		std::cerr << "Output iterator of x retrieved " << x_nnz << " elements, "
			<< "expected 497!\n";
		out = 30;
		return;
	}

	grb::utils::MatrixFileReader< double > parser( in, true );
	if( parser.m() != 497 || parser.n() != 497 ) {
		std::cerr << in << " corresponds to a matrix of unexpected size\n";
		out = 40;
		return;
	}
	{
		const auto iterator = parser.begin();
		std::cout << "First parsed entry: ( " << iterator.i() << ", " << iterator.j() << " ) = " << iterator.v() << "\n";
	}

	rc = grb::buildMatrixUnique(
		A,
		parser.begin( grb::SEQUENTIAL ), parser.end( grb::SEQUENTIAL ),
		grb::SEQUENTIAL
	);
	if( rc != grb::SUCCESS ) {
		std::cerr << "Error encountered during reading " << in << "\n";
		out = 50;
		return;
	}
	std::cout << "nonzeroes in A: " << grb::nnz( A ) << "\n";

	// all OK
	out = 0;
}

int main( int argc, char ** argv ) {
	if( argc < 2 || argc > 2 ) {
		std::cout << "Usage: " << argv[ 0 ] << " </path/to/west0497.mtx>\n";
		return 0;
	}

	// get input
	Filename fn;
	(void) std::strncpy( fn, argv[ 1 ], max_fn_size );

	// set up output field
	int error_code = 100;

	// launch hello world program
	grb::Launcher< grb::AUTOMATIC > launcher;
	assert( launcher.exec( &hello_world, fn, error_code, true )
		== grb::SUCCESS );

	// return with the hello_world error code
	return error_code;
}

