#ifndef _HASP_API_H
#define _HASP_API_H

#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>

#if defined(WIN32) || defined(_WIN32) || defined(_WINDOWS_) || defined(_WINDLL) || defined(_DLL)
# include <windows.h>
#define FUNCTION_PREFIX_HASPAPI __declspec(dllexport) 
#else
# include <string.h>
# include <unistd.h>
#define FUNCTION_PREFIX_HASPAPI
#endif

#define EYEDEA_HASP_API_VERSION "1.1.0"

/* This struct must be the same as EfHaspTime, defined in EyeFaceType.h */
/*! HASP time structure. */
struct hasptime
{
    int year;             /*!< Year */
    int month;            /*!< Month */
    int day;              /*!< Day */
    int hour;             /*!< Hour */
    int minute;           /*!< Minute */
    int second;           /*!< Second */
};
typedef struct hasptime HaspTime;

/* This struct must be the same as EfHaspFeatureInfo, defined in EyeFaceType.h */
/*! HASP product feature structure. */
struct haspfeatureinfo
{
    /*! String describing the license type. Possible values are: perpetual, trial, expiration, timeperiod, executions */
    char license_type[32];
    /*! flag indicating whether the product feature is usable */
    int usable;
    /*! license activation time, only for "trial" or "timeperiod" licenses, otherwise 0 */
    HaspTime start_time;
    /*! license expiration time */
    HaspTime expiration_time;
    /*! a time period length in seconds for which the license is valid, only for "trial" or "timeperiod" licenses! */
    int total_time;
    /*! max simultaneous logins */
    int maxlogins;
    /*! number of current logins */
    int currentlogins;
    /*! feature info in xml format */
    char xml_feature_info[1024];
};
typedef struct haspfeatureinfo HaspFeatureInfo;

/* This struct must be the same as EfHaspKeyInfo, defined in EyeFaceType.h */
/*! HASP key structure. */
struct haspkeyinfo
{
    /*! hasp key id number */
    long long id;
    /*! hasp key info in xml format */
    char xml_key_info[1024];
    /*! flag indicating whether a key is a trial key or a purchased key */
    int trial;
    /*! flag indicating whether a key is present */
    int is_present;
    /*! number of features the key is allocated for */
    int num_feats;
    /*! Feature IDs */
    int * feat_ids;
    /*! Features */
    HaspFeatureInfo * feats;
};
typedef struct haspkeyinfo HaspKeyInfo;

////////////////////////////////////////////////////////////////
// Main shk-api
////////////////////////////////////////////////////////////////

/*#if defined(CPP)
extern "C" {
#endif*/

/*! \fn long long* haspGetKeyIdsFromInfo(int* piNumKeys, int product_id)
  \brief  Gets number of recognised HASP keys and their IDs
  \param  piNumKeys Pointer to int where to write number of recognised keys.
  \param product_id ID of selected product. See header constants.
  \return Pointer to allocated array of key IDs.
*/
FUNCTION_PREFIX_HASPAPI long long* haspGetKeyIdsFromInfo(int* piNumKeys, int product_id);

/*! \fn  void haspFreeKeyIds(long long* pllIds)
	\brief Frees the memory allocated via haspGetKeyIdsFromInfo
	\param pllIds pointer to long long, return value of haspGetKeyIdsFromInfo
*/
FUNCTION_PREFIX_HASPAPI void haspFreeKeyIds(long long* pllIds);

/*! \fn int haspGetHaspKeyInfo(HaspKeyInfo *pHaspKeyInfo, int product_id, long long key_id)
  \brief  Gets information of the specified key
  \param  pHaspKeyInfo Pointer to HaspKeyInfo structure where to write info data.
  \param product_id ID of product of selected key.
  \param  key_id Key ID.
  \return Zero on success.
*/
FUNCTION_PREFIX_HASPAPI int haspGetHaspKeyInfo(HaspKeyInfo *pHaspKeyInfo, long long key_id);

/*! \fn int haspGetHaspSessionKeyInfo(HaspKeyInfo *pHaspKeyInfo)
  \brief  Gets current session key info of all features
  \param  pHaspKeyInfo Pointer to HaspKeyInfo structure where to write info data.
  \return Zero on success.
*/
FUNCTION_PREFIX_HASPAPI int haspGetHaspSessionKeyInfo(HaspKeyInfo *pHaspKeyInfo);

/*! \fn int haspGetSessionFeatureInfo(int iFeatureId, HaspFeatureInfo *pFeatureInfo)
  \brief  Gets current login feature info of the specified features
  \param  iFeatureId Feature ID.
  \param  pFeatureInfo Pointer to HaspFeatureInfo structure where to write info data.
  \return Zero on success.
*/
FUNCTION_PREFIX_HASPAPI int haspGetSessionFeatureInfo(int iFeatureId, HaspFeatureInfo *pFeatureInfo);

/*! \fn int haspWriteC2VtoFile(const char* c2v_path )
  \brief  Writes c2v info of all recognized keys to separate files
  \param  c2v_path Output directory where to save the files.
  \return Zero on success.
*/
FUNCTION_PREFIX_HASPAPI int haspWriteC2VtoFile(const char * c2v_path, int product_id);

/*! \fn int haspWriteAllC2VtoOneFile(const char* filename )
  \brief  Writes c2v info of all recognized keys to one common file
  \param  filename Output filename.
  \return Zero on success.
*/
FUNCTION_PREFIX_HASPAPI int haspWriteAllC2VtoOneFile(const char * filename, int product_id);

/*! \fn int haspWriteFingerprint(const char* filename )
  \brief  Writes c2v fingerprint info of all recognized keys to one common file
  \param  filename Output filename.
  \param  writemode Standard fopen write mode ("wt" or "at")
  \return Zero on success.
*/
FUNCTION_PREFIX_HASPAPI int haspWriteFingerprint(const char * filename, const char * writemode );

/*! \fn int haspWriteRecipientInformation(const char* filename )
  \brief  Writes .id file to be used in licence rehosting scenario
  \param  filename Output filename.
  \return Zero on success.
*/
FUNCTION_PREFIX_HASPAPI int haspWriteRecipientInformation(const char* filename );

/*! \fn int haspWriteTransferLicense(const char* output_filename,const char* id_filename, long long licence_id)
  \brief  Writes v2c file with the license to be transferred in rehosting scenario
  \param  output_filename Output filename.
  \param  id_filename .id file containing recipient information, generated by haspWriteRecipientInformation on Recipient machine
  \param  licence_id the licence to be transfered
  \return Zero on success.
*/
FUNCTION_PREFIX_HASPAPI int haspWriteTransferLicense(const char* output_filename, const char* id_filename, long long licence_id);

/*! \fn int haspActivateV2C(const char* v2c_filename, const char* ack_filename )
  \brief  Activates the delivered license code.
  \param  v2c_filename The delivered license code.
  \param  ack_filename Output acknowledgement filename.
  \return Zero on success.
*/
FUNCTION_PREFIX_HASPAPI int haspActivateV2C(const char* v2c_filename,const char* ack_filename );

/*! \fn void haspPrintErrorString( FILE* fid, int iErrorCode )
  \brief  Prints the error code.
  \param  fid Pointer to error output.
  \param  iErrorCode Error code.
  \return Zero on success.
*/
FUNCTION_PREFIX_HASPAPI void haspPrintErrorString( FILE* fid, int iErrorCode );

/*! \fn int haspInitHaspKeyInfo(int * feats, int num_feats, HaspKeyInfo * hasp_key_info)
   \brief Initializes HaspKeyInfo structure before use (e.g. in haspGetHaspSessionKeyInfo())
   \param feats Array of feature ids, for which the info will be requested.
   \param num_feats Number of features in feats.
   \param hasp_key_info pointer to the preallocated structure to be filled.
   \return Zero on success.
*/
FUNCTION_PREFIX_HASPAPI int haspInitHaspKeyInfo(int * feats, int num_feats, HaspKeyInfo * hasp_key_info);

/*! \fn void haspFreeHaspKeyInfo(HaspKeyInfo * hasp_key_info)
   \brief Frees the memory allocated by haspInitHaspKeyInfo().
   \param hasp_key_info Pointer to the structure filled using haspInitHaspKeyInfo().
 */
FUNCTION_PREFIX_HASPAPI void haspFreeHaspKeyInfo(HaspKeyInfo * hasp_key_info);
/*
#ifdef CPP
}
#endif*/


#endif
