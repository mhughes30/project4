# Project README file

Author: Mike Hughes (gte769h)

This is the README file for project 4: RPC - Remote-Procedure Calls.

## Project Description

This project consisted of buildina an RPC server and client, as well as optional extra credit, utlizing Sun RPC. 

### Main Part: Create RPC with single-threaded server

- __Summary Description__

The first step in this project involved defining the RPC interface, using the XDR file minifyjpeg.x. Then, the rpcgen tool is used, which generated much 
of the required code, including the client and server stubs. Some of the generated functions required modification and were defined in minifyjpeg.h. 

The RPC server produced a lower resolution version of the input image sent from the client, using ImageMagick. 

I decided on the following interface for my RPC implementation, shown below. 

I used the opague data type for jpeg_in and jpeg_out, because opaque is used for uniterpreted binary data (just raw bytes), which is perfect
for use with jpegs. I chose a maximum array size MAX_SIZE of 100 MB, in order to support the range of expected files. I used "<>" notation
in order to specify variable-length arrays. Therefore, only the required memory will be used for each file, up to the maximum size of 100 MB.
The structs jpeg_in and jpeg_out are translated into a data structure containing length and value fields. 

For the function interface, I named it DECIMATE_JPEG, because that is what is done. It takes a jpeg_in type and outputs a jpeg_out type. 
I chose the service ID, based on its being in the conventional range of IDs. I chose 2 as the version, simply because this was me second version of this interface. 

```c 
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
```

The client-side functions were implemented in minify_via_rpc.c. I selected a timeout period of 8 seconds and a timeout code 
of 124, as required by the directions. 

The client creation function is shown below. The function clnt_create creates the client handle, specifying the server, 
the RPC interface, and TCP. I also specified the timeout period, using clnt_control. If the timeout period expires durig operation, the 124
error code is returned. 

```c
#define CLIENT_TIMEOUT_DUR_SEC     8    // timeout in seconds
#define CLIENT_TIMEOUT_CODE        124  // special return code for a timeout
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
```

The following function, minify_via_rpc performs the actual remote procedure call. It puts the source jpeg file into a 
jpeg_in struct. IT then calls the RPC routine decimate_jpeg_2 with the input of jpegIn and the client handle. The 
decimated (minified) file is returned as jpegOut. 

```c
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
```
The RPC server-side functions were implemented in minifyjpeg.c. The code below shows the function.
It takes a jpeg_int, calls magickminify to decimate the file and returns a jpeg_out. 

```C
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
```

Sun RPC is awesome. I really enjoyed implementing this project. I love how it hides the implementation details of the client/server interaction. 


- __What created problems for you?__

I honestly didn't have any problems with this project. Everything was relatively straightforward, but I did need to study Sun RPC in more detail, using the references 
listed below. 

- __What tests would you have added to the test suite?__

I would have tested outer image formats. However, the RPC server was image format agnostic, except with respect to ImageMagick. And, this project was
based on using JPEGs. 

- __If you were going to do this project again, how would you improve it?__

I am happy with the outcome of this project, and there is nothing I would do to improve my implementation. 

- __If you didn't think something was clear in the documentation, what would you write instead?__

The documentation was clear. 


### Extra Credit: Create RPC with multi-threaded server

- __Summary Description__

I did not perform this secion.

- __What created problems for you?__

- __What tests would you have added to the test suite?__

- __If you were going to do this project again, how would you improve it?__

- __If you didn't think something was clear in the documentation, what would you write instead?__


## Known Bugs/Issues/Limitations

### Main Part: Create RPC with single-threaded server

There are no known bugs, issues, or limitations. Everythign works perfectly with the RPC mechanism. 

### Extra Credit: Create RPC with multi-threaded server


## References

http://users.cs.cf.ac.uk/Dave.Marshall/C/node34.html
http://web.cs.wpi.edu/~rek/DCS/D04/SunRPC.html
http://www.ibm.com/support/knowledgecenter/ssw_i5_54/apis/clnt_control.htm


