// BTree.cpp : Defines the entry point for the application.

#include "Main.h"
#include "BTree.h"

using namespace std;

// https://cis.stvincent.edu/html/tutorials/swd/btree/btree.html
// https://courses.grainger.illinois.edu/cs225/sp2025/labs/btree/
// https://courses.grainger.illinois.edu/cs225/sp2025/resources/btrees/
// https://planetscale.com/blog/btrees-and-database-indexes
// https://github.com/Zhenye-Na/data-structures-uiuc/tree/master/lab_btree
// video: https://www.youtube.com/watch?v=K1a2Bk8NrYQ

int main()
{
	BTree btree(3);
	btree.insert(1, 1);
	btree.insert(2, 1);
	btree.insert(3, 1);
	btree.insert(4, 1);
	//btree.insert(5, 1);
	//btree.insert(10, 1);
	//btree.insert(8, 1);
	//btree.insert(6, 1);
	//btree.insert(7, 1);
	//btree.insert(20, 1);
	//btree.insert(21, 1);
	//btree.insert(23, 1);

	//btree.insert(50, 1);
	//btree.insert(100, 1);
	//btree.insert(34, 1);
	//btree.insert(25, 1);
	//btree.insert(9, 1);
	btree.print();

	return 0;
}
