/*
 * Complete this file and run rpcgen -MN minifyjpeg.x
 */

const MAX_SIZE = 100000000;

struct jpeg_in {
	opaque in<MAX_SIZE>;
};

struct jpeg_out {
	opaque out<MAX_SIZE>;
};


program DECIMATE_PROG { 						/*RPC service name*/
	version DECIMATE_VERS {
		jpeg_out DECIMATE_JPEG(jpeg_in) = 1;	/*procedure*/
	} = 2;										/*version*/
} = 0x2EAD1234;									/*service ID*/				