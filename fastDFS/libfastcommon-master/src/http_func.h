/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

#ifndef _HTTP_FUNC_H
#define _HTTP_FUNC_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common_define.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
get content from url
params:
	url: the url to fetch, must start as: "http://"
	connect_timeout: connect timeout in seconds
	network_timeout: network timeout in seconds
	http_status: return http status code, 200 for Ok
	content: return the content (HTTP body only, not including HTTP header),
		 *content should be freed by caller when input *content is NULL for auto malloc 
	content_len: input for *content buffer size when *content is NOT NULL,
         output for content length (bytes)
return: 0 for success, != 0 for error
**/
int get_url_content_ex(const char *url, const int url_len,
        const int connect_timeout, const int network_timeout,
        int *http_status, char **content, int *content_len, char *error_info);


/**
get content from url
params:
	url: the url to fetch, must start as: "http://"
	connect_timeout: connect timeout in seconds
	network_timeout: network timeout in seconds
	http_status: return http status code, 200 for Ok
	content: return the content (HTTP body only, not including HTTP header),
		 *content should be freed by caller
	content_len: return content length (bytes)
return: 0 for success, != 0 for error
**/
int get_url_content(const char *url, const int connect_timeout, \
	const int network_timeout, int *http_status, \
	char **content, int *content_len, char *error_info);

/**
parse url
params:
	url: the url to parse, the url be modified after parse
	params: params array to store param and it's value
	max_count: max param count
return: param count
**/
int http_parse_query(char *url, KeyValuePair *params, const int max_count);

/**
parse url ex
params:
	url: the url to parse, the url be modified after parse
    url_len: the length of url
    uri_len: return the uri length (not including ? and parameters)
	params: params array to store param and it's value
	max_count: max param count
return: param count
**/
int http_parse_query_ex(char *url, const int url_len,
        int *uri_len, KeyValuePairEx *params, const int max_count);

/**
parse url params
params:
	param_str: the url params to parse, the params be modified after parse
    param_len: the length of url params
	params: params array to store param and it's value
	max_count: max param count
return: param count
**/
int http_parse_url_params(char *param_str, const int param_len,
        KeyValuePairEx *params, const int max_count);

#ifdef __cplusplus
}
#endif

#endif

