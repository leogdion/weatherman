//
//  main.c
//  weatherman
//
//  Created by Leo G Dion on 1/8/13.
//  Copyright (c) 2013 Leo Dion. All rights reserved.
//

#include <stdio.h>
#include <curl/curl.h>
#include <sqlite3.h>
#include "json.h"

static size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    printf(userdata);
    //size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    //return written;
    return size;
}

int main(int argc, const char * argv[])
{
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://api.wunderground.com/api/1128524773875b60/conditions/q/KLAN.json");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, "test");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        
#ifdef SKIP_PEER_VERIFICATION
        /*
         * If you want to connect to a site who isn't using a certificate that is
         * signed by one of the certs in the CA bundle you have, you can skip the
         * verification of the server's certificate. This makes the connection
         * A LOT LESS SECURE.
         *
         * If you have a CA cert for the server stored someplace else than in the
         * default bundle, then the CURLOPT_CAPATH option might come handy for
         * you.
         */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif
        
#ifdef SKIP_HOSTNAME_VERFICATION
        /*
         * If the site you're connecting to uses a different host name that what
         * they have mentioned in their server certificate's commonName (or
         * subjectAltName) fields, libcurl will refuse to connect. You can skip
         * this check, but this will make the connection less secure.
         */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
        
        res = curl_easy_perform(curl);
        
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return 0;
}

