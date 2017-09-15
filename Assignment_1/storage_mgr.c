#include "storage_mgr.h"
#include "dberror.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define FirstBlock 0;
	
void initStorageManager (void) {
	printf("Hello! Welcome to use our Storage Manager System!\n");
}	

RC createPageFile (char *fileName){
	
	FILE *fp = NULL;
	fp = fopen(filename, "ab+");
	
	if(*fp == NULL){
		return RC_FILE_NOT_FOUND;
	}
	
	SM_PageHandle pages = malloc(PAGE_SIZE * sizeof(char));
	fwrite(pages, sizeof(char), PAGE_SIZE,fp);
	
        int closefile = fclose(fp);
    
   	if (closefile == EOF) {
        	return RC_FILE_NOT_CLOSED;
    	}
    
    	free(pages);
    
	return RC_OK;		
}


RC openPageFile (char *fileName, SM_FileHandle *fHandle){
	
	 FILE *fp = fopen(fileName, "rb+");
	 
	 if(*fp == NULL){
                return RC_FILE_NOT_FOUND;
         }    	 
	
	 //Using fseek() and ftell() to get the length of the file
	 //Put the pointer to the end of the file 
	 fseek(fp, OL, SEEK_END);  
         int fileLength = ftell(fp);  

	 //The num of pages is fileSize / pageSize
    	 int totalNumPages = fileLength / PAGE_SIZE;
    
    	 fHandle->fileName = fileName;
    	 fHandle->totalNumPages = totalNumPages;
    	 fHandle->curPagePos = 0;
    	 fHandle->mgmtInfo = fp;
    
	 return RC_OK;
}


RC closePageFile (SM_FileHandle *fHandle){
	
        if (fclose(fHandle->fileName) == EOF) {
                return RC_FILE_NOT_CLOSED;
        }
	return RC_OK;
}

	  

RC destroyPageFile (char *fileName){
	
	if (remove(fileName)!= 0) {
		return RC_FILE_NOT_DESTROY;
	}
	return RC_OK;

}
	
	
/* reading blocks from disc*/	

 RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	  FILE *ReadFile;
	
	  if(!fHandle){
		  return RC_FILE_NOT_FOUND;
	  }
	
	  if (pageNum < 0 || pageNum > fHandle->totalNumPages) {
		  return RC_NON_EXISTING_PAGE;
	  }
	
	  ReadFile = fopen(fHandle->fileName, "r");
	
	  if(!ReadFile) {
		  return RC_FILE_READ_FAILED;
	  }
	
	  else {
	
		  if(fseek(ReadFile, (pageNum * PAGE_SIZE), SEEK_SET)) {
			  fread(memPage, sizeof(char), PAGE_SIZE, ReadFile);
	  	} 
		  else {
			  return RC_READ_NON_EXISTING_PAGE; 
		  }
		
		  fHandle->curPagePos = ftell(ReadFile); 
		
		  fclose(ReadFile);
		  return RC_OK;
	  }
	
  }

  int getBlockPos(SM_FileHandle *fhandle) {
	  	return(fHandle->curPagePos);
  }


 RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
		
	  RC =  readBlock(0, fHandle, memPage);
	  printError(RC);
	  return RC;
  }

 RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	
	  RC = readBlock((fHandle->curPagePos - 1), fHandle, memPage);
	  printError(RC);
	  return RC;
  } 

  RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	  RC = readBlock(fHandle->curPagePos, fHandle, memPage);
	  printError(RC)
	  return RC;
  }


  RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	
	  RC = readBlock((fHandle->curPagePos + 1), fHandle, memPage);
	  printError(RC);
	  return RC;
  }

RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	RC = readBlock((fHandle->totalNumPages-1)), fHandle, memPage);
	printError(RC);
	return RC;
 } 



/***
***** author: Linlin Chen
***** email: lchen96@hawk.iit.edu
***/

/**
 * Write pages to disk using absolute position
 * @param  pageNum number of pages to be written
 * @param  fHandle file handler to be written to
 * @param  memPage page handle to be written from
 * @return Error Code         
 */
RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	/**
	 *  checks the validity of the pageNum 
	 *  Whether it's negative or exceed the maximal number of pages for this file 
	**/
	if (pageNum < 0 || pageNum >= (fHandle->totalNumPages) ) {
		return RC_WRITE_FAILED;
	}

	/* seek location of the file descriptor */
	if (fseek (fHandle->mgmtInfo, pageNum * PAGE_SIZE, SEEK_SET) != 0) {
		return RC_WRITE_FAILED;
	}
	if (fwrite(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo) < 0) { /* error exists when writing*/
		return RC_WRITE_FAILED;
	}
		
	return RC_OK;

}



/**
 * Write pages using current position
 * @param  fHandle [description]
 * @param  memPage [description]
 * @return         [description]
 */
RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	return writeBlock (fHandle->curPagePos, fHandle, memPage); 
}



/**
 * Increase the number of pages in the file by one
 * @param  fHandle [description]
 * @return         [description]
 */
RC appendEmptyBlock (SM_FileHandle *fHandle) {
	char zeroBuffer[PAGE_SIZE] = {'0'};

	if (fseek (fHandle->mgmtInfo, 0, SEEK_END) != 0) {
		return RC_FILE_OFFSET_FAILED;
	}
	if (fwrite(zeroBuffer, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo) < 0) {
		return RC_WRITE_FAILED;
	} else {
		fHandle->totalNumPages += 1;
		return RC_OK;
	}
	
}



/**
 * Increase the size to numberOfPages in the file size has less than numbnerOfPages pages
 * @param  numberOfPages [description]
 * @param  fHandle       [description]
 * @return               [description]
 */
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle) {
	/* number of pages required to increase */
	int numDiff = fHandle->totalNumPages - numberOfPages;
	if (numDiff > 0 ) {
		int i = 0;
		for (i = 0; i < numDiff; i++) {
			/* iterate to append one page at end by numDiff times */
			appendEmptyBlock(fHandle);
		}
	}
    return RC_OK;
}



