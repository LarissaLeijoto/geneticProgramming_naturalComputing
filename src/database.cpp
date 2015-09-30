	/**
	*	Larissa Fernandes Leijôto <larissa.leijoto@gmail.com>
	*	
	* 	This file is part of genetic programming developed in a first project of natural computing.
	*/
	#include <assert.h>
	#include <stdio.h>
	#include <string.h>
	#include <string>
	#include <stdlib.h>
	#include <cstdio>
	#include <cstdlib>
	#include <limits.h>
	#include <time.h>
	#include <vector>

	#include "database.h"
	#include "util.h"


	#define dbg 0
	#define time_program 0

	using namespace std;

	/**
	 * @brief Parses the database.
	 *
	 * @details Parses the database so that I can count the number of lines.
	 *
	 * @param filename Name of input files.
	 *
	 * @returns The number of points.
	 * 
	 * @todo Check for bad file format.
	 */
	void database_parse(const char *filename)
	{
		/* Sanity checks. */
		assert(filename != NULL);

			char ch;     /* Working character. */
			FILE *wfile; /* Working file.      */
			
			/* Open working file. */
			wfile = fopen(filename, "r");
			if (wfile == NULL)
				error ("cannot open input file");
			
			/* Get number of points. */
			while ((ch = getc(wfile)) != EOF)
			{
				if (ch == '\n')
					database.npoints++;
			}
			fclose(wfile);
			
		/* Invalid number of points. */
		if (database.npoints == 0)
			error("invalid number of amino acids");
	}

	/**
	 * @brief Reads the database into memory.
	 *
	 * @details Reads the database into memory so we can speedup computation.
	 *
	 * @param filenames   Name of input files.
	 *
	 * @returns The database.
	 * 
	 * @todo Check for bad file format.
	 */
	void database_read(const char *filename)
	{
		/* Sanity check. */
		assert(filename != NULL);
		
		char *line; 	/* Working line.      */
		FILE *wfile;	/* Working file.      */
		char *token;    /* Working token.     */
		unsigned i = 0; /*Loop index.		  */
		
		/* Allocate database. */
		database.points = (double **)smalloc(database.npoints*sizeof(double *));
		for (unsigned i = 0; i < database.npoints; i++)
			database.points[i] = (double *)smalloc(2*sizeof(double));
			
		/* Open working file. */
		wfile = fopen(filename, "r");
			if (wfile == NULL)
				error ("cannot open input file");
		
		
			while (!feof(wfile) & i < database.npoints)
			{
				line = readline(wfile);	
				sscanf(line,"%lf %lf",&database.points[i][0],&database.points[i][1]);
				i++;		
			}
		
	}

	/**
	 * @brief Destroys the database.
	 * 
	 * @details Destroys the database, freeing underlying resources.
	 */
	void database_destroy()
	{	
		unsigned i; /** Loop index.*/
		clock_t start_time = 0;
		start_time = clock();

		for(i = 0; i < database.npoints;i++)
			free(database.points[i]);
		
		free(database.points);
		
		#if(time_program>0)
		double time_in_seconds = (clock() - start_time) / (double)CLOCKS_PER_SEC;
		fprintf(stderr, "database destroy: %.2f\n",time_in_seconds );
		#endif 
		
	}

	/**
	 * @brief Print the database.
	 * 
	 * @details Prints the database, when is necessary.
	 */

	void print_base(void)
	{
		unsigned i; /* Loop index */
		for(i = 0; i < database.npoints;i++)
			fprintf(stderr, "%lf, %lf\n", database.points[i][0], database.points[i][1]);
		
	}
