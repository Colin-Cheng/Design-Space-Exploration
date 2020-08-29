#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <math.h>
#include <fcntl.h>
#include <vector>
#include <iterator>

#include "431project.h"

using namespace std;

/*
 * Enter your PSU ID here to select the appropriate dimension scanning order.
 */
#define MY_PSU_ID 901439637
#define ID_FLAG (MY_PSU_ID % 2)

/*
 * Some global variables to track heuristic progress.
 * 
 * Feel free to create more global variables to track progress of your
 * heuristic.
 */
unsigned int currentlyExploringDim = ID_FLAG ? 0 : (NUM_DIMS-NUM_DIMS_DEPENDENT-1);
bool currentDimDone = false;

/*
 * Given a half-baked configuration containing cache properties, generate
 * latency parameters in configuration string. You will need information about
 * how different cache paramters affect access latency.
 * 
 * Returns a string similar to "1 1 1"
 */
std::string generateCacheLatencyParams(string halfBackedConfig){

  string latencySettings;

  //
  //YOUR CODE BEGINS HERE
  //
  unsigned int l1block_index = extractConfigPararm(halfBackedConfig, 2);
  unsigned int dl1sets_index = extractConfigPararm(halfBackedConfig, 3);
  unsigned int dl1assoc_index = extractConfigPararm(halfBackedConfig, 4);
  unsigned int il1sets_index = extractConfigPararm(halfBackedConfig, 5); 
  unsigned int il1assoc_index = extractConfigPararm(halfBackedConfig, 6);
  unsigned int ul2sets_index = extractConfigPararm(halfBackedConfig, 7);
  unsigned int ul2block_index = extractConfigPararm(halfBackedConfig, 8);
  unsigned int ul2assoc_index = extractConfigPararm(halfBackedConfig, 9);
  
  unsigned int dl1lat_index;
  unsigned int il1lat_index;
  unsigned int ul2lat_index;

  unsigned int l1block[4] = {8,16,32,64};
  unsigned int dl1sets[9] = {32,64,128,256,512,1024,2048,4096,8192};
  unsigned int dl1assoc[3] = {1,2,4};
  unsigned int il1sets[9] = {32,64,128,256,512,1024,2048,4096,8192}; 
  unsigned int il1assoc[3] = {1,2,4};
  unsigned int ul2sets[10] = {256,512,1024,2048,4096,8192,16384,32768,65536,131072};
  unsigned int ul2block[4] = {16,32,64,128}; 
  unsigned int ul2assoc[5] = {1,2,4,8,16};

  /* L1 data cache size in KB */
  unsigned int dl1size = l1block[l1block_index] * dl1sets[dl1sets_index] 
                   * dl1assoc[dl1assoc_index] / 1024; 

  /* L1 instruction cache size in KB */
  unsigned int il1size = l1block[l1block_index] * il1sets[il1sets_index] 
                   * il1assoc[il1assoc_index] / 1024; 

  /* L2 unified cache size in KB */
  unsigned int ul2size = ul2block[ul2block_index] * ul2sets[ul2sets_index] 
                   * ul2assoc[ul2assoc_index] / 1024; 
   
  /* set value for l1 data cache latency index */
  if (dl1size == 2) {
    dl1lat_index = 0;
  }  
  else if (dl1size == 4) {
    dl1lat_index = 1;
  }
  else if (dl1size == 8) {
    dl1lat_index = 2;
  }
  else if (dl1size == 16) {
    dl1lat_index = 3;
  }
  else if (dl1size == 32) {
    dl1lat_index = 4;
  }
  else if (dl1size == 64) {
    dl1lat_index = 5;
  }
  
  if (dl1assoc[dl1assoc_index] == 2) {
    dl1lat_index += 1;
  } 
  else if (dl1assoc[dl1assoc_index] == 4) {
    dl1lat_index += 2;
  }
  
  /* set value for l1 instruction cache latency index */
  if (il1size == 2) {
    il1lat_index = 0;
  }
  else if (il1size == 4) {
    il1lat_index = 1;
  }
  else if (il1size == 8) {
    il1lat_index = 2;
  }
  else if (il1size == 16) {
    il1lat_index = 3;
  }
  else if (il1size == 32) {
    il1lat_index = 4;
  }
  else if (il1size == 64) {
    il1lat_index = 5;
  }

  if (il1assoc[il1assoc_index] == 2) {
    il1lat_index += 2;
  } 
  else if (il1assoc[il1assoc_index] == 4) {
    il1lat_index += 4;
  }
  
  /* set value for l2 unified cache latency index */
  if (ul2size == 32) {
    ul2lat_index = 0;
  }
  else if(ul2size == 64) {
    ul2lat_index = 1;
  }
  else if (ul2size == 128) {
    ul2lat_index = 2;
  }
  else if (ul2size == 256) {
    ul2lat_index = 3;
  }
  else if (ul2size == 512) {
    ul2lat_index = 4;
  }
  else if (ul2size == 1024) {
    ul2lat_index = 5;
  }


  if (ul2assoc[ul2assoc_index] == 2) {
    ul2lat_index += 1;
  } 
  else if (ul2assoc[ul2assoc_index] == 4) {
    ul2lat_index += 2;
  } 
  else if (ul2assoc[ul2assoc_index] == 8) {
    ul2lat_index += 3;
  }
  else if (ul2assoc[ul2assoc_index] == 16) {
    ul2lat_index += 4;
  }


  // convert from int to str
  std::ostringstream dl1lat;
  dl1lat << dl1lat_index;
  std::ostringstream il1lat;
  il1lat << il1lat_index;
  std::ostringstream ul2lat;
  ul2lat << ul2lat_index;

  // concact str together
  latencySettings = (dl1lat.str()) + " "
                      + (il1lat.str()) + " " 
                      + (ul2lat.str());
  //
  //YOUR CODE ENDS HERE
  //

  return latencySettings;
}



/*
 * Returns 1 if configuration is valid, else 0
 */
int validateConfiguration(std::string configuration){

  // The below is a necessary, but insufficient condition for validating a
  // configuration.
  unsigned int c1;
  unsigned int c2;
  unsigned int c3;
  unsigned int c4;  
  unsigned int c5;
  unsigned int c6;
  unsigned int c7;

  if (isNumDimConfiguration(configuration)){
    c1 = 1;
  } else {
    c1 = 0;
  }
  
  unsigned int width[4] = {1,2,4,8};
  unsigned int l1block[4] = {8,16,32,64};
  unsigned int dl1sets[9] = {32,64,128,256,512,1024,2048,4096,8192};
  unsigned int dl1assoc[3] = {1,2,4};
  unsigned int il1sets[9] = {32,64,128,256,512,1024,2048,4096,8192};
  unsigned int il1assoc[3] = {1,2,4};
  unsigned int ul2sets[10] = {256,512,1024,2048,4096,8192,16384,32768,65536,131072};
  unsigned int ul2block[4] = {16,32,64,128};
  unsigned int ul2assoc[5] = {1,2,4,8,16};

  unsigned int width_index = extractConfigPararm(configuration, 0);
  unsigned int l1block_index = extractConfigPararm(configuration, 2);
  unsigned int dl1sets_index = extractConfigPararm(configuration, 3);
  unsigned int dl1assoc_index = extractConfigPararm(configuration, 4);
  unsigned int il1sets_index = extractConfigPararm(configuration, 5);
  unsigned int il1assoc_index = extractConfigPararm(configuration, 6);
  unsigned int ul2sets_index = extractConfigPararm(configuration, 7);
  unsigned int ul2block_index = extractConfigPararm(configuration, 8);
  unsigned int ul2assoc_index = extractConfigPararm(configuration, 9);
  
  /* check if L1 block size match the instruction fetch queue size */
  int ifqsize = 8 * width[width_index];
  int l1blocksize = l1block[l1block_index];
  if (ifqsize == l1blocksize) {
    c2 = 1;
  } else{
    c2 = 0;
  }

  /* check if L2 block size is at least twice as large as il1 block size */ 
  int ul2blocksize = ul2block[ul2block_index];
  if (ul2blocksize >= 2 * l1blocksize) {
    c3 = 1;
  } else {
    c3 = 0;
  }
   
  /* check if L2 is at least twice as large as il1 + dl1 */ 
  /* L1 data cache size in KB */
  int dl1size = l1block[l1block_index] * dl1sets[dl1sets_index]
                  * dl1assoc[dl1assoc_index] / 1024;
                   
  /* L1 instruction cache size in KB */
  int il1size = l1block[l1block_index] * il1sets[il1sets_index]
                  * il1assoc[il1assoc_index] / 1024;
                   
  /* L2 unified cache size in KB */
  int ul2size = ul2block[ul2block_index] * ul2sets[ul2sets_index]
                  * ul2assoc[ul2assoc_index] / 1024;  
 
 if (ul2size >= 2*(dl1size + il1size)) {
     c4 = 1;
   } else {
    c4 = 0;
   }

  /* L1 min and max size */
  if (il1size < 2 || il1size > 64){
    c5 = 0;
  } else{
    c5 = 1;
  }

  /* D1 min and max size */
  if (dl1size < 2 || dl1size > 64){
    c6 = 0;
  } else {
    c6 = 1;
  }

  /* UL2 min and max size */
  if (ul2size < 32 || ul2size > 1024){
    c7 = 0;
  } else {
    c7 = 1;
  }
 
  return (c1*c2*c3*c4*c5*c6*c7);
}


/*
 * Given the current best known configuration, the current configuration,
 * and the globally visible map of all previously investigated configurations,
 * suggest a previously unexplored design point. You will only be allowed to
 * investigate 1000 design points in a particular run, so choose wisely.
 */
std::string generateNextConfigurationProposal(
    std::string currentconfiguration,
    std::string bestEXECconfiguration,
    std::string bestEDPconfiguration,
    int optimizeforEXEC,
    int optimizeforEDP,
    bool & isDSEComplete) {

  //
  // Note:
  // isDSEComplete is passed by reference. This means any changes
  // to this variable here will affect its value in calling main function
  // as well. Any implementation can set isDSEComplete=true to signal that
  // DSE is complete.

  //
  // Some interesting variables in 431project.h include:
  //
  // 1. GLOB_dimensioncardinality
  // 2. GLOB_baseline
  // 3. NUM_DIMS
  // 4. NUM_DIMS_DEPENDENT
  // 5. GLOB_seen_configurations


  // Check if proposed configuration has been seen before.
  std::string nextconfiguration = currentconfiguration;
  while (GLOB_seen_configurations[nextconfiguration]) {

    std::stringstream ss;
    int all_invaild = 0;

    string bestConfig;
    if (optimizeforEXEC == 1)
      bestConfig = bestEXECconfiguration;

    if (optimizeforEDP == 1)
      bestConfig = bestEDPconfiguration;

    // Fill in the dimensions already-scanned with the already-selected best
    // value.
    if (ID_FLAG) {

      for (int dim = 0; dim < currentlyExploringDim; ++dim){
        ss << extractConfigPararm (bestConfig, dim) << " ";
      }

    } else {

      for (int dim = 0; dim < currentlyExploringDim; ++dim){
        ss << "0 ";
      }

    }


    // Handling for currently exploring dimension. This is a very dumb
    // implementation.
    int baseValue = extractConfigPararm (GLOB_baseline, currentlyExploringDim);
    int nextValue = extractConfigPararm (nextconfiguration, currentlyExploringDim) + 1;

    /* this is in decreasing stage */
    if (nextValue <= baseValue){
      if (nextValue <= 1){
        nextValue = 0;
        currentDimDone = true;
      }
      else{
        nextValue -= 2;
      }
    }else{  // this is in increasing stage 

      if (nextValue >= GLOB_dimensioncardinality[currentlyExploringDim]) {
        if (baseValue==0){
          nextValue = GLOB_dimensioncardinality[currentlyExploringDim] - 1;
          currentDimDone = true;
        } else{
          nextValue = baseValue-1;
        }
      }
    }

    ss << nextValue << " ";


    // Fill in remaining independent params with 0.
    if (ID_FLAG) {

      for(int dim = (currentlyExploringDim+1); dim < (NUM_DIMS-NUM_DIMS_DEPENDENT); ++dim){
        ss << extractConfigPararm (GLOB_baseline, dim) << " ";
        //ss << "0 ";
      }

    } else {

      for(int dim = (currentlyExploringDim+1); dim < (NUM_DIMS-NUM_DIMS_DEPENDENT); ++dim){
        ss << extractConfigPararm (bestConfig, dim) << " ";
      }

    }
    //
    // Last NUM_DIMS_DEPENDENT3 configuration parameters are not independant.
    // They depend on one or more parameters already set. Determine the
    // remaining parameters based on already decided independant ones.
    //
    string configSoFar = ss.str();

    // Populate this object using corresponding paramters from config.
    ss << generateCacheLatencyParams (configSoFar);


    // Configuration is ready now.
    nextconfiguration = ss.str();


  /* keep generating next configuration until valid */
  while (!validateConfiguration(nextconfiguration)){
      ss.str("");

      if (ID_FLAG) {

      for (int dim = 0; dim < currentlyExploringDim; ++dim){
        ss << extractConfigPararm (bestConfig, dim) << " ";
      }

    } else {

      for (int dim = 0; dim < currentlyExploringDim; ++dim){
        ss << "0 ";
      }

    }

    if (currentDimDone) {
      nextValue = baseValue;
      all_invaild = 1;
    } else {

    nextValue = nextValue + 1;
    if (nextValue <= baseValue){
      if (nextValue <=1){
        nextValue = 0;
        currentDimDone = true;
      }
      else{
        nextValue -= 2;
      }
    }else{

      if (nextValue >= GLOB_dimensioncardinality[currentlyExploringDim]) {
        if (baseValue==0){
          nextValue = GLOB_dimensioncardinality[currentlyExploringDim] - 1;
          currentDimDone = true;
        } else{
          nextValue = baseValue -1;
        }
      }
    }
  }

    ss << nextValue << " ";

    if (ID_FLAG) {

      for(int dim = (currentlyExploringDim+1); dim < (NUM_DIMS-NUM_DIMS_DEPENDENT); ++dim){
        ss << extractConfigPararm (GLOB_baseline, dim) << " ";
        //ss << "0 ";
      }

    } else {

      for(int dim = (currentlyExploringDim+1); dim < (NUM_DIMS-NUM_DIMS_DEPENDENT); ++dim){
        ss << extractConfigPararm (bestConfig, dim) << " ";
      }

    }
    string configSoFar = ss.str();
    ss << generateCacheLatencyParams (configSoFar);
    nextconfiguration = ss.str();
    if (all_invaild){
      break;
    }
    }


    if (ID_FLAG) {
      // Make sure we start exploring next dimension in next iteration.
      if (currentDimDone) {
        currentlyExploringDim++;
        currentDimDone = false;
      }

      // Signal that DSE is complete after this configuration.
      if (currentlyExploringDim == (NUM_DIMS-NUM_DIMS_DEPENDENT)-1)
        isDSEComplete = true;
    } else {
      // Make sure we start exploring next dimension in next iteration.
      if (currentDimDone) {
        currentlyExploringDim--;
        currentDimDone = false;
      }

      // Signal that DSE is complete after this configuration.
      if (currentlyExploringDim == -1)
        isDSEComplete = true;
    }

    // Keep the following code in this function as-is.
    if (!validateConfiguration(nextconfiguration)){
      cerr << "Exiting with error; Configuration Proposal invalid: " << nextconfiguration << endl;
   //exit(-1);
    }
  }
  return nextconfiguration;
}

