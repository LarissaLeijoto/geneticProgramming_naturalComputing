	/**
	*	Larissa Fernandes Leijôto <larissa.leijoto@gmail.com>
	*	
	* 	This file is part of genetic programming developed in a first project of natural computing.
	*/

	#include "tree.h"
	#include "util.h"
	#include "database.h"

	
	using namespace std;
	
	/**
	 * @brief Creates an initialized node.
	 * @param level that the node belongs.
	 * 
	 * @return A new node initialized.
	 */
	ExpNode *create_ExpNode(unsigned l)
	{
		ExpNode *node = (struct ExpNode *)smalloc(sizeof(ExpNode));
		node->level = l;
		node->op = ' ';
		node->number = rand()/ (double)RAND_MAX;
		node->kind = -1;
		node->id = -1;
		node->left = NULL;
		node->right = NULL;
		
		return node;
	}
	
	/**
	 * @brief Creates an initialized tree.
	 * 
	 * @return A new tree initialized.
	 */
	Tree *create_tree()
	{
		Tree *tree = (Tree *)smalloc(sizeof(Tree));
		tree->root = NULL;
		tree->countNodes = 0;		
		
		return tree;
	}
	
	/**
	 * @brief Copys a node to another.
	 * 
	 * @param Node that will be copied.
	 */
	void ExpNode::copy_node(ExpNode *node)
	{
		op = node->op;
		id = node->id;
		kind = node->kind;
		number = node->number;							
	}
	
	/**
	 * @brief Frees memory allocated to a nod.
	 * 
	 * @param Node that will be deallocated.
	 */
	void tree_destroy(ExpNode *node)
	{
		if(node!=NULL)
		{
			tree_destroy(node->left);
			tree_destroy(node->right);
			free(node);
			node = NULL;
		}

	}
	/**
	 * @brief
	 */
	 ExpNode *add_child(unsigned maxDepth,ExpNode *root, int &countVariables)
	{
		ExpNode *child = NULL;
		
		if (root->level + 1 > maxDepth) 
		{
			return child;	
	
		}
		if(root->left == NULL or root->right == NULL)
		{
			child =  create_ExpNode(root->level + 1);
			
			if (child->level == maxDepth) 
			{
				/* Choose at least one variable*/
				if(countVariables > 0)
				{
					child->kind = VAR;
					child->number = rand()/ (double)RAND_MAX;
					countVariables--;
				}
				else if(rand()%2 == 0 and countVariables == 0)
				{
					child->kind = VAR;
					child->number = rand()/ (double)RAND_MAX;
				}else
				{			
					child->kind = NUMBER;
				}
			}
			else 
			{
				// choose between op
				child->kind = OPERATOR;
				child->op = Operators[rand()%4];			
			}
			
			if (root->left == NULL) 
				root->left = child;
							 
			else if (root->right == NULL) 
				root->right = child;
			
		}
		return child;
	}
	
	
	/**
	 * @brief  Return the value of the expression represented by the tree to which node refers.  Node must be non-NULL.
	 */
	double getValue(ExpNode *node, double x)
	{		
             if (node->left == NULL and node->right == NULL)
             {
				 double terminal = node->number;
				/* The value of a NUMBER node is the number it holds.*/
				            
				if(node->kind == VAR)
					terminal = x;
		
                return terminal;
             }
             else 
             { /*The kind must be OPERATOR, to get the values of the operands and combine them using the operator. */ 
				double result = 0.0;               
                double leftVal, rightVal;
                
                if(node->left != NULL)
					leftVal = getValue(node->left, x);
								
				if(node->right != NULL)
					rightVal = getValue(node->right, x);
                
                switch (node->op) 
                {
					case '+':  
						result = leftVal + rightVal;
						break;
					case '-': 
						result = leftVal - rightVal;
						break;
					case '*':  
						result = leftVal * rightVal;
						break;
					case '/': 
						if(rightVal == 0.0)
							result = leftVal;
						else
							result = leftVal/rightVal;
						break;
					default:
						result = leftVal + rightVal; 
						break;
                }
                return result;
             }
    } 
	
	/**
	 * @brief
	 */
	void Full(Tree *tree, unsigned maxDepth)
	{
		ExpNode *child; 
		int countVariables = database.ncolunms -1;
		
		tree->root = create_ExpNode(0);
		tree->root->kind = OPERATOR;
		tree->root->op = Operators[rand()%4];
		tree->countNodes+=1;
		tree->root->id =tree->countNodes;
		
		vector<ExpNode*> available;

		available.push_back(tree->root);
		
		while (not available.empty()) 
		{
			unsigned pos = rand() % available.size();
			ExpNode *cur = available[pos];

			child = add_child(maxDepth, cur, countVariables);
			
			if (child == NULL) 
			{
				available.erase(available.begin() + pos);
			}
			else 
			{
				tree->countNodes+=1;
				child->id = tree->countNodes;
				available.push_back(child);
				
			}
		}
	}
	
	/**
	 * @brief
	 */
	void Grow( Tree *tree, unsigned maxDepth)
	{
		unsigned depth = 0;
		int countVariables = database.ncolunms -1;
		ExpNode *child; 


		tree->root = create_ExpNode(0);
		tree->root->kind = OPERATOR;
		tree->root->op = Operators[rand()% 4];
		
		vector< ExpNode*> available;

		available.push_back(tree->root);
		
		while (depth < maxDepth) 
		{
			unsigned pos = rand() % available.size();
			ExpNode *cur = available[pos];

			child = add_child(maxDepth, cur, countVariables);
			
			if (child == NULL) 
				available.erase(available.begin() + pos);
			else 
			{
				available.push_back(child);
				tree->countNodes+1;
				
				if(depth < child->level)
					depth = child->level;
			}
		}
		
		for (ExpNode *node : available) 
			node->var_leaf();
	}
	
	/**
	 * @brief To count the number of leaf nodes
	 */ 
	void ExpNode::var_leaf()
	{
		ExpNode *node;
		
		if (left == NULL && right == NULL)
		{
			if(rand()%2 == 0)
			{
				kind = NUMBER;
			}
			else
			{
				kind = VAR;
				number = rand()/(double)RAND_MAX ;
			}
			
		}
		else if(left == NULL )
		{
			left = create_ExpNode(level+1);
			
			if(op == '+' or op == '-')
			{
				left->kind = NUMBER;
				left->number = 0;
			}
			else if(op == '*' or op == '/')
			{
				left->kind = NUMBER;
				left->number = 1;
			}
				
		}
		else if(right == NULL)
		{
			right = create_ExpNode(level+1);
			
			if(op == '+' or op == '-')
			{
				right->kind = NUMBER;
				right->number = 0;
			}
			else if(op == '*' or op == '/')
			{
				right->kind = NUMBER;
				right->number = 1;
			}
		}
			
	}
	
	
	 /**
	 * @brief To display the elements in a tree using inorder
	 */
	void display(ExpNode *list)
	{
		if (list == NULL)
			return;

		if(list->kind == OPERATOR)
		{
			fprintf(stderr,"%c ", list->op);
		}
		else if(list->kind == VAR)
		{
			fprintf(stderr," X");

			//fprintf(stderr,"%d  %.2f\n ",list->id,list->number);
		}
		else
		{
			fprintf(stderr,"%.2f ",list->number);		
		} 	
		display(list->left);	
		display(list->right);
	}
	
	

	 
	/**
	 * @brief
	 */
	ExpNode *copy_Individual(ExpNode *offspring, ExpNode *parent, int cutoff)
	{
		ExpNode *nodeleft = NULL;
		ExpNode *noderight = NULL;
			
		if(parent->left != NULL and parent->left->id != cutoff)
		{
			offspring->left = create_ExpNode(offspring->level+1);
			offspring->left->copy_node(parent->left);
			nodeleft = copy_Individual(offspring->left, parent->left, cutoff);		
	
		}
	
		if(parent->right != NULL and parent->right->id != cutoff)
		{
			offspring->right = create_ExpNode(offspring->level+1);
			offspring->right->copy_node(parent->right);
			noderight = copy_Individual(offspring->right, parent->right, cutoff);	
		}
		
		if(parent->left != NULL and parent->left->id == cutoff)
			return offspring->left = create_ExpNode(offspring->level+1);

		if(parent->right != NULL and parent->right->id == cutoff)
			return offspring->right = create_ExpNode(offspring->level+1);
			
		if(nodeleft != NULL)
			return nodeleft;
		
		return noderight;		
		
	}

	/**
	 * @brief
	 */
	ExpNode *Find_cutoff(ExpNode *parent, int cutoff)
	{
		if(parent == NULL)
			return NULL;
				
		if(parent->left != NULL and parent->left->id == cutoff)
			return parent->left;

		if(parent->right != NULL and parent->right->id == cutoff)
			return parent->right;
		
		ExpNode * child ;
		child = Find_cutoff(parent->left, cutoff);
		
		if(child != NULL)
			return child;
		
		child = Find_cutoff(parent->right, cutoff);
		
		
		if(child != NULL)
			return child;
			
		return NULL;
	}
	
	/**
	 * @brief
	 */
	ExpNode *copy_tree(ExpNode *root) 
	{
     ExpNode *new_root;
     
		 if(root!=NULL)
		 {
			 new_root = create_ExpNode(root->level);
			 new_root->copy_node(root);
			 new_root->left = copy_tree(root->left);
			 new_root->right = copy_tree(root->right);
		 }
		 else
			return NULL;
			
		 return new_root;
	}
	
	/**
	 * @brief 
	 */
	int ExpNode::assignId(unsigned i) 
	{
		 id = i;
		
		if( left != NULL)
			i = left->assignId(i+1);
					
		if(right != NULL)
			i = right->assignId(i+1);

		return i;
        
     } 
