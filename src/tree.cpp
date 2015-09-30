	/**
	*	Larissa Fernandes Leijôto <larissa.leijoto@gmail.com>
	*	
	* 	This file is part of genetic programming developed in a first project of natural computing.
	*/

	#include "tree.h"
	#include "util.h"
	
	
	using namespace std;

	#define TREE(x) ((struct Tree *)(x))
	#define NODE(x) ((struct ExpNode *)(x))
	
	/**
	 * @brief
	 */
	ExpNode *create_ExpNode(unsigned l)
	{
		ExpNode *node = (struct ExpNode *)smalloc(sizeof(ExpNode));
		NODE(node)->level = l;
		NODE(node)->op = ' ';
		NODE(node)->number = -1;
		NODE(node)->kind = -1;
		NODE(node)->id = -1;
		NODE(node)->left = NULL;
		NODE(node)->right = NULL;
		
		return node;
	}
	
	/**
	 * @brief
	 */
	Tree *create_tree()
	{
		Tree *tree = (Tree *)smalloc(sizeof(Tree));
		TREE(tree)->root = NULL;
		TREE(tree)->countNodes = 0;		
		
		return tree;
	}
	
	/**
	 * @brief
	 */
	void ExpNode::copy_node(ExpNode *node)
	{
		op = node->op;
		kind = node->kind;
		number = node->number;
							
	}
	
	/**
	 * @brief
	 */
	double getValue(ExpNode *node, double x)
	{
		double leftVal, rightVal;
                // Return the value of the expression represented by
                // the tree to which node refers.  Node must be non-NULL.
             if ( NODE(node)->kind == NUMBER or NODE(node)->kind == VAR) 
             {
                   // The value of a NUMBER node is the number it holds.
                   double ter;
                  if(NODE(node)->kind == NUMBER) 
                  {
					 ter = NODE(node)->number;
					 
				  }else
				  {
					 ter = x;
				  }
                   
                return ter;
             }
             else 
             {  // The kind must be OPERATOR.
                     // Get the values of the operands and combine them
                     //    using the operator.
                
                if(NODE(node)->left != NULL)
					leftVal = getValue(NODE(node)->left, x);
				if(NODE(node)->right != NULL)
					rightVal = getValue(NODE(node)->right, x);
                
                switch (NODE(node)->op) 
                {
                   case '+':  
					   return leftVal + rightVal;
					   break;
                   case '-': 
					   return leftVal - rightVal;
					   break;
                   case '*':  
					   return leftVal * rightVal;
					   break;
                   case '/': 
					   if(rightVal == 0)
							return 1;
						else					   
							return leftVal / rightVal;
					   break;
                }
             }
    } 
	
	/**
	 * @brief
	 */
	void Full(Tree *tree, unsigned maxDepth)
	{
		ExpNode *child; 
		
		tree->root = create_ExpNode(0);
		tree->root->kind = OPERATOR;
		tree->root->op = Operators[rand()% 4];
		tree->countNodes+=1;
		tree->root->id =tree->countNodes;
		
		vector<ExpNode*> available;

		available.push_back(tree->root);
		
		while (not available.empty()) 
		{
			unsigned pos = rand() % available.size();
			ExpNode *cur = available[pos];

			child = add_child(maxDepth, cur);
			
			if (child == NULL) 
			{
				available.erase(available.begin() + pos);
			}
			else 
			{
				TREE(tree)->countNodes+=1;
				NODE(child)->id = TREE(tree)->countNodes;
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

			child = add_child(maxDepth, cur);
			
			if (child == NULL) 
				available.erase(available.begin() + pos);
			else 
			{
				available.push_back(child);
				TREE(tree)->countNodes+1;
				
				if(depth < NODE(child)->level)
					depth = NODE(child)->level;
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
			if(rand()%2)
			{
				kind = NUMBER;
				number = rand()% 10;
			}
			else
			{
				kind = VAR;
				number = -1;
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
	 * @brief
	 */
	 ExpNode *add_child(unsigned maxDepth, ExpNode *root)
	{
		ExpNode *child = NULL;
		
		if (NODE(root)->level + 1 > maxDepth) 
		{
			return child;	
	
		}
		if(NODE(root)->left == NULL or NODE(root)->right == NULL)
		{
			child =  create_ExpNode(NODE(root)->level + 1);
			
			if (NODE(child)->level == maxDepth) 
			{
				// build leaf...
				if(rand()%2)
				{
					NODE(child)->kind = NUMBER;
					//NODE(child)->number = ((double)rand() / (double)(RAND_MAX));
					NODE(child)->number = rand()%10;
				}
				else
				{
					NODE(child)->kind = VAR;
					NODE(child)->number = -1;
				}
			}
			else 
			{
				// choose between op
				NODE(child)->kind = OPERATOR;
				NODE(child)->op = Operators[rand()%4];			
			}
			
			if (NODE(root)->left == NULL) 
				NODE(root)->left = child;
							 
			else if (NODE(root)->right == NULL) 
				NODE(root)->right = child;
			
		}
		return child;
	}
	
	 /**
	 * @brief To display the elements in a tree using inorder
	 */
	void display(ExpNode *list)
	{
		if (NODE(list) == NULL)
			return;

		display(NODE(list)->left);
		if(NODE(list)->kind == OPERATOR)
		{
			printf("%d\t%c\n",NODE(list)->id, NODE(list)->op);
		}
		else if(NODE(list)->kind == VAR)
		{
			printf("%d\tX\n",NODE(list)->id);
		}
		else
		{
			printf("%d\t%.2f\n",NODE(list)->id,NODE(list)->number);		
		} 		
		display(NODE(list)->right);
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
			nodeleft = copy_Individual(NODE(offspring)->left, parent->left, cutoff);		
	
		}
	
		if(parent->right != NULL and parent->right->id != cutoff)
		{
			offspring->right = create_ExpNode(offspring->level+1);
			offspring->right->copy_node(parent->right);
			noderight = copy_Individual(NODE(offspring)->right, parent->right, cutoff);	
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
	
