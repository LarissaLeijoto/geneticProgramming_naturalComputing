	/**
	*	Larissa Fernandes Leijôto <larissa.leijoto@gmail.com>
	*	
	* 	This file is part of genetic programming developed in a first project of natural computing.
	*/

	#include <stdio.h>
	#include <stdlib.h>
	#include <string>
	#include <time.h>
	#include <vector>
	#include <time.h>
	#include <iostream>
	#include <cstdlib>
	#include <cstdio>     
	#include <cstring> 
	#include "util.h"
	#include "database.h"
	#include "tree.h"
	#include "geneticProgramming.h"

	using namespace std;

	
	#define dbg  0
	#define time_program 0

	/* Program parameters. */
	static const char *filename = NULL;  /* Name of input files.              */
	unsigned popsize = 0;		         /* Population size.                  */
	unsigned ngen = 0;          		 /* Number of generations.            */
	unsigned maxDepth = 0;				 /* Initial depth of each individual. */

	/**
	 * @brief Database.
	 */
	struct database database;

	/**
	 * @brief Prints program usage and exits.
	 * 
	 * @details Prints program usage and exits.
	 */
	static void usage(void)
	{
		printf("Usage: genetic algorithm <popsize> <ngen> <depth> <file>");
		exit(EXIT_SUCCESS);
	}

	/**
	 * @brief Reads command line arguments.
	 * 
	 * @details Reads and parses command line arguments pointed to by @p argv.
	 * 
	 * @todo Read and parse command line arguments.
	 */
	static void readargs(int argc, char **argv)
	{		
		/* Missing arguments. */
		if (argc < 4)
			usage();
		
		popsize = atoi(argv[1]);
		ngen = atoi(argv[2]);
		maxDepth = atoi(argv[3]);
		filename = argv[4];
		
			
		/* Assert program parameters. */
		if (popsize == 0)
			error("invalid population size");
		else if (ngen == 0)
			error("invalid number of generations");
		else if (maxDepth == 0)
			error("invalid number of depth");
			
	}
	
	int main(int argc, char **argv)
	{
		
		readargs(argc, argv);
		
		/* Initialization. */
		srand( (unsigned)time(NULL) );
		
		/* Parse database. */
		#if(dbg>0)
		fprintf(stderr, "parsing database... Ok\n");
		#endif
		database_parse(filename);
			
		#if(dbg>0)
		fprintf(stderr, "info: reading database! Ok\n");
		#endif
		/* Read database. */
		database_read(filename);
		
		#if(dbg>0)
		fprintf(stderr, "info: genetic programming! Ok\n");
		#endif
		symbolicRegression(popsize,ngen);
		
		//print_base();
		
		/* House keeping. */
		database_destroy();
		
		return (EXIT_SUCCESS);
	}
	
	
	/*
	 * 			
		Individual *i1 = Individual_create();
		Individual *i2 = Individual_create();
		
		Individual *filho = Individual_create();
		
		i1->tree = create_tree();
		i2->tree = create_tree();
		filho->tree = create_tree();
			
		Full(i1->tree, maxDepth);
		Full(i2->tree, maxDepth);
		
		display(i1->tree->root);
		fprintf(stderr,"\n");				
		display(i2->tree->root);
		
		fprintf(stderr,"\n");
		fprintf(stderr,"\n");
		individual_crossover(filho, i1, i2);
		
		
		fprintf(stderr,"FILHOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
		display(filho->tree->root);
	 */
		
		//fprintf(stderr, "\nRESULT: %.2f",getValue(tree->root, 2));
		//fprintf(stderr, "\n");
		
		//Tree *tree = create_tree();
		//Grow(tree, maxDepth);
		
		//display(tree->root);
