	/**
	*	Larissa Fernandes Leijôto <larissa.leijoto@gmail.com>
	*	
	* 	This file is part of genetic programming developed in a first project of natural computing.
	*/
	
	#ifndef TREE_H_
	#define TREE_H_

	#include <vector>
	#include <string>
	#include <stdlib.h>  
	#include <time.h>      

	const char Operators[4] = {'+','-','*','/'};

	/* Values representing two kinds of nodes. */
	const int NUMBER = 0,    
              OPERATOR = 1,
              VAR = 2;
    
    /* Tree that belongs to an individual.*/
	struct Tree
	{
		struct ExpNode *root;
		unsigned countNodes;			    	  	   
	};
 
	/* A node in an expression tree.*/
	struct ExpNode 
	{                                      
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
		/*
		 * 
		 */
		void copy_node(struct ExpNode*);   
		/*
		 * 
		 */
		int assignId(unsigned);	 
		
    };    
    
    /**
     * 
     */    
    void tree_destroy(ExpNode *node);
      
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
	 ExpNode *add_child(unsigned maxDepth, ExpNode *node, int &countVariables);
	
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
	
	/**
	 * 
	 */	
	ExpNode *copy_tree(ExpNode *root);

	#endif /* TREE_H_ */
