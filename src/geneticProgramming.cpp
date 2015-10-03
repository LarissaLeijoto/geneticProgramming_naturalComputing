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


	bool ordenation (const Individual *one, const Individual *two) 
	{return (abs(one->fitness) < abs(two->fitness));}
	

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
		#if(dbg>0)
		fprintf(stderr, "info: Individual create\n");
		#endif
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
	 */
	void individual_destroy(Individual *individual)
	{
		#if(dbg>0)
		fprintf(stderr, "info: Individual destroy\n");
		#endif
		
		tree_destroy(individual->tree->root);
		free(individual->tree);
		free(individual);
		
	}
	
	/**
	 * @brief Destroys a Individual.
	 */
	Individual *copy_individual(Individual *individual)
	{
		#if(dbg>0)
		fprintf(stderr, "info: Individual copy\n");
		#endif
		
		Individual *newIndividual = Individual_create();
		
		newIndividual->tree = create_tree();
		newIndividual->tree->root = copy_tree(individual->tree->root);
		newIndividual->tree->countNodes = individual->tree->countNodes;
		newIndividual->fitness = individual->fitness;
				
		return newIndividual;		
	}

	/**
	 * @brief Replaces old population for new population applying elitism and random kill.
	 */
	void replacement(genome *g, vector<Individual*> &population, vector<Individual*> &newpopulation)
	{
		#if(dbg>0)
		fprintf(stderr, "info: repalcement\n");
		#endif
		
		/* Sanity check */
		assert(!population.empty());
		assert(!newpopulation.empty());
			
		unsigned i = 0;		 /* Loop index.					 */
		unsigned nelite = 1; /* Number of elite individuals.  */
			
		/* Extract elite organisms.*/
		std::sort(population.begin(),population.end(),&ordenation);
		std::random_shuffle(newpopulation.begin(), newpopulation.end());
		
		/* Insert elite organisms.*/
		for(i = 0; i < nelite; i++)
		{
			if(newpopulation.size() == popsize)
				newpopulation.push_back(copy_individual(population[i]));
			else if(newpopulation.size() < popsize)
				newpopulation.push_back(copy_individual(population[i]));		
		}
		
		/* Insert the individuals that still lack in the population*/
		if(newpopulation.size() < popsize)
		{
			std::random_shuffle(population.begin(), population.end());
			for( i = newpopulation.size(); i < popsize; i++)
			{
				newpopulation.push_back(copy_individual(population.back()));
				individual_destroy(population.back());
				population.pop_back();
			}
		}

		/* Discard all the old organisms. */
		for(i = 0 ; i < population.size(); i++)
			individual_destroy(population[i]);
		population.resize(0);
		
		/* Swap the old population for the new */
		for(i = 0; i < newpopulation.size(); i++)
			population.push_back(copy_individual(newpopulation[i]));
			
		/* Discard remaining new organisms. */
		for(i = 0 ; i < newpopulation.size(); i++)
			individual_destroy(newpopulation[i]);
		newpopulation.resize(0);
	}

	/**
	 * @brief Selects organisms for mating using tournament.
	 */
	 void selection(vector<Individual *> &parents, vector< Individual *> &pop, unsigned tournament)
	{
		#if(dbg>0)
		fprintf(stderr, "info: selection\n");
		#endif
		
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
	 */
	 void reproduction(genome *g, vector<Individual*> &newpop, vector<Individual *> &parents)
	{
		#if(dbg>0)
		fprintf(stderr, "info: reproduction\n");
		#endif
		
		/* Sanity check */
		assert(!parents.empty());		
		
		unsigned i = 0;				  /* Loop index.	*/
		Individual *mom;   		 	  /* Mother.       	*/
		Individual *dad;   		  	  /* Father.       	*/
		Individual *offspring;        /* Offspring.		*/	
		//Individual *mutated;		  /* Anomaly.		*/
		
			
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
	 * @param Individual1 First Individual.
	 * @param Individual2 Second Individual.
	 */
	void individual_crossover(Individual *offspring, Individual *mom, Individual *dad)
	{	
		#if(dbg>0)
		fprintf(stderr, "info: Individual crossover\n");
		#endif
		
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
		#if(dbg>0)
		fprintf(stderr, "info: Individual mutation\n");
		#endif
		
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
	 */
	void individual_evaluate(vector<Individual*> &population)
	{  
		#if(dbg>0)
		fprintf(stderr, "info: Individual evaluate\n");
		#endif
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
			0.05,           /* Mutation rate.    */
			0.90,           /* Crossover rate.   */
	};

	/**
	 * @brief genetic programming for symbolic regression.
	*/
	void symbolicRegression(unsigned popsize, unsigned ngen)
	{		
		geneticProgramming(&problem, popsize, ngen);
	} 

	/**
	 * @brief
	 */
	void statistics(vector< Individual*> &population, FILE *logStatistic)
	{	
		
		double sum = 0; 	/* Sum of fitness of each individual. 				 */
		double mean = 0;	/* Mean of population's fitness.     				 */
		double stddev = 0;	/* Standart desviation of population's fitness.      */
		unsigned i;			/* Loop index.										 */
		

		std::sort(population.begin(),population.end(),ordenation);
		
		
		//#if(saveFile>0)
		fprintf(logStatistic,"%.2f;",population[0]->fitness); /* Máximo */
		fprintf(logStatistic,"%.2f;",population[popsize-1]->fitness); /* Minimo*/
		//#endif
		
		for(i = 0; i < popsize; i++)
			sum += population[i]->fitness;
			
		mean = sum/popsize;
		
		//#if(saveFile>0)
		fprintf(logStatistic,"%f;",mean);
		//#endif
		
		sum = 0;
		
		for(i = 0; i < popsize; i++)
			sum += pow((population[i]->fitness - mean),2);
			
		stddev = sum/popsize;		
		
		//#if(saveFile>0)
		fprintf(logStatistic,"%f",stddev);
		fprintf(logStatistic,"\n");
		//#endif
			
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
		double mean = 0;								/* Mean of population's fitness.     			 */
		double stddev = 0;								/* Standart desviation of population's fitness.  */
		
		double time_in_seconds_evaluation = 0;
		double time_in_seconds_selection = 0;
		double time_in_seconds_reproduction = 0;
		double time_in_seconds_replacement =0;
		
		logStatistic = fopen("statistic.txt", "w");
		logIndividuals = fopen("individuals.txt", "w");
		logBests = fopen("bests.txt","w");
		
		//fprintf(logStatistic,"Min;Max;Mean;Stdv"); /* Header */
		
		SANITY_CHECK();

		//for(unsigned stat = 0; stat < 1;stat++)
		//{
			
			/* Build initial population. */
			for(unsigned i = 0; i < popsize;i++ )
				population.push_back(Individual_random(maxDepth,1));
						
			
			for(unsigned i = 0; i < popsize; i++)
			{

				fprintf(stderr, "generation: %d\n", i);

				#if(dbg>0)
				fprintf(stderr, "info: individual evaluate! Ok\n");
				#endif
				/**Evaluate the new population*/
				clock_t start_time_evaluation = clock();
				individual_evaluate(population);
				time_in_seconds_evaluation += (clock() - start_time_evaluation) / (double)CLOCKS_PER_SEC;
				
				
			
				/* Print population statistics.*/
				//statistics(population, logStatistic);
					
				#if(dbg>0)
				fprintf(stderr, "info: selection! Ok\n");
				#endif
				/* Select individuals for reproduction.*/
				clock_t start_time_selection = clock();
				selection(parents, population, 2);					
				time_in_seconds_selection += (clock() - start_time_selection) / (double)CLOCKS_PER_SEC;
				
				#if(dbg>0)
				fprintf(stderr, "info: reproduction! Ok\n");
				#endif
				/* Crossover and mutation.*/
				clock_t start_time_reproduction = clock();
				reproduction(g, newpopulation, parents);
				time_in_seconds_reproduction += (clock() - start_time_reproduction) / (double)CLOCKS_PER_SEC;
				
				#if(dbg>0)
				fprintf(stderr, "info: replacement! Ok\n");
				#endif
				/* Replaces the old population by the new.*/
				clock_t start_time_replacement = clock();
				replacement(g, population, newpopulation);
				time_in_seconds_replacement += (clock() - start_time_replacement) / (double)CLOCKS_PER_SEC;
			}	
			
			/* House Kepping.*/
			for(unsigned w = 0; w < population.size(); w++)
				individual_destroy(population[w]);
			population.resize(0);
			
			//for(unsigned w = 0; w < parents.size(); w++)
				//individual_destroy(parents[w]);
			//parents.resize(0);
			
			//for(unsigned w = 0; w < newpopulation.size(); w++)
				//individual_destroy(newpopulation[w]);
			//newpopulation.resize(0);	
			
			//for(unsigned i = 0; i < popsize;i++ )
				//{
					//fprintf(stderr,"%.2f\n", population[i]->fitness);
					//fprintf(stderr,"\n\n");
				//}
			
		
		//}
		
		fclose(logStatistic);
		fclose(logIndividuals);
		fclose(logBests);
		
		#if(time_ga>0)
		fprintf(logBests, "Evaluation: %.2f\n",time_in_seconds_evaluation);
		fprintf(logBests, "Selection: %.2f\n",time_in_seconds_selection);
		fprintf(logBests, "Reproduction: %.2f\n",time_in_seconds_reproduction);
		fprintf(logBests, "Replacement: %.2f\n",time_in_seconds_replacement);
		#endif

	}
