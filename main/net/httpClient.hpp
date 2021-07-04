//******************************************************************************
//! \file httpClient.hpp
//!
//!
//******************************************************************************
#ifndef httpClient_hpp
#define httpClient_hpp

#include <functional>
#include <vector>

#include "esp_http_client.h"


//******************************************************************************
//! class httpClientT
//
//! Simple HTTP client wrapper for esp http client
//******************************************************************************
class httpClientT
{
        //### CONSTRUCTOR/DESTRUCTOR ###//
    public:
        //! Constructor for a client connecting to a single full URL
        httpClientT(const char * urlIC);

        //! Constructor for a client to a given host/IP and path
        httpClientT(const char * hostIC, const char * pathIC);

        ~httpClientT()
        { }


        //### ATTRIBUTES ###//
    public:


    protected:


        //### OPERATIONS ###//
    public:
        //! Performs HTTP request based on current configuration and stores response data
        //! into the given vector
        void Get(std::vector<char> & respBufIR);

        //! Performs HTTP request based on current configuration and executes the given
        //! call back for every client event.
        void Perform(std::function<void(esp_http_client_event_t *evt)> cb);

        //! Sets the http path
        void SetPath(const char * pathIC)
        {
            configM.path = pathIC;
        }

        //! Sets the http query which is appended to path
        //! <host><path>?<query>
        void SetQuery(const char * queryIC)
        {
            configM.query = queryIC;
        }


    protected:
        //! Routes c-style callback to c++ handler
        static esp_err_t HandleGlobalEvent(esp_http_client_event_t *evt);

        //! Handles http events
        esp_err_t HandleEvent(esp_http_client_event_t *evt);


        //### DATA MEMBERS ###//
    protected:
        //! The configuration for the http client
        esp_http_client_config_t configM;

        //! Pointer to the http client
        esp_http_client_handle_t clientM;

        //! Pointer to callback for http event
        std::function<void(esp_http_client_event_t *evt)> * getCbMP;

}; // end httpClientT


#endif // httpClient_hpp
