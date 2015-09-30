	/**
	*	Larissa Fernandes Leijôto <larissa.leijoto@gmail.com>
	*	
	* 	This file is part of genetic programming developed in a first project of natural computing.
	*/
	#include <string.h>
	#include <string>


	#ifndef DATABASE_H_
	#define DATABASE_H_


		struct database
		{
			double **points;   /**< points to peforme the regression.   */
			unsigned npoints;  /**<  number of points.  			  	*/
		};
		
		
		/* Forward definitions. */
		extern void database_parse(const char* filename);
		extern void database_read(const char* filename);
		extern void database_destroy(void);
		extern void print_base(void);
		
		/* Forward definitions. */
		extern unsigned npoints;
		extern struct database database;
		

		
	#endif /* DATABASE_H_ */
