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
	#include <fstream>  
	#include <sstream>
	#include <iostream> 

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
			bool countColumns = true;
			
			/* Open working file. */
			wfile = fopen(filename, "r");
			if (wfile == NULL)
				error ("cannot open input file");
			
			/* Get number of points. */
			while ((ch = getc(wfile)) != EOF)
			{
				if(ch == ' ' and countColumns)			
					database.ncolunms++;
				
				if (ch == '\n')
				{
					database.npoints++;
					countColumns = false;
				}
			}
			database.ncolunms++;
			fclose(wfile);
			
		/* Invalid number of points. */
		if (database.npoints == 0)
			error("invalid number of points");
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
		
		//char *line; 			/* Working line.      */
		FILE *wfile;			/* Working file.      */
		unsigned i = 0, j = 0;  /*Loop index.		  */
		ifstream inFile;
	
		
		/* Allocate database. */
		database.points = (double **)smalloc(database.npoints*sizeof(double *));
		for (i = 0; i < database.npoints; i++)
			database.points[i] = (double *)scalloc(database.ncolunms,sizeof(double));

		/* Open working file.*/
		inFile.open(filename, ios::in);
			if (!inFile)
				error ("cannot open input file");
		
		i = 0;
		
		for(i =0 ; i< database.npoints; i++)
		{
			for(j = 0; j < database.ncolunms; j++)
			{
				inFile >> database.points[i][j];
			}
		}
		
		inFile.close();

		 
	}

	/**
	 * @brief Destroys the database.
	 * 
	 * @details Destroys the database, freeing underlying resources.
	 */
	void database_destroy()
	{	
		unsigned i; /** Loop index.*/

		for(i = 0; i < database.npoints;i++)
			free(database.points[i]);
		
		free(database.points);	
	}

	/**
	 * @brief Print the database.
	 * 
	 * @details Prints the database, when is necessary.
	 */

	void print_base(void)
	{
		unsigned i, j; /* Loop index */
		
		for(i = 0; i < database.npoints;i++)
		{
			for( j = 0; j < database.ncolunms;j++)
			{
				fprintf(stderr, "%.2f ", database.points[i][j]);
			}
			fprintf(stderr, "\n");
		}
		
	}
