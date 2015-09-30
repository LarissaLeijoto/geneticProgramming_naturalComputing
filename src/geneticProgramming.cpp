	/**
	*	Larissa Fernandes Leijôto <larissa.leijoto@gmail.com>
	*	
	* 	This file is part of genetic programming developed in a first project of natural computing.
	*/

	#include <cmath>        // std::abs
	#include <stdlib.h>
	#include <string.h>
	#include <omp.h>
	#include <time.h>
	#include <cmath>
	#include <assert.h>
	#include <string>
	#include <vector>
	#include <iostream>     // std::cout
	#include <algorithm>    // std::shuffle
	#include <random>       // std::default_random_engine
	#include <chrono>       // std::chrono::system_clock
	#include "util.h"
	#include "geneticProgramming.h"
	#include "database.h"
	#include "tree.h"

	using namespace std;

	
	#define dbg 0
	#define time_program 0
	#define time_ga 0
	#define saveFile 0


	/**
	 * @brief Casts a individual.
	 */
	#define INDIVIDUAL(x) ((struct Individual *)(x))

	bool ordenation (const Individual *one, const Individual *two) 
	{return (abs(INDIVIDUAL(one)->fitness) < abs(INDIVIDUAL(two)->fitness));}
	

	/**
	 * @brief Sanity check for genetic_algorithm().
	 */
	#define SANITY_CHECK()         \
		assert(popsize > 0);       \
		assert(ngen > 0);          \
		assert(g->m_rate >= 0.0);  \
		assert(g->m_rate <= 1.0);  \
		assert(g->c_rate >= 0.0);  \
		assert(g->c_rate <= 1.0);  \
		assert(g->e_rate > 0.0);   \
		assert(g->e_rate < 1.0);   \

	/*============================================================================*
	 *                              genetic Operators                             *
	 *============================================================================*/
	
	/**
	 * @brief creates a Individual.
	 * 
	 * @return A Individual.
	 */
	Individual *Individual_create()
	{
		Individual * individual = (Individual *)smalloc(sizeof(Individual));
		individual->fitness = 0;
		
		return individual;		
	}
	
	/**
	 * @brief creates a random Individual.
	 * 
	 * @return A random Individual.
	 */
	Individual *Individual_random(unsigned maxdepth, unsigned method)
	{	
		#if(dbg>0)
		fprintf(stderr, "info: Individual random\n");
		#endif
		
		struct Individual *individual = Individual_create();
		INDIVIDUAL(individual)->tree = create_tree();
		
		if(method == 1)
			Full(INDIVIDUAL(individual)->tree, maxdepth);
		else
			Grow(INDIVIDUAL(individual)->tree, maxdepth);
		
		 
		 return individual;		
	}
	
	
	/**
	 * @brief Destroys a Individual.
	 */
	void individual_destroy(Individual *individual)
	{
		
	}



	/**
	 * @brief Replaces old population for new population applying elitism and random kill.
	 * 
	 * 
	 */
	void replacement(genome *g, vector<Individual*> &population, vector<Individual*> &newpopulation)
	{
		/* Sanity check */
		assert(!population.empty());
		assert(!newpopulation.empty());
		
		
	}

	/**
	 * @brief Selects organisms for mating using tournament.
	 */
	 void selection(vector<Individual *> &parents, vector< Individual *> &pop, unsigned tournament)
	{
		unsigned i = 0, j = 0; /* Loop index*/
		vector<Individual *> ranking;
			
			while(i < popsize)
			{
				j = 0;
				while(j < tournament)
				{
					unsigned pos  = rand()%popsize;
					ranking.push_back(pop[pos]);		
					j++;
				}			
				
				std::sort(ranking.begin(),ranking.end(),ordenation);
				parents.push_back(ranking[0]);
				ranking.resize(0);
								
				i++;
			}
	}

	/**
	 * @brief creates a new population.
	 */
	 void reproduction(genome *g, vector<Individual *> &newpop, vector< Individual *> &parents)
	{
		unsigned i = 0;				  /* Loop index.	 */
		Individual *mom;   		 	  /* Mother.       	 */
		Individual *dad;   		  	  /* Father.       	 */
		Individual *offspring1;       /* First offspring.*/	
		Individual *offspring2;       /* First offspring.*/
				
		std::random_shuffle(parents.begin(), parents.end());

		/* Build new population.  */
		for ( i = 0; i < popsize; i += 2)
		{
			if ((rand()/ (double)RAND_MAX ) < g->c_rate)
			{
				mom = parents[i];
				dad = parents[i + 1];
				
					offspring1 = Individual_create();
					offspring2 = Individual_create();
				
					individual_crossover(offspring1, mom, dad);	

				if ((rand()/ (double)RAND_MAX ) < g->m_rate)
					//individual_mutation(offspring1);
					
				newpop.push_back(offspring1);
			}
		}
	
	/* House keeping. */
	for(i = 0 ; i < parents.size(); i++)
			individual_destroy(parents[i]);
		parents.clear();
		
		
	}


	/**
	 * @brief Crossovers two Individuals.
	 * 
	 * @param Individual1 First Individual.
	 * @param Individual2 Second Individual.
	 */
	void individual_crossover(Individual *offspring, Individual *mom, Individual *dad)
	{		
		int cutoffPointMom = 0;
		int cutoffPointDad = 0;

		cutoffPointMom = (rand()%mom->tree->countNodes)+1;
		cutoffPointDad = (rand()%dad->tree->countNodes)+1;
			
		Tree *momTree = mom->tree,
			 *dadTree  = dad->tree, 
			 *childTree = offspring->tree;
		
		/* Criando a arvore para que possa alocar o novo individuo gerado a partir do cruzamento entre pai e mãe */
		childTree = create_tree();
		
		/* Alocando o nó raiz para que possa alocar o resto da arvore*/
		childTree->root = create_ExpNode(0);
		
		/* Copiando o conteudo do nó da mãe para o filho*/
		childTree->root->copy_node(momTree->root);
		
		/* Copia até achar o cutoff point e retorna o nodo de cutoff*/
		ExpNode *cutoffMommy = copy_Individual(childTree->root, momTree->root, cutoffPointMom); 
		
		/* Encontra o ponto de crossover da arvore do pai */
		ExpNode *cutoffDaddy = Find_cutoff(dadTree->root, cutoffPointDad);
				
		if(cutoffDaddy == NULL)
			return;
		if(cutoffMommy == NULL)
			return;
	
		/* Copiando o conteudo do nó do pai para o nó de cutoff da mãe*/
		cutoffMommy->copy_node(cutoffDaddy);		
		
		/* Copia do cutoff daddy para o cutoff mommy */
		copy_Individual(cutoffMommy, cutoffDaddy, -1);
	}

	/**
	 * @brief Mutates a Individual.
	 * 
	 * @return The mutated Individual.
	 */
	void individual_mutation(Individual *individual)
	{	
		
	}
	
	/**
	 * @brief Evaluates the fitness of a Individual.
	 */
	void individual_evaluate(vector<Individual*> &population)
	{  
		for(unsigned i = 0; i < population.size(); i++)
		{
			double sum = 0;  
			for(unsigned j = 0; j < database.npoints; j++)
			{  
				sum += (getValue(INDIVIDUAL(population[i])->tree->root, database.points[j][0])- database.points[j][1] );  
			}  
			
			INDIVIDUAL(population[i])->fitness = sum;
		}
	}

	/*============================================================================*
	 *                              genetic Definitions                           *
	 *============================================================================*/
	/**
	 * @brief Problem's genome.
	*/
	struct genome problem = 
	{
			0.05,           /* Mutation rate.    */
			0.80,           /* Crossover rate.   */
			0.01,           /* Elitism rate.     */
			1.00           	/* Replacement rate. */
	};

	/**
	 * @brief genetic programming for symbolic regression.
	*/
	void symbolicRegression(unsigned popsize, unsigned ngen)
	{
			
			geneticProgramming(&problem, popsize, ngen);

	} 

	/*============================================================================*
	 *                              genetic utilities                             *
	 *============================================================================*/
	/**
	 * @brief 
	*/
	void print_individual(Individual *individual, FILE *logIndividuals)
	{
		
		
	}

	/**
	 * @brief
	 */
	void statistics(vector< Individual*> &population, double &best, unsigned &count, FILE *logStatistic)
	{	
		
		
	}

	/*============================================================================*
	 *                              genetic Programming                           *
	 *============================================================================*/
	 /**
	 * @brief
	 */
	void geneticProgramming(genome *g, unsigned popsize, unsigned ngen)
	{
		
		#if(dbg>0)
		fprintf(stderr,"popsize: %d\n", popsize);
		fprintf(stderr,"ngen: %d\n", ngen);
		fprintf(stderr,"Crossover rate: %.2f\n", g->c_rate);
		fprintf(stderr,"Elitism rate: %.2f\n", g->e_rate);
		fprintf(stderr,"Repalcement rate: %.2f\n", g->r_rate);
		fprintf(stderr,"Mutation rate: %.2f\n", g->m_rate);
		#endif	
		
		FILE *logStatistic;
		FILE *logIndividuals;
		FILE *logBests;

		vector<Individual*> population;					/* Current population.    						 */
		vector<Individual*> newpopulation;       		/* New population.								 */
		vector<Individual*> parents;					/* Mating pool.	     							 */ 
		vector<Individual*> bestExecutions;				/* Many executions for statistics results.		 */
		unsigned i = 0;									/* Loop index.			  						 */
		double best;									/* Best solution found.  						 */
		unsigned count = 0;								/* Time that the GA not improve.				 */
		double sum = 0; 								/* Sum of fitness of each individual. 			 */
		double mean = 0;								/* Mean of population's fitness.     			 */
		double stddev = 0;								/* Standart desviation of population's fitness.  */
		
		double time_in_seconds_evaluation = 0;
		double time_in_seconds_selection = 0;
		double time_in_seconds_reproduction = 0;
		double time_in_seconds_replacement =0;
		
		logStatistic = fopen("statistic.txt", "w");
		logIndividuals = fopen("individuals.txt", "w");
		logBests = fopen("bests.txt","w");
		
		SANITY_CHECK();

		//for(unsigned stat = 0; stat < 1;stat++)
		//{
			
			/* Build initial population. */
			for(unsigned i = 0; i < popsize;i++ )
				population.push_back(Individual_random(maxDepth,1));
				
			
			for(unsigned i = 0; i < 1; i++)
			{

				fprintf(stderr, "generation: %d\n", i);

				/**Evaluate the new population*/
				clock_t start_time_evaluation = clock();
				individual_evaluate(population);
				time_in_seconds_evaluation += (clock() - start_time_evaluation) / (double)CLOCKS_PER_SEC;
			
				/* Print population statistics.*/
				//statistics(population, best, logStatistic);
					
				/* Select individuals for reproduction.*/
				clock_t start_time_selection = clock();
				selection(parents, population, 2);		
				time_in_seconds_selection += (clock() - start_time_selection) / (double)CLOCKS_PER_SEC;

				/* Crossover and mutation.*/
				clock_t start_time_reproduction = clock();
				reproduction(g, newpopulation, parents);
				time_in_seconds_reproduction += (clock() - start_time_reproduction) / (double)CLOCKS_PER_SEC;
				
				/* Replaces the old population by the new.*/
				clock_t start_time_replacement = clock();
				//replacement(g, population, newpopulation);
				time_in_seconds_replacement += (clock() - start_time_replacement) / (double)CLOCKS_PER_SEC;
				
			}		

			
		//}
		
		#if(time_ga>0)
		fprintf(logBests, "Evaluation: %.2f\n",time_in_seconds_evaluation/30);
		fprintf(logBests, "Selection: %.2f\n",time_in_seconds_selection/30);
		fprintf(logBests, "Reproduction: %.2f\n",time_in_seconds_reproduction/30);
		fprintf(logBests, "Replacement: %.2f\n",time_in_seconds_replacement/30);
		#endif

	}
