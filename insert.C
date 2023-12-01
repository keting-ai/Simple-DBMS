//////////////////////////////////////////////////////////////////////////////
// Project:          Project 6
// File name:        insert.c
// Description:      This file provides the method QU_Insert() to insert
// 					 attributes into the relation
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
 * Insert a tuple with the given attribute values (in attrList) in relation. 
 * The value of the attribute is supplied in the attrValue member of the attrInfo structure.
 * 
 * @param relation: The relation to be inserted
 * @param attrCnt: The attribute count
 * @param attrList: The list of attribute to be inserted
 * @return: OK on success
 * 			an error code otherwise
 */

const Status QU_Insert(const string & relation, 
	const int attrCnt, 
	const attrInfo attrList[])
{
	// part 6
	Status status;
	int real_attrCnt;
	AttrDesc *real_attrs;
	int reclen = 0;

	InsertFileScan res(relation, status);
	if (status != OK) {
		return status;
	}

	status = attrCat->getRelInfo(relation, real_attrCnt, real_attrs);
	if (status != OK) {
		return status;
	}

	for (int i = 0; i < real_attrCnt; i++) {
		reclen += real_attrs[i].attrLen;
	}
	char insert_data[reclen];

	for(int i = 0; i < real_attrCnt; i++) {
		bool reject = true;
		for (int j = 0; j < attrCnt; j ++) {
			if (strcmp(real_attrs[i].attrName, attrList[j].attrName) == 0) {
				int int_val = 0;
				float flt_val = 0;
				switch (attrList[j].attrType)
				{
				case STRING:
					memcpy(insert_data + real_attrs[i].attrOffset, attrList[j].attrValue, real_attrs[i].attrLen);
					break;
				
				case INTEGER:
					int_val = atoi((char*)attrList[j].attrValue);
					memcpy(insert_data + real_attrs[i].attrOffset, &int_val, real_attrs[i].attrLen);
					break;

				case FLOAT:
					flt_val = atof((char*)attrList[j].attrValue);
					memcpy(insert_data + real_attrs[i].attrOffset, &flt_val, real_attrs[i].attrLen);
					break;
				}
				reject = false;
			}
		}
		if (reject) {
			free(real_attrs);
			return UNIXERR;
		}
	}

	Record insert_rec;
	RID insert_rid;
	insert_rec.data = (void*) insert_data;
	insert_rec.length = reclen;
	status = res.insertRecord(insert_rec, insert_rid);
	if (status != OK) {
		return status;
	}
	free(real_attrs);

	return OK;
}

