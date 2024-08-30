#include "minifyjpeg.h"
#include "magickminify.h"


//----------------- decimate_jpeg_2_svc ------------------//
// Implements the server-side RPC procedure
// Takes in jpeg_in and returns decimated version as jpeg_out. 
jpeg_out* decimate_jpeg_2_svc(jpeg_in* jpegIn, struct svc_req* req)
{
    jpeg_out* jpegOut = malloc(sizeof(jpeg_out));
    ssize_t   dst_len;
    
    //---- perform the minification
    jpegOut->out.out_val = magickminify(jpegIn->in.in_val, jpegIn->in.in_len, &dst_len);
    jpegOut->out.out_len = dst_len;
    
    return jpegOut;    
}


int decimate_prog_2_freeresult( SVCXPRT* port, xdrproc_t proc, caddr_t add )
{
    
    return 1;
}