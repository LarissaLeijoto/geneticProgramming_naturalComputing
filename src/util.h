	/**
	*	Larissa Fernandes Leijôto <larissa.leijoto@gmail.com>
	*	
	* 	This file is part of genetic programming developed in a first project of natural computing.
	*/
	#ifndef UTIL_H_
	#define UTIL_H_

		#include <stdio.h>
		#include <stdlib.h>

	/*============================================================================*
	 *                               Private Interface                            *
	 *============================================================================*/
		
		extern char eol;

	/*============================================================================*
	 *                                Public Interface                            *
	 *============================================================================*/
		
		/* Logic constants. */
		#define FALSE 0 /* False. */
		#define TRUE 1  /* True.  */

		/* Maximum random number. */
		#define RANDNUM_MAX (4294967295u)
		
				
		/*
		 * Prints an error message and exits.
		 */
		extern void error(const char *msg);
		
		/*
		 * Generates a normal number.
		 */
		extern double normalnum(double mu, double sigma);
		
		/*
		 * Generates a random number between 0 and 1.
		 */
		extern unsigned randnum(void);
		
		/*
		 * Reads a line from a file.
		 */
		extern char *readline(FILE *input);
		
		/*
		 * Safe calloc().
		 */
		extern void *scalloc(size_t nmemb, size_t size);
		
		/*
		 * Set end of line character.
		 */
		extern char seteol(char c);
		
		/*
		 * Safe malloc().
		 */
		extern void *smalloc(size_t size);
		
		/*
		 * Sets a seed value for the random number generator.
		 */
		extern void srandnum(int seed);
		
		/*
		 * Safe realloc().
		 */
		extern void *srealloc(void *ptr, size_t size);
		
		/*
		 * Swaps two objects.
		 */
		#define swap(a, b, t)        \
			{ (t) = (a); (a) = (b); (b) = (t); } \
		
		/*
		 * Prints a warning message.
		 */
		extern void warning(const char *msg);
		
		/*
		 * Returns the maximum of two numbers.
		 */
		#define MIN(a, b) \
			(((a) < (b)) ? (a) : (b))
		
		/*
		 * Set unused parameter.
		 */
		#define UNUSED(x) \
			((void)(x))
		
		/*
		 * Double caster.
		 */
		#define DOUBLE(x) \
			((double)(x))
		
		/*
		 * Double pointer caster.
		 */
		#define DOUBLEP(x) \
			((double *)(x))
		
		/*
		 * Integer caster.
		 */
		#define INT(x) \
			((int)(x))
		
		/*
		 * Integer pointer caster.
		 */
		#define INTP(x) \
			((int *)(x))

	#endif /* UTIL_H_ */
