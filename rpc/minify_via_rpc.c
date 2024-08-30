#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "minifyjpeg_xdr.c"
#include "minifyjpeg_clnt.c"
#include <pthread.h>


#define CLIENT_TIMEOUT_DUR_SEC     8    // timeout in seconds
#define CLIENT_TIMEOUT_CODE        124  // special return code for a timeout


//----------------------- get_minify_client --------------------------//
// Connects to the RPC server, and returns a CLIENT handle.
CLIENT* get_minify_client(char* server)
{
    struct timeval tv;
	CLIENT *cl = NULL;

    //---- bind to the RPC service
	cl = clnt_create(server, DECIMATE_PROG, DECIMATE_VERS, "tcp");
	if (cl == NULL)
	{
		fprintf(stderr, "clnt_create() error\n");
		clnt_pcreateerror(server);
	}
    
    //---- set the timeout
    tv.tv_sec  = CLIENT_TIMEOUT_DUR_SEC;
    tv.tv_usec = 0;
    clnt_control(cl, CLSET_TIMEOUT, (char*)&tv);

    return cl;
}


//----------------------- minify_via_rpc --------------------------//
// calls the remote procedure
void* minify_via_rpc(CLIENT* cl, void* src_val, size_t src_len, size_t* dst_len)
{
	jpeg_out* jpegOut; 
    jpeg_in*  jpegIn = malloc(sizeof(jpeg_in));
    
    jpegIn->in.in_len = src_len;
    jpegIn->in.in_val = src_val;
    
    fprintf(stderr, "req: %lu", src_len);

    //---- Call the RPC Routine
    jpegOut = decimate_jpeg_2(jpegIn, cl);   
    if (jpegOut == NULL)
    {
        // this indicates a timeout occurred
        exit(CLIENT_TIMEOUT_CODE);
    }
    *dst_len = jpegOut->out.out_len;
    fprintf(stderr, "decimate done: %lu\n", *dst_len);
    
    free(jpegIn);
    
    return jpegOut->out.out_val;
}