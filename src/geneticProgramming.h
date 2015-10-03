	/**
	*	Larissa Fernandes Leijôto <larissa.leijoto@gmail.com>
	*	
	* 	This file is part of genetic programming developed in a first project of natural computing.
	*/
	#include "tree.h"
	
	#ifndef _GENETIC_PROGRAMMING_H_
	#define _GENETIC_PROGRAMMING_H_

	/**
	 *  Gene.
	 */
	struct Individual
	{
		struct Tree *tree; 				 /* Tree that represents the individual.*/
		double fitness;				     /* Fitness value of the individual. 	*/		
	};

	/*
	 * Genome.
	 */
	struct genome
	{
		/* Attributes. */
		double m_rate; /* Mutation rate.    */
		double c_rate; /* Crossover rate.   */
	};
	
	/*
	 * 
	 */
	Individual *Individual_create();
	
	/*
	 * 
	 */
	Individual *Individual_random(Tree *tree);
	
	/*
	 * Destroys an individual.
	 */
	void individual_destroy(Individual *individual);
	
	/*
	 * Mates two individuals.
	 */
	void individual_crossover(Individual *offspring, Individual *mom, Individual *dad);
	
	/*
	 * Mutates a individual.
	 */
	Individual *individual_mutation(Individual *individual);

	/*
	* Selects organisms for mating using tournament.
	*/
	void selection(std::vector< Individual*> &parents, std::vector< Individual*> &pop, unsigned tournament);
	
	/*
	* Generates a new population.
	*/
	void reproduction(genome *g, std::vector<Individual*> &newpop, std::vector<Individual*> &parents);
	
	/*
	* Replaces old population for new population applying elitism and random kill.
	*/
	void replacement(genome *g, std::vector<Individual*> &population,  std::vector< Individual*> &newpopulation);
	
	/*
	 * Genetic Programming.
	 */
	void geneticProgramming(genome *g, unsigned popsize, unsigned ngen);

	/**
	 * Call symbolic regression to find a best curve that fits all the points.
	 */
	void symbolicRegression(unsigned popsize, unsigned ngen);
	
	/**
	 * Prints the individual and his respective nodes.
	 */
	void print_individual(Individual *individual, FILE *logIndividuals);
	
	/**
	 * Evaluates each individual 
	 */
	void individual_evaluate(std::vector< Individual*> &population);
	
	/**
	 *	Calculates mean and standard deviation for a population.
	 */
	void statistics(std::vector< Individual*> &population,  FILE *logStatistc);
	
	/**
	 * Funtion used to make a copy of a Individual.
	 */
	Individual *copy_individual(Individual *individual);
	
	
	
	/* Global parameters. */
	extern unsigned gen;          /* Number of generations elapsed. */
	extern unsigned popsize;      /* Population size.               */
	extern unsigned maxDepth;     /* Depth of each individual.		*/
	extern struct genome *g; 	  /* Genome.                        */

  #endif /* _GENETIC_PROGRAMMING_H_ */
