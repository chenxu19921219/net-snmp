#include <config.h>

#include <sys/types.h>
#include <stdio.h>
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#include "asn1.h"
#include "acl.h"



static struct aclEntry *List = NULL, *ScanPtr = NULL;


struct aclEntry *
acl_getEntry(target, subject, resources)
    int target, subject, resources;
{
    struct aclEntry *ap;

/* cache here */
    for(ap = List; ap; ap = ap->next){
        if (target == ap->aclTarget && subject == ap->aclSubject
	    && resources == ap->aclResources)
	    return ap;
    }
    return NULL;
}

int
acl_scanInit()
{
  ScanPtr = List;
}

struct aclEntry *
acl_scanNext()
{
    struct aclEntry *returnval;

    returnval = ScanPtr;
    if (ScanPtr != NULL)
        ScanPtr = ScanPtr->next;
    return returnval;
}

struct aclEntry *
acl_createEntry(target, subject, resources)
    int target, subject, resources;
{
    struct aclEntry *ap;

    ap = (struct aclEntry *)malloc(sizeof(struct aclEntry));
#ifdef SVR4
    memset((char *)ap, NULL, sizeof(struct aclEntry));
#else
    bzero((char *)ap, sizeof(struct aclEntry));
#endif

    ap->aclTarget = target;
    ap->aclSubject = subject;
    ap->aclResources = resources;
    ap->reserved = (struct aclEntry *)malloc(sizeof(struct aclEntry));
#ifdef SVR4
    memset((char *)ap->reserved, NULL, sizeof(struct aclEntry));
#else
    bzero((char *)ap->reserved, sizeof(struct aclEntry));
#endif

    ap->next = List;
    List = ap;
    return ap;
}

acl_destroyEntry(target, subject, resources)
    int target, subject, resources;
{
    struct aclEntry *ap, *lastap;

    if (List->aclTarget == target && List->aclSubject == subject
	&& List->aclResources == resources){
	ap = List;
	List = List->next;
    } else {
	for(ap = List; ap; ap = ap->next){
	    if (ap->aclTarget == target
		&& ap->aclSubject == subject
		&& ap->aclResources == resources)
		break;
	    lastap = ap;
	}
	if (!ap)
	    return;
	lastap->next = ap->next;
    }
    if (ap->reserved)
	free((char *)ap->reserved);
    free(ap);
    return;
}
