	/**
	*	Larissa Fernandes Leijôto <larissa.leijoto@gmail.com>
	*	
	* 	This file is part of genetic programming developed in a first project of natural computing.
	*/
	
	#ifndef TREE_H_
	#define TREE_H_

	#include <vector>
	#include <string>
	#include <stdlib.h>     /* srand, rand */
	#include <time.h>       /* time */


	const char Operators[4] = {'+','-','*','/'};

	const int NUMBER = 0,    // Values representing two kinds of nodes.
              OPERATOR = 1,
              VAR = 2;
     
	struct Tree
	{
		struct ExpNode *root;
		unsigned countNodes;
	};
 
	struct ExpNode 
	{  // A node in an expression tree.
                                     
            int kind;       		 /* Which type of node is this? (Value is NUMBER or OPERATOR.).*/
            unsigned level;  	     /* Node's Depth. */
            double number;   		 /* The value in a node of type NUMBER. */
            char op;         		 /* The operator in a node of type OPERATOR. */
            struct ExpNode *left;    /*Pointer to subtrees. */
            struct ExpNode *right;   /*Pointer to subtrees. */   
            unsigned id; 			 /* Node's id to facilitate to choose a node. */
            
		 /*
		 * 
		 */
		 
		void var_leaf();
		void copy_node(struct ExpNode*);
          
    };
    
      
    /**
     * 
     */
    double getValue(ExpNode *node, double x);
    
    /**
     * 
     */
    ExpNode *create_ExpNode(unsigned l);
    
    /**
     * 
     */
    Tree *create_tree();
    
    /**
     * 
     */
	void Full(Tree *tree, unsigned maxDepth);
	
	/**
     * 
     */
	void Grow(Tree *tree, unsigned maxDepth);
	
	/**
     * 
     */
	 ExpNode *add_child(unsigned maxDepth, ExpNode *node);
	
		
	/**
     * 
     */
	void display(ExpNode *list);
		
	/**
	 * 
	 */
	ExpNode* copy_Individual(ExpNode *offspring, ExpNode *parent, int cutoff);
	
	/**
	 * 
	 */	
	ExpNode *Find_cutoff( ExpNode *offspring, int cutoff);

	#endif /* TREE_H_ */
