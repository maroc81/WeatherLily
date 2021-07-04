//******************************************************************************
//! \file jsonParser.cpp
//!
//******************************************************************************

#include <array>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>

#include "esp_log.h"
#include "jsmn.h"

#include "jsonParser.hpp"


//******************************************************************************
//! jsonParserT() -- Constructor
//
//!
//******************************************************************************
jsonParserT::jsonParserT(const std::vector<char> & jsonBufIRC) :
    jsonBufMRC(jsonBufIRC)
{

} // end jsonParserT() - Constructor


//******************************************************************************
//! Lookup()
//
//!
//******************************************************************************
const jsonElemT * jsonParserT::Lookup(const jsonElemT mapIAC[], const size_t numElemsIC, const char * pathIC) const
{
    for( int idX = 0; idX < numElemsIC; idX++ )
    {
        const jsonElemT & mapRC = mapIAC[idX];
        const size_t mapLenC = strlen(mapRC.path);
        const size_t pathLenC = strlen(pathIC);
        if(   mapLenC == pathLenC
           && strncmp(mapRC.path, pathIC, mapLenC ) == 0
          )
        {
            return( &mapRC );
        }
    }
    return( nullptr );

} // end Lookup()


//******************************************************************************
//! Lookup()
//
//!
//******************************************************************************
const jsonElemT * jsonParserT::Lookup(const jsonElemT mapIAC[], const size_t numElemsIC, const jsmntok_t & valueIRC) const
{
    for( int idX = 0; idX < numElemsIC; idX++ )
    {
        const jsonElemT & mapRC = mapIAC[idX];
        const size_t mapLenC = strlen(mapRC.path);
        const size_t pathLenC = valueIRC.end - valueIRC.start;
        if(   mapLenC == pathLenC
           && strncmp(mapRC.path, &jsonBufMRC[valueIRC.start], mapLenC ) == 0
          )
        {
            return( &mapRC );
        }
    }
    return( nullptr );

} // end Lookup()


//******************************************************************************
//! FindElem()
//
//!
//******************************************************************************
const jsonElemT * jsonParserT::FindElem(const jsonElemT mapIAC[], const size_t numElemsIC, const jsmntok_t & fieldIRC, const jsmntok_t & valueIRC)
{
    if(   fieldIRC.size == 1
       && fieldIRC.type == JSMN_STRING
       && valueIRC.size == 0
       && (   valueIRC.type == JSMN_STRING
           || valueIRC.type == JSMN_PRIMITIVE
           )
       )
   {
       return Lookup( mapIAC, numElemsIC, fieldIRC );
   }
   return( nullptr );

} // end FindElem()


//******************************************************************************
//! ParseIntoTokens()
//
//!
//******************************************************************************
int jsonParserT::ParseIntoTokens()
{
    // Clear any previous parsed tokens
    tokensMA.clear();

    // Init the parser and count number of tokens needed
    jsmn_parser parser;
    jsmn_init(&parser);
    int numTokens = jsmn_parse(&parser, jsonBufMRC.data(), jsonBufMRC.size(), nullptr, 0);

    if( numTokens > 0 )
    {
        // Resize token vector to hold number of tokens
        tokensMA.resize(numTokens);
        jsmn_init(&parser);
        numTokens = jsmn_parse(&parser, jsonBufMRC.data(), jsonBufMRC.size(), tokensMA.data(), tokensMA.size());
    }

    return numTokens;

} // end ParseIntoTokens()


//******************************************************************************
//! Parse()
//
//!
//******************************************************************************
void jsonParserT::Parse(const jsonElemT mapIAC[], const size_t numElemsIC, void * destIP)
{
    // Loop through tokens
    for( int idX = 1; idX < tokensMA.size(); idX++ )
    {
        const auto & curTokenRC = tokensMA.at(idX);
        const auto & prevTokenRC = tokensMA.at(idX - 1);

        const jsonElemT * elem = FindElem(mapIAC, numElemsIC, prevTokenRC, curTokenRC);
        if( elem != nullptr )
        {
            ProcessField(*elem, (static_cast<char*>(destIP) + elem->offset), curTokenRC);
        }
    }

} // end Parse()


//******************************************************************************
//! ParseArray()
//
//!
//******************************************************************************
void jsonParserT::ParseArray(const char * arrayNameIC, const jsonElemT mapIAC[], const size_t numElemsIC, std::vector<void *> & destElemsIR)
{
    int arrIdX = -1;
    int arrEndCharIdX = jsonBufMRC.size();
    int numArrayElems = 0;
    int numObjFields = 0;
    int arrayTokenIdX = 0;

    int curObjField = 0;

    // Loop through all the tokens or until we find a token that goes past the end of the array
    for( int idX = 2; idX < tokensMA.size() && tokensMA.at(idX).end <= arrEndCharIdX && (arrIdX < destElemsIR.size() || arrIdX < 0 ); idX++ )
    {
        const auto & curTokenRC = tokensMA.at(idX);
        const auto & prevTokenRC = tokensMA.at(idX - 1);


        if( arrIdX >= 0 )
        {
            const jsonElemT * elem = FindElem(mapIAC, numElemsIC, prevTokenRC, curTokenRC);
            if( elem != nullptr && destElemsIR.at(arrIdX) != nullptr)
            {
                char * destIP = (static_cast<char*>(destElemsIR.at(arrIdX)) + elem->offset);
                ProcessField(*elem, destIP, curTokenRC);
                numObjFields++;
            }

            if( curTokenRC.type == JSMN_OBJECT && curTokenRC.parent == arrayTokenIdX )
            {
                // Starting new object in array
                arrIdX++;
            }
        }
        else if(   curTokenRC.type == JSMN_OBJECT
                && prevTokenRC.type == JSMN_ARRAY
                && tokensMA.at(idX - 2).type == JSMN_STRING
                && CompareToken(tokensMA.at(idX - 2), arrayNameIC)
               )
        {
            // Array sequence is a string, array, then object where the string matches the array field name
            arrIdX = 0;
            arrEndCharIdX = prevTokenRC.end;
            numArrayElems = prevTokenRC.size;
            numObjFields = curTokenRC.size;

            // Save the index of the array start token (i.e. "[")
            // For each object in the array, the parent will be
            // set to this index
            arrayTokenIdX = idX - 1;
        }
    }

} // end ParseArray()


//******************************************************************************
//! ProcessField()
//
//!
//******************************************************************************
void jsonParserT::ProcessField(const jsonElemT & elemIRC, void * destIP, const jsmntok_t & valueIRC)
{
    // Copy the token into a null-terminated char buf since all
    // string to number conversions require null-terminated strings
    const size_t valueLenC = valueIRC.end - valueIRC.start + 1;
    std::vector<char> valueStr(valueLenC, 0);
    memcpy(valueStr.data(), &jsonBufMRC[valueIRC.start], valueLenC-1);

    switch( elemIRC.type )
    {
        case JT_string:
        {
            char * destCharP = static_cast<char *>(destIP);
            strncpy(destCharP, valueStr.data(), std::min(valueLenC, elemIRC.size-1));
            break;
        }
        case JT_byte:
            *(static_cast<int8_t*>(destIP)) = atoi(valueStr.data());
            break;
        case JT_short:
            *(static_cast<int16_t*>(destIP)) = atoi(valueStr.data());
            break;
        case JT_int:
            *(static_cast<int*>(destIP)) = atoi(valueStr.data());
            break;
        case JT_long:
            *(static_cast<long*>(destIP)) = atol(valueStr.data());
            break;
        case JT_float:
            *(static_cast<float*>(destIP)) = atoff(valueStr.data());
            break;
        case JT_double:
            *(static_cast<double*>(destIP)) = atof(valueStr.data());
            break;
        default:
            break;
    }

} // end ProcessField()


//******************************************************************************
//! CompareToken()
//
//!
//******************************************************************************
bool jsonParserT::CompareToken(const jsmntok_t & tokenIRC, const char * strIP)
{
    const size_t tokenLenC = tokenIRC.end - tokenIRC.start;
    const size_t strLenC = strlen(strIP);
    return(   tokenLenC == strLenC
           && strncmp(&jsonBufMRC[tokenIRC.start], strIP, tokenLenC) == 0
          );

} // end CompareToken()


//******************************************************************************
//! PrintTokens()
//
//!
//******************************************************************************
void jsonParserT::PrintTokens()
{
    char buf[128];
    int idX = 0;
    for( const auto & tokenRC : tokensMA)
    {
        if( tokenRC.type == JSMN_STRING || tokenRC.type == JSMN_PRIMITIVE )
        {
            const int lenC = std::min(127, tokenRC.end - tokenRC.start);
            strncpy(&buf[0], &jsonBufMRC[tokenRC.start], lenC);
            buf[lenC] = '\0';
        }
        else
        {
            buf[0] = '\0';
        }
        printf("Token %d, start %d, end %d, type %d, parent %d, size %d, %s\n", idX, tokenRC.start, tokenRC.end, tokenRC.type, tokenRC.parent, tokenRC.size, buf);

        idX++;
    }

} // end PrintTokens()
