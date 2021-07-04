//******************************************************************************
//! \file jsonParser.hpp
//!
//******************************************************************************
#ifndef jsonParser_hpp
#define jsonParser_hpp

#include <cstddef>
#include <vector>

#define JSMN_HEADER
#include "jsmn.h"



//******************************************************************************
//! jsonTypesE
//
//! Enum of possible data types parse from JSON
//******************************************************************************
enum jsonTypesE
{
    JT_string,
    JT_byte,
    JT_short,
    JT_int,
    JT_long,
    JT_float,
    JT_double
};


//******************************************************************************
//! jsonElemT
//
//! Struct of a JSON element that describes how to parse fields in JSON
//******************************************************************************
struct jsonElemT
{
    // The path of the json field (for now is just the name of the field)
    const char * path;

    // The offset in the struct where the parsed JSON value should be copied
    size_t offset;

    // The type of JSON field to parse
    jsonTypesE type;

    // Number of bytes
    size_t size;
};


//******************************************************************************
//! class jsonMapT
//
//!
//******************************************************************************
class jsonMapT
{
        //### CONSTRUCTOR/DESTRUCTOR ###//
    public:
        jsonMapT();

        virtual ~jsonMapT()
        { }


        //### ATTRIBUTES ###//
    public:

    const jsonElemT * Lookup(const char * fieldIC) const
    {
        for( const auto & elemRC : elemsMA )
        {
            const size_t mapLenC = strlen(elemRC.path);
            const size_t fieldLenC = strlen(fieldIC);
            if(   mapLenC == fieldLenC
               && strncmp(elemRC.path, fieldIC, fieldLenC) == 0
              )
            {
                return( &elemRC );
            }
        }
        return( nullptr );

    } // end Lookup()


    const jsonElemT * FindElem(const jsmntok_t & fieldIRC, const jsmntok_t & valueIRC, const char * fieldNameIRC)
    {
        if(   fieldIRC.size == 1
           && fieldIRC.type == JSMN_STRING
           && valueIRC.size == 2
           && (   valueIRC.type == JSMN_STRING
               || valueIRC.type == JSMN_PRIMITIVE
              )
          )
        {
            return Lookup( fieldNameIRC );
        }
        return( nullptr );
    }


    protected:


        //### OPERATIONS ###//
    public:


    protected:


        //### DATA MEMBERS ###//
    protected:
        std::vector<jsonElemT> elemsMA;


}; // end jsonMapT



//******************************************************************************
//! class jsonParserT
//
//! Simple class to parse JSON using the JSMN parser
//******************************************************************************
class jsonParserT
{
        //### CONSTRUCTOR/DESTRUCTOR ###//
    public:
        jsonParserT(const std::vector<char> & jsonBufIRC);

        ~jsonParserT()
        { }


        //### ATTRIBUTES ###//
    public:
        //! Returns an element from the json element map that matches the given path
        const jsonElemT * Lookup(const jsonElemT mapIAC[], const size_t numElemsIC, const char * pathIC) const;

        //! Returns an element from the json element map that matches the characters in the buffer
        //! specified in the given value token
        const jsonElemT * Lookup(const jsonElemT mapIAC[], const size_t numElemsIC, const jsmntok_t & valueIRC) const;

        //! Returns an element from the json element map using the given field and value tokens
        const jsonElemT * FindElem(const jsonElemT mapIAC[], const size_t numElemsIC, const jsmntok_t & fieldIRC, const jsmntok_t & valueIRC);

    protected:


        //### OPERATIONS ###//
    public:
        //! Parse JSON buffer and create tokens
        int ParseIntoTokens();

        //! Parses a JSON buffer using the given map into the given destination
        void Parse(const jsonElemT mapIAC[], const size_t numElemsIC, void * destIP);

        //! Parse
        void ParseArray(const char * arrayNameIC, const jsonElemT mapIAC[], const size_t numElemsIC, std::vector<void *> & destElemsIR);

        //! Prints tokens
        void PrintTokens();

    protected:
        //! Processes a field in the JSON
        void ProcessField(const jsonElemT & mapIRC, void * destIP, const jsmntok_t & valueIRC);

        //! Compares a token with the given string
        bool CompareToken(const jsmntok_t & tokenIRC, const char * strIP);


        //### DATA MEMBERS ###//
    protected:
        //! Vector of tokens parsed from JSON
        std::vector<jsmntok_t> tokensMA;

        //! Reference to buffer to parse
        const std::vector<char> & jsonBufMRC;

}; // end jsonParserT


#endif // jsonParser_hpp
