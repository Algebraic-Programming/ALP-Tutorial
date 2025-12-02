#include<iostream>
#include<graphblas.hpp>

void grb_program( const size_t &data_in , size_t &data_out ){
	const size_t s = grb::spmd<>::pid();
	std :: cerr << "Hello from process " << s << std :: endl; // printed by each process
	data_out = 69;
}

int main ( int argc , char ** argv ) {
	size_t data_in = 42 , data_out;

	std::cerr << "Starting... " << std::endl ; // printed only once
	grb::Launcher< grb::AUTOMATIC > launcher;
	launcher.exec( &grb_program, data_in, data_out, true );
	std::cerr << "Finishing: data_out is " << data_out << std::endl ; // printed once
}
