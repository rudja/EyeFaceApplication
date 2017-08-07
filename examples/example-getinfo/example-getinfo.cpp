////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//                                                                //
// EyeFace-SDK: Getting Sentinel License Manager license info     //
// ------------------------------------------------------------   //
//                                                                //
//    Eyedea Recognition, Ltd. (C) 2014, February 17th            //
//                                                                //
//    Contact:                                                    //
//               web: http://www.eyedea.cz                        //
//             email: info@eyedea.cz                              //
//                                                                //
//    Some of the procedures in this example may failed based on  //
//    license key types available at the client's machine.        //
//                                                                //
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

// EyeFace-SDK library selection - feature definition
#include "hasp-api.h"
#include "eyeface-hasp.h"


#if defined(WIN32) || defined(_WIN32) || defined(_WINDOWS_)
#include <windows.h>
static double myGetTimeMilisecond()
{
    return GetTickCount();
}
#elif defined(__MACH__)

#include <time.h>
#include <mach/mach.h>
#include <mach/clock.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>

/* returns time spent in user space */
double myGetTimeMilisecond()
{
    struct timespec ts;
    clock_serv_t cclock;
    mach_timespec_t mts;
    kern_return_t err_code = host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);

    if (err_code != 0)
        return -1.0;

    err_code = clock_get_time(cclock, &mts);

    if (err_code != 0)
        return -1.0;

    err_code = mach_port_deallocate(mach_task_self(), cclock);

    if (err_code != 0)
        return -1.0;

    ts.tv_sec = mts.tv_sec;
    ts.tv_nsec = mts.tv_nsec;

    return (double)((1000.*1000.*1000.) * ts.tv_sec + ts.tv_nsec) / (1000.*1000.*1000.);
}
#else
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
static double myGetTimeMilisecond()
{
   struct timespec ts;
   if (!clock_gettime(CLOCK_REALTIME, &ts))
      return (double)((1000.*1000.*1000.) * ts.tv_sec + ts.tv_nsec) / (1000.*1000.);
   return -1.0;
}
#endif

// Print the info details
static void printHaspFeatureInfo(HaspFeatureInfo feature_info)
{
    printf("  - usable         : %d\n",feature_info.usable);
    printf("  - license type   : %s\n",feature_info.license_type);
    if ((strcmp(feature_info.license_type,"trial") == 0) || (strcmp(feature_info.license_type,"timeperiod") == 0))
    {
        printf("  - start time     : %d/%d/%d\n",
            feature_info.start_time.year,
            feature_info.start_time.month,
            feature_info.start_time.day);
        printf("  - total time     : %d [s]\n",feature_info.total_time);
    }
    printf("  - expiration time: %d/%d/%d\n",
        feature_info.expiration_time.year,
        feature_info.expiration_time.month,
        feature_info.expiration_time.day);
    printf("  - max logins     : %d\n", feature_info.maxlogins);
    printf("  - current logins : %d\n", feature_info.currentlogins);
    printf("  - xml tag        : %s\n", feature_info.xml_feature_info);
}

// Print the info details
static void printHaspKeyInfo(HaspKeyInfo hasp_key_info)
{
    int i;
    
    printf("\n Key info:\n");
    printf("  - id             : %llu\n", hasp_key_info.id);
    printf("  - trial          : %d\n", hasp_key_info.trial);

    
    
    for (i=0; i < hasp_key_info.num_feats; i++)
    {
        switch (i)
        {
            case 0:
                printf("\n Face detection license info:\n");
                break;
            
            case 1:
                printf("\n Gender recognition license info:\n");
                break;
            
            case 2:
                printf("\n Age recognition license info:\n");
                break;
                
            case 3:
                printf("\n Unique person count recognition license info:\n");
                break;
        }
        
        printf("\n  Feature %d\n", hasp_key_info.feat_ids[i]);
        printHaspFeatureInfo(hasp_key_info.feats[i]);
    }
}

int main (int argc, char * argv[])
{
    HaspKeyInfo hasp_key_info_from_key_number; // must be memset to 0
    HaspKeyInfo hasp_key_info_custom; // must be memset to 0
    HaspFeatureInfo feature_info;
    double start_time, running_time;
    int i;

// list Standard licenses for EyeFace SDK
    int product_id = EYEDEA_PROD_EYEFACE_STANDARD;
    int feature_det = EYEDEA_FEAT_EF_STANDARD_DET;
    int feature_gen = EYEDEA_FEAT_EF_STANDARD_GEN;
    int feature_age = EYEDEA_FEAT_EF_STANDARD_AGE;
    int feature_upc = EYEDEA_FEAT_EF_STANDARD_UPC;

    memset(&hasp_key_info_from_key_number, 0, sizeof(HaspKeyInfo)); // initialize key info
    memset(&hasp_key_info_custom, 0, sizeof(HaspKeyInfo)); // initialize key info

    ////////////////////////////////////////////////////////////////////////////
    //
    //  Get Sentinel LDK's key info by a key ID
    //
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    //  Get the number of Sentinel LDK's Standard keys and their key IDs
    long long *pllKeyIds=NULL;
    int iNumKeys = 0;
    pllKeyIds = haspGetKeyIdsFromInfo(&iNumKeys, product_id);

    printf("Number of detected Standard keys: %d\n",iNumKeys);
    for (i=0; i<iNumKeys; i++)
    {
        printf(" %d. Hasp key id: %llu\n",i+1,pllKeyIds[i]);
    }

    printf("\n");

    // Tidy-up
    haspFreeKeyIds(pllKeyIds);

// show Expert licenses of EyeFace SDK
    product_id = EYEDEA_PROD_EYEFACE_EXPERT;
    feature_det = EYEDEA_FEAT_EF_EXPERT_DET;
    feature_gen = EYEDEA_FEAT_EF_EXPERT_GEN;
    feature_age = EYEDEA_FEAT_EF_EXPERT_AGE;
    feature_upc = EYEDEA_FEAT_EF_EXPERT_UPC;
    
    memset(&hasp_key_info_from_key_number, 0, sizeof(HaspKeyInfo)); // initialize key info
    memset(&hasp_key_info_custom, 0, sizeof(HaspKeyInfo)); // initialize key info
    
    ////////////////////////////////////////////////////////////////////////////
    //
    //  Get Sentinel LDK's key info by a key ID
    //
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    //  Get the number of Sentinel LDK's  Expert keys and their key IDs
    pllKeyIds=NULL;
    iNumKeys = 0;
    pllKeyIds = haspGetKeyIdsFromInfo(&iNumKeys, product_id);
    
    printf("Number of detected Expert keys: %d\n",iNumKeys);
    for (i=0; i<iNumKeys; i++)
    {
        printf(" %d. Hasp key id: %llu\n",i+1,pllKeyIds[i]);
    }
    
    ////////////////////////////////////////////////////////////////////////////
    //  For each key get key info (set of features present) by key ID
    
    for (i=0; i<iNumKeys; i++)
    {
        printf("\nGetting HASP key info of the key %llu... ",pllKeyIds[i]);
        start_time = myGetTimeMilisecond();
        
        // key_info is automatically allocated with all features of the given key
        if (haspGetHaspKeyInfo(&hasp_key_info_from_key_number, pllKeyIds[i]))
        {
            printf("failed.\n");
        }
        else
        {
            running_time = myGetTimeMilisecond() - start_time;
            printf("done. [time: %f ms] \n",running_time);

            // Print the info details
            printHaspKeyInfo(hasp_key_info_from_key_number);
        }
        // free allocated keyinfo
        haspFreeHaspKeyInfo(&hasp_key_info_from_key_number);
    }

    // Tidy-up
    haspFreeKeyIds(pllKeyIds);
    

    ////////////////////////////////////////////////////////////////////////////
    //
    //  Get current session feature info for a customized set of features 
    //  (Imaginary key, no key ID present)
    //
    ////////////////////////////////////////////////////////////////////////////
    int custom_feats[] = {feature_det, feature_gen, feature_age, feature_upc};
    haspInitHaspKeyInfo(custom_feats, sizeof(custom_feats)/sizeof(int), &hasp_key_info_custom);
    printf("\n\nGetting HASP login Expert key info given a key number ... \n");
    start_time = myGetTimeMilisecond();
    if (haspGetHaspSessionKeyInfo(&hasp_key_info_custom))
    {
        printf("failed.\n");
    }
    else
    {
        running_time = myGetTimeMilisecond() - start_time;
        printf("done. [time: %f ms] \n",running_time);
        // Print the info details
        printHaspKeyInfo(hasp_key_info_custom);
    }
    haspFreeHaspKeyInfo(&hasp_key_info_custom);
    
    
    ////////////////////////////////////////////////////////////////////////////
    //
    //  Getting HASP feature info
    //
    ////////////////////////////////////////////////////////////////////////////
    printf("\n\nGetting detection feature info of Expert key... \n");
    start_time = myGetTimeMilisecond();
    if ( haspGetSessionFeatureInfo(feature_det,&feature_info)!=0 ) 
    {
        printf(" failed.\n");
    }
    else
    {
        running_time = myGetTimeMilisecond() - start_time;
        printf("done. [time: %f ms] \n",running_time);
        
        
        printf("Feature info:\n");
        printHaspFeatureInfo(feature_info);
        printf("\nComplete feature info: %s\n\n",feature_info.xml_feature_info);
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    //  Writing c2v file(s) (one file per key)
    //
    ////////////////////////////////////////////////////////////////////////////
    printf("\n\nWriting c2v information of Expert keys into a file... ");
    fflush(stdout);
    if ( haspWriteC2VtoFile( "./", product_id )!=0 ) 
    {
        printf("failed.\n");
    }
    printf("done. Info saved into ./[id number].c2v file[s].\n");

    ////////////////////////////////////////////////////////////////////////////
    //
    //  Writing c2v info of all keys to one file (one file for all keys)
    //
    ////////////////////////////////////////////////////////////////////////////
    printf("\n\nWriting c2v info from all keys into one file ... ");
    fflush(stdout);
    if ( haspWriteAllC2VtoOneFile( "c2v-info.mc2v" , EYEDEA_PROD_ALL)!=0 ) 
    {
        printf("failed.\n");
        exit(-1);
    }
    printf("done. Info saved into c2v-info.mc2v file.\n");

    printf("\n\nHASP test finished!\n");
    
    printf("Finished! Press ENTER to continue...\n");
    
    getchar();
    return 0;
    
}
