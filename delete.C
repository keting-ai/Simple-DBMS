//////////////////////////////////////////////////////////////////////////////
// Project:          Project 6
// File name:        insert.c
// Description:      This file provides the method QU_Delete() to delete
// 					 attributes from the relation
// 
// Member Name:      Keting Chen
// Email address:    kchen346@wisc.edu
// Student ID:       9082227530
// 
// Member Name:      Chenhan Luo
// Email address:    cluo49@wisc.edu
// Student ID:       9082574345
///////////////////////////////////////////////////////////////////////////////

#include "catalog.h"
#include "query.h"

/**
 * This function will delete all tuples in relation satisfying the predicate specified by attrName,
 * op, and the constant attrValue. type denotes the type of the attribute.
 *
 * @param relation: delete attribute in this relation
 * @param attrName: the attribute name to be deleted
 * @param op: the operation type
 * @param type: the data's type
 * @param attrValue: the attribute's value/data
 * @return: OK on success
 * 			an error code otherwise
 */

const Status QU_Delete(const string & relation, 
		       const string & attrName, 
		       const Operator op,
		       const Datatype type, 
		       const char *attrValue)
{
	// part 6
	Status status;
	HeapFileScan scan(relation, status);
	if (status != OK) {
		return status;
	}
	AttrDesc attrDesc;
	attrCat->getInfo(relation, attrName, attrDesc);

	int int_val;
	float flt_val;
	switch (type)
	{
	case STRING:
		status = scan.startScan(attrDesc.attrOffset, attrDesc.attrLen, type, attrValue, op);
		break;
	
	case INTEGER:
		int_val = atoi(attrValue);
		status = scan.startScan(attrDesc.attrOffset, attrDesc.attrLen, type, (char*)&int_val, op);
		break;

	case FLOAT:
		flt_val = atof(attrValue);
		status = scan.startScan(attrDesc.attrOffset, attrDesc.attrLen, type, (char*)&flt_val, op);
		break;
	}
	if (status != OK) {
		return status;
	}

	RID nextRID;
	while (scan.scanNext(nextRID) == OK) {
		status = scan.deleteRecord();
		if (status != OK) {
			return status;
		}
	}

	return OK;
}


