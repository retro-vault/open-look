/*
 * Declares private XView interfaces, types, and macros for io stream.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef suntool_io_stream_DEFINED
#define suntool_io_stream_DEFINED
#include <xview/sun.h> 

#ifndef makedev
#include <sys/types.h>
#endif


/* TYPES */
union	Ops_Pointer {
	struct input_ops_vector		*input_ops;
	struct output_ops_vector	*output_ops;
};

enum Stream_Type {Input, Output};

struct	Stream {
	enum Stream_Type	stream_type;
	char			*stream_class; /* e.g. "String_Input_Stream" */
	union Ops_Pointer	ops; /* the procedures that implement the stream */
	struct Stream		*backing_stream;	/* for streams that are composed by layering one stream on top of the other, such as xv_filtered_comments_stream. Allows clients to get at the lower stream, e.g. to find out how many characters were seen at that level */
	caddr_t			client_data;
};

struct	posrec {
	int		lineno;
	int		charpos;
};

typedef	struct Stream STREAM;

struct	input_ops_vector {
	int	(*str_getc)(STREAM *in);	/* (struct Stream *in)
				 * Note: it is the responsibility of the
				 * implementor of the stream to return EOF
				 * to indicate that the stream is empty,
				 * and to insure that subsequent attempts
				 * to read from that stream will continue
				 * to return EOF.
				 */
	int	(*ungetc)(char c, STREAM *in);	/* (char c, struct Stream *in)
				 * implementor only responsible for
				 * for putting back characters that were there.
				 * should return EOF if was unable to put back
				 * the character.
				 * Note: if not provided by implementor,
				 * i.e., ungetc = NULL, will be supplied
				 * by stream package first time it is required.
*** NOT IMPLEMENTED YET ***
				 * See comment regarding ungetc below. 
				 */
	char	*(*str_fgets) (char *s, int n, STREAM *in);/* (char *, n, struct Stream *in)
				 * reads n-1 characters, or up to a newline
				 * character, whichever comes first,
				 * from the stream in into the string s.
				 * The last character read into s is
				 * followed by a null character.
				 * The newline is included.						 * returns NULL on end of file or error.
				 * not every stream will implement this
				 * procedure. For those that don't, the
				 * equivalent n calls to str_getc will be
				 * performed.
				 */
	int	(*chars_avail) (STREAM *in); /* (struct Stream *in) */
	struct posrec (*get_pos) (STREAM *in);	/* (struct Stream *in) 
				 * returns postion record consisting of
				 * line number and character position,
				 * character position is how many characters have been read.
				 * this is the value to give to set_pos
				 * if lineno = -1, means stream doesn't know
				 * lineno. This can occur after having
				 * reset with set_pos
				 */
	int	(*set_pos) (STREAM *in, int n);	/* (int n, struct Stream *in)
				 * not every input stream will implement this
				 * procedure. returns -1 if not implemented
				 * or some other failure.
				 */
	
	void	(*close) (STREAM *in);	/* (struct Stream *in) */
};

struct	output_ops_vector {
	int	(*str_putc) (char c, STREAM *out);	/* (char c, struct Stream *out) */
	void	(*str_fputs) (char* s, STREAM *out);	/* (char *, struct Stream *out)
				 * copies the null-terminated string s to out.
				 * does not append a newline
				 */
	struct posrec	(*get_pos) (STREAM *out);	/* (struct Stream *out) */
	void	(*flush) (STREAM *out);	/* (struct Stream *out)
				 * can be defaulted to NULL = NOP
				 */
	void	(*close) (STREAM *out);	/* (struct Stream *stream) */
};


#ifndef CHARCLASS	 /* also defined in io_streams.h */
#define CHARCLASS
enum CharClass {Break, Sepr, Other};
#endif


#ifndef CHARACTION	 /* also defined in io_streams.h */
#define CHARACTION
struct CharAction {
	Bool	stop;
	Bool	include;
};
#endif

#endif
