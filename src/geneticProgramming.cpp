	/**
	*	Larissa Fernandes Leijôto <larissa.leijoto@gmail.com>
	*	
	* 	This file is part of genetic programming developed in a first project of natural computing.
	*/

	#include <stdlib.h>
	#include <string.h>
	#include <omp.h>
	#include <time.h>
	#include <cmath>
	#include <math.h>       /* isnan, sqrt */
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
	#define time_ga 1
	#define saveFile 0


	bool ordenation (const Individual *one, const Individual *two) 
	{return (abs(one->fitness) < abs(two->fitness));}
	

	/**
	 * @brief Sanity check for genetic Programming().
	 */
	#define SANITY_CHECK()         \
		assert(popsize > 0);       \
		assert(ngen > 0);          \
		assert(g->m_rate >= 0.0);  \
		assert(g->m_rate <= 1.0);  \
		assert(g->c_rate >= 0.0);  \
		assert(g->c_rate <= 1.0);  \

	/*============================================================================*
	 *                              genetic Operators                             *
	 *============================================================================*/
	
	/**
	 * @brief creates a Individual.
	 * 
	 * @return A new individual.
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
	 * @return A random individual, creates by method FULL or GROW.
	 */
	Individual *Individual_random(unsigned maxdepth, unsigned method)
	{	
		Individual *individual = Individual_create();
		individual->tree = create_tree();
		
		if(method == 1)
			Full(individual->tree, maxdepth);
		else
			Grow(individual->tree, maxdepth);

		 return individual;		
	}
	
	/**
	 * @brief Destroys a Individual.
	 * 
	 * @param Individual in which memory will be freed.
	 */
	void individual_destroy(Individual *individual)
	{
		tree_destroy(individual->tree->root);
		free(individual->tree);
		free(individual);	
	}
	
	/**
	 * @brief Make a copy of a Individual.
	 * 
	 * @param Individual that will be copied.
	 */
	Individual *copy_individual(Individual *individual)
	{
		Individual *newIndividual = Individual_create();
		
		newIndividual->tree = create_tree();
		newIndividual->tree->root = copy_tree(individual->tree->root);
		newIndividual->tree->countNodes = individual->tree->countNodes;
		newIndividual->fitness = individual->fitness;
				
		return newIndividual;		
	}

	/**
	 * @brief Replaces old population for new population applying elitism and random kill.
	 * 
	 * @param Old population.
	 * @param New population.
	 */
	void replacement(vector<Individual*> &population, vector<Individual*> &newpopulation)
	{
		/* Sanity check */
		assert(!population.empty());
		assert(!newpopulation.empty());
		
		vector<Individual*> nextpopulation;
			
		unsigned i = 0;		 /* Loop index.					 */
		unsigned nelite = 1; /* Number of elite individuals.  */
			
		/* Extract elite organisms.*/
		std::sort(population.begin(),population.end(),&ordenation);
		std::random_shuffle(newpopulation.begin(), newpopulation.end());
		
		/* Insert elite organisms.*/
		for(i = 0; i < nelite; i++)
			nextpopulation.push_back(copy_individual(population[i]));

		/* Insert the individuals that still lack in the population*/
		while (!newpopulation.empty() and newpopulation.size() < popsize)
		{
			nextpopulation.push_back(copy_individual(newpopulation.back()));
			individual_destroy(newpopulation.back());
			newpopulation.pop_back();
		}
		
		std::random_shuffle(population.begin(), population.end());
		
		for(i = nextpopulation.size(); i < popsize; i++)
			nextpopulation.push_back(copy_individual(population.back()));
	
				
		/* Discard remaining new organisms. */
		for(i = 0 ; i < newpopulation.size(); i++)
			individual_destroy(newpopulation[i]);
		if(!newpopulation.empty())
			newpopulation.resize(0);

		/* Discard all the old organisms. */
		for(i = 0 ; i < population.size(); i++)
			individual_destroy(population[i]);
		if(!population.empty())
		population.resize(0);

		/* Swap the old population for the new */
		for(i = 0; i < nextpopulation.size(); i++)
			population.push_back(copy_individual(nextpopulation[i]));
			
		/* Discard organisms. */
		for(i = 0 ; i < nextpopulation.size(); i++)
			individual_destroy(nextpopulation[i]);
		if(!nextpopulation.empty())
			nextpopulation.resize(0);
	}

	/**
	 * @brief Selects organisms for mating using tournament.
	 * 
	 * @param parents that will be generated.
	 * @param population that will be used to do the selection.
	 */
	 void selection(vector<Individual *> &parents, vector< Individual *> &pop, unsigned tournament)
	{
		/* Sanity check */
		assert(!pop.empty());
		
		unsigned i = 0, j = 0; /* Loop index*/
		vector<Individual *> ranking;
			
			while(i < popsize)
			{
				j = 0;
				while(j < tournament)
				{
					unsigned pos  = rand()%popsize;
					ranking.push_back(copy_individual(pop[pos]));		
					j++;
				}			
				
				std::sort(ranking.begin(),ranking.end(),ordenation);
				parents.push_back(copy_individual(ranking[0]));
				
				for(unsigned w = 0; w < ranking.size(); w++)
					individual_destroy(ranking[w]);
				ranking.resize(0);
									
				i++;
			}
	}

	/**
	 * @brief creates a new population.
	 * 
	 * @param new population that will be generated from the parents.
	 * @param parents that will generate children.
	 */
	 void reproduction(genome *g, vector<Individual*> &newpop, vector<Individual *> &parents)
	{
		/* Sanity check */
		assert(!parents.empty());		
		
		unsigned i = 0;				  /* Loop index.	*/
		Individual *mom;   		 	  /* Mother.       	*/
		Individual *dad;   		  	  /* Father.       	*/
		Individual *offspring;        /* Offspring.		*/	
			
		std::random_shuffle(parents.begin(), parents.end());

		/* Build new population.  */
		for ( i = 0; i < popsize; i += 2)
		{
			if ((rand()/ (double)RAND_MAX ) < g->c_rate)
			{
				mom = parents[i];
				dad = parents[i + 1];

				offspring = Individual_create();
				individual_crossover(offspring, mom, dad);	
				
				offspring->tree->countNodes = offspring->tree->root->assignId(1);
				
				if ((rand()/ (double)RAND_MAX ) < g->m_rate)
					offspring = individual_mutation(offspring);
				
				newpop.push_back(copy_individual(offspring));
				individual_destroy(offspring);						
			}
		}

		/* House keeping. */
		for(i = 0 ; i < parents.size(); i++)
				individual_destroy(parents[i]);
			parents.resize(0);
	}


	/**
	 * @brief Crossovers two Individuals.
	 * 
	 * @param Child that will be generated from  mom and dad.
	 * @param Mom used to mate with dad.
	 * @param Dad used to mate with mom.
	 */
	void individual_crossover(Individual *offspring, Individual *mom, Individual *dad)
	{	
		mom->tree->countNodes = mom->tree->root->assignId(1);
		dad->tree->countNodes = dad->tree->root->assignId(1);
		
		int cutoffPointMom = 0;
		int cutoffPointDad = 0;
		
		do
		{
			cutoffPointMom = (rand()% mom->tree->countNodes)+1;
			cutoffPointDad = (rand()% dad->tree->countNodes)+1;
			
		}while(cutoffPointDad == cutoffPointMom);

		/* Criando a arvore para que possa alocar o novo individuo gerado a partir do cruzamento entre pai e mãe */
		offspring->tree = create_tree();
			
		/* Alocando o nó raiz para que possa alocar o resto da arvore*/
		offspring->tree->root = create_ExpNode(0);
		
		/* Copiando o conteudo do nó da mãe para o filho*/
		(offspring->tree->root)->copy_node(mom->tree->root);
		
		/* Copia até achar o cutoff point e retorna o nodo de cutoff*/
		ExpNode *cutoffMommy = copy_Individual(offspring->tree->root, mom->tree->root, cutoffPointMom); 
		
		/* Encontra o ponto de crossover da arvore do pai */
		ExpNode *cutoffDaddy = Find_cutoff(dad->tree->root, cutoffPointDad);

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
	Individual *individual_mutation(Individual *individual)
	{	
		Individual *randomIndividual;
		Individual *newIndividual = Individual_create();
			
		if((rand()/ (double)RAND_MAX )  < 0.5)
			randomIndividual = Individual_random(maxDepth,1);
		else
			randomIndividual = Individual_random(maxDepth,2);
		
		
		individual_crossover(newIndividual, individual, randomIndividual);
		
		newIndividual->tree->countNodes = newIndividual->tree->root->assignId(1);

		/* House Keeping*/
		individual_destroy(randomIndividual);
		individual_destroy(individual);
		
		return newIndividual;
	}	
	/**
	 * @brief Evaluates the fitness of a Individual.
	 * 
	 * @param Population in which will calculate the fitness of each individual.
	 */
	void individual_evaluate(vector<Individual*> &population)
	{  
		for(unsigned i = 0; i < population.size(); i++)
		{
			double residual = 0;  
			
			for(unsigned j = 0; j < database.npoints; j++)
				residual += abs((getValue(population[i]->tree->root, database.points[j][0])- database.points[j][1]));  		
	
			population[i]->fitness = residual;
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
			0.01,           /* Mutation rate.    */
			0.90,           /* Crossover rate.   */
	};

	/**
	 * @brief genetic programming for symbolic regression.
	 * 
	 * @param Population size
	 * @param Number of generation to evolve the popualtion.
	*/
	void symbolicRegression(unsigned popsize, unsigned ngen)
	{		
		geneticProgramming(&problem, popsize, ngen);
	} 

	/**
	 * @brief Calculate statistics needed to assess the algorithm.
	 * 
	 * @param Population in which will be calculated the statistics.
	 * @param File that will be used to store information.
	 */
	void statistics(vector< Individual*> &population, FILE *logStatistic)
	{	
		
		double sum = 0; 	/* Sum of fitness of each individual. 				 */
		double mean = 0;	/* Mean of population's fitness.     				 */
		double stddev = 0;	/* Standart desviation of population's fitness.      */
		unsigned i;			/* Loop index.										 */
		
		std::sort(population.begin(),population.end(),ordenation);

		fprintf(logStatistic,"%.2f;",population[0]->fitness); /* Máximo */
		fprintf(logStatistic,"%.2f;",population[popsize-1]->fitness); /* Minimo*/
		
		for(unsigned w = 0; w < population.size(); w++)
			fprintf(stderr,"%.2f\n",population[w]->fitness);
		
		for(i = 0; i < population.size(); i++)
			sum += population[i]->fitness;
			
		mean = sum/population.size();
		
		fprintf(logStatistic,"%2.f;",mean);
		
		sum = 0;
		
		for(i = 0; i < population.size(); i++)
			sum += pow((population[i]->fitness - mean),2);
			
		stddev = sum/population.size();		
		
		fprintf(logStatistic,"%2.f",stddev);
		fprintf(logStatistic,"\n");
			
	}

	/*============================================================================*
	 *                              genetic Programming                           *
	 *============================================================================*/
	 /**
	 * @brief Genetic Programming 
	 * 
	 * @param Genome used to define the algorithm params.
	 * @param Population size.
	 * @param Number of genetations.
	 */
	void geneticProgramming(genome *g, unsigned popsize, unsigned ngen)
	{
		#if(dbg>0)
		fprintf(stderr,"popsize: %d\n", popsize);
		fprintf(stderr,"ngen: %d\n", ngen);
		fprintf(stderr,"Crossover rate: %.2f\n", g->c_rate);
		fprintf(stderr,"Mutation rate: %.2f\n", g->m_rate);
		#endif	
		
		FILE *logStatistic;
		FILE *logBests;
		FILE *logtime;

		vector<Individual*> population;					/* Current population.    						 */
		vector<Individual*> newpopulation;       		/* New population.								 */
		vector<Individual*> parents;					/* Mating pool.	     							 */ 
		vector<Individual*> bestExecutions;				/* Many executions for statistics results.		 */
		unsigned i = 0;									/* Loop index.			  						 */
		double best;									/* Best solution found.  						 */
		double mean = 0;								/* Mean of population's fitness.     			 */
		double stddev = 0;								/* Standart desviation of population's fitness.  */
		double sum = 0; 								/* Sum of fitness of each individual. 			 */

		double time_in_seconds_evaluation = 0;
		double time_in_seconds_selection = 0;
		double time_in_seconds_reproduction = 0;
		double time_in_seconds_replacement =0;
		
		logStatistic = fopen("Statistic.txt", "w");
		logBests = fopen("bests.txt","w");
		logtime = fopen("time.txt","w");
		
		/* Header of file*/
		fprintf(logStatistic,"Bests;\n"); 
		
		SANITY_CHECK();

		for(unsigned stat = 0; stat < 30;stat++)
		{
			/* Build initial population. */
			for(unsigned i = 0; i < popsize;i++ )
				population.push_back(Individual_random(maxDepth,1));
				
			for(unsigned i = population.size(); i < popsize;i++ )
				population.push_back(Individual_random(maxDepth,2));
				
			/**Evaluate the new population*/
				clock_t start_time_evaluation = clock();
				individual_evaluate(population);
				time_in_seconds_evaluation += (clock() - start_time_evaluation) / (double)CLOCKS_PER_SEC;	
			
						
			for(unsigned i = 0; i < popsize; i++)
			{
				//fprintf(stderr, "generation: %d\n", i);
				
				/* Print population statistics.*/
				//statistics(population, logStatistic);
		
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
				replacement(population, newpopulation);
				time_in_seconds_replacement += (clock() - start_time_replacement) / (double)CLOCKS_PER_SEC;	
				
				/**Evaluate the new population*/
				clock_t start_time_evaluation = clock();
				individual_evaluate(population);
				time_in_seconds_evaluation += (clock() - start_time_evaluation) / (double)CLOCKS_PER_SEC;
				
			}	
			
			/* Store the best individual of a execution. */
			fprintf(stderr,"#Execution Finish: %d\n",stat);
			std::sort(population.begin(),population.end(),ordenation);
			bestExecutions.push_back(copy_individual(population[0]));
				
			/* House Kepping.*/
			for(unsigned w = 0; w < population.size(); w++)
				individual_destroy(population[w]);
			population.resize(0);

		
		}
		
		#if(time_ga>0)
		fprintf(logtime, "Evaluation: %.2f\n",time_in_seconds_evaluation/30);
		fprintf(logtime, "Selection: %.2f\n",time_in_seconds_selection/30);
		fprintf(logtime, "Reproduction: %.2f\n",time_in_seconds_reproduction/30);
		fprintf(logtime, "Replacement: %.2f\n",time_in_seconds_replacement/30);
		#endif
				
		for(i = 0; i < bestExecutions.size(); i++)
		fprintf(logStatistic,"%.2f\n",bestExecutions[i]->fitness);
		
		for(i = 0; i < bestExecutions.size(); i++)
			sum += bestExecutions[i]->fitness;
		
			
		mean = sum/bestExecutions.size();
		
		fprintf(logBests,"Mean: %f\n",mean);
			
		sum = 0;
		
		for(i = 0; i < bestExecutions.size(); i++)
			sum += pow((bestExecutions[i]->fitness - mean),2);
			
		stddev = sum/bestExecutions.size();		
		fprintf(logBests,"Standard Desviation: %f\n",stddev);

		/* House Kepping.*/
		for(i = 0 ; i < bestExecutions.size(); i++)
				individual_destroy(bestExecutions[i]);
			bestExecutions.clear();
			
		fclose(logStatistic);
		fclose(logBests);
		
		
	}
