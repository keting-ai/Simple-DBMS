//////////////////////////////////////////////////////////////////////////////
// Project:          Project 6
// File name:        insert.c
// Description:      This file provides the method QU_Insert() and ScanSelect() 
//                   to select attributes from the relation based on projections 
//                   and conditions in the where clause.
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

// forward declaration
const Status ScanSelect(const string & result,
			const int projCnt,
			const AttrDesc projNames[],
			const AttrDesc *attrDesc,
			const Operator op,
			const char *filter,
			const int reclen);

/**
 * A selection is implemented using a filtered HeapFileScan. The result of the selection is stored
 * in the result relation called result (a heapfile with this name will be created by the parser
 * before QU_Select() is called). The project list is defined by the parameters projCnt and
 * projNames. Projection should be done on the fly as each result tuple is being appended to the
 * result table.
 * 
 * @param result: the result
 * @param projCnt: the projection number
 * @param projNames: the projection names array
 * @param attr: the attribute to be matched
 * @param op: the operation type
 * @param attrValue: the attribute value/data to be matched
 * @return: OK on success
 * 	        an error code otherwise
 */

const Status QU_Select(const string & result,
        const int projCnt,
        const attrInfo projNames[],
        const attrInfo *attr,
        const Operator op,
        const char *attrValue)
{
    Status status;
    AttrDesc attr_desc;
    AttrDesc proj_attr_desc[projCnt];
    int reclen = 0;

    for (int i = 0; i < projCnt; i++) {
        status = attrCat->getInfo(projNames[i].relName, projNames[i].attrName, proj_attr_desc[i]);
        if (status != OK) { 
            return status; 
        }
        reclen += proj_attr_desc[i].attrLen;
    }

    char* filter_val;
    if (attr == NULL) {
        filter_val = NULL;
        attr_desc.attrLen = 0;
        strcpy(attr_desc.relName, projNames[0].relName);
        attr_desc.attrOffset = 0;
        attr_desc.attrType = 0;
        strcpy(attr_desc.attrName, projNames[0].attrName);
    } else {
        status = attrCat->getInfo(attr->relName, attr->attrName, attr_desc);
        if (status != OK) { 
            return status; 
        }
        int int_val;
        float flt_val;
        switch (attr->attrType) 
        {
        case INTEGER:
            int_val = atoi(attrValue);
            filter_val = (char*)&int_val;
            break;
        case FLOAT:
            flt_val = atof(attrValue);
            filter_val = (char*)&flt_val;
            break;
        case STRING:
            filter_val = (char*)attrValue;
            break;
        }
    }

    return ScanSelect(
        result, 
        projCnt, 
        proj_attr_desc, 
        &attr_desc, 
        op, 
        filter_val, 
        reclen
    );
}

/**
 * The helper mathod of QU_Select() method. This method really does the scanning
 * of all attributes in the database and match the result attributes. All resulting
 * attributes are inserted to a temporary table by calling InsertFileScan::insertRecord().
 * 
 * @param projCnt: the number of projections
 * @param projNames: the array of projection names
 * @param attrDesc: the attribute description of the attribute to be matched
 * @op: the operation type
 * @filter: the filter value used to match the attributes
 * @reclen: the max length of matched record's data
*/
const Status ScanSelect(const string & result,
#include "stdio.h"
#include "stdlib.h"
        const int projCnt,
        const AttrDesc projNames[],
        const AttrDesc *attrDesc,
        const Operator op,
        const char *filter,
        const int reclen)
{
    Status status;
    HeapFileScan hfs(attrDesc->relName, status);
    if (status != OK) { 
        return status; 
    }
    status = hfs.startScan(
        attrDesc->attrOffset, 
        attrDesc->attrLen, 
        (Datatype)attrDesc->attrType, 
        filter, 
        op
    );
    if (status != OK) { 
        return status; 
    }

    RID rid;
    Record rec;
    InsertFileScan ifs(result, status);
    if (status != OK) { 
        return status; 
    }

    Record res_rec;
    char res_data[reclen];
    res_rec.data = (void*)res_data;
    res_rec.length = reclen;
    while (hfs.scanNext(rid) == OK) {
        status = hfs.getRecord(rec);
        if (status != OK) { 
            return status; 
        }
        int off = 0;
        for (int i = 0; i < projCnt; ++i) {
            memcpy(
                res_data + off, 
                (char*)rec.data + projNames[i].attrOffset, 
                projNames[i].attrLen
            );
            off += projNames[i].attrLen;
        }
        RID _tempRID;
        status = ifs.insertRecord(res_rec, _tempRID);
        if (status != OK) { 
            return status; 
        }
    }

    return OK;
}