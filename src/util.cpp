	#include "util.h"
	#include <stdlib.h>
	#include <stdio.h>
	#include <math.h>
	#include <string.h>
	#include <assert.h>


	using namespace std;

	/* Default generator values.*/
	#define DEFAULT_W 521288629
	#define DEFAULT_Z 362436069

	/*
	 * End of line character.
	 */
	char eol = '\n';

	/*
	 * Current random number generator state.
	 */
	static struct
	{
		unsigned int w;
		unsigned int z;
	} curr = {
		DEFAULT_W,
		DEFAULT_Z
	};

	/*
	 * Sets a seed value for the random number generator.
	 */
	void srandnum(int seed) 
	{
		unsigned n1, n2;
		
		n1 = (seed * 104623) % (RANDNUM_MAX);
		curr.w = (n1) ? n1 : DEFAULT_W;
		n2 = (seed * 48947) % (RANDNUM_MAX);
		curr.z = (n2) ? n2 : DEFAULT_Z;
	}

	/*
	 * Generates a random number between 0 and 1.
	 */
	unsigned randnum(void)
	{
		unsigned num;
	  
		curr.z = 36969 * (curr.z & 65535) + (curr.z >> 16);
		curr.w = 18000 * (curr.w & 65535) + (curr.w >> 16);
		num = (curr.z << 16) + curr.w;
		
		return (num);
	}


	/*
	 * Safe calloc().
	 */
	void *scalloc(size_t nmemb, size_t size)
	{
		void *ptr;
		
		ptr = calloc(nmemb, size);
		
		/* Failed to allocate memory. */
		if (ptr == NULL)
			error("cannot malloc()");
		
		return (ptr);	
	}

	/*
	 * Safe malloc().
	 */
	void *smalloc(size_t size)
	{
		void *ptr;
		
		ptr = malloc(size);
		
		/* Failed to allocate memory. */
		if (ptr == NULL)
			error("cannot malloc()");
		
		return (ptr);	
	}

	/*
	 * Safe realloc().
	 */
	void *srealloc(void *ptr, size_t size)
	{	
		ptr = realloc(ptr, size);
		
		/* Failed to allocate memory. */
		if (ptr == NULL)
			error("cannot realloc()");
		
		return (ptr);	
	}

	/*
	 * Prints a warning message.
	 */
	void warning(const char *msg)
	{
		fprintf(stderr, "warning: %s\n", msg);
	}


	/*
	 * Reads a line from a file.
	 */
	char *readline(FILE *input)
	{
		int n, length;
		char *s1, *s2, c;
		
		n = 0;
		length = 80;
		
		s1 = (char *)smalloc((length + 1)*sizeof(char));

		/* Read line. */
		while (((c = getc(input)) != eol) && (c != EOF))
		{
			/* Resize buffer. */
			if (n == length)
			{
				s2 = (char *)srealloc(s1, length *= 2);
				s1 = s2;
			}
			
			s1[n++] = c;
		}
		
		/* Extract line. */
		s1[n] = '\0';
		s2 = (char *)malloc((length + 1)*sizeof(char));
		strcpy(s2, s1);
		free(s1);
		
		return (s2);
	}


	/*
	 * Generates a normal number.
	 */
	double normalnum(double mu, double sigma)
	{
		double U1, U2, W, mult;
		static double X1, X2;
		static int call = 0;
	 
		if (call == 1)
		{
			call = !call;
			return (mu + sigma * (double) X2);
		}
	 
		do
		{
			U1 = -1 + ((double) randnum() / RAND_MAX) * 2;
			U2 = -1 + ((double) randnum() / RAND_MAX) * 2;
			W = pow (U1, 2) + pow (U2, 2);
		} while (W >= 1 || W == 0);
	 
		mult = sqrt ((-2 * log (W)) / W);
		X1 = U1 * mult;
		X2 = U2 * mult;
	 
		call = !call;
	 
		return (mu + sigma * (double) X1);
	}

	/*
	 * Prints an error message and exits.
	 */
	void error(const char *msg)
	{
		fprintf(stderr, "error: %s\n", msg);
		exit(EXIT_FAILURE);
	}


	/*
	 * Set end of line character.
	 */
	char seteol(char c)
	{
		char old;
		
		old = eol;
		eol = c;
		
		return (old);
	}
