%skeleton "lalr1.cc"
%require "2.4"
%defines
%name-prefix="http"
%define "parser_class_name" "Parser"
%file-prefix="http"

%parse-param { http::Driver &driver }
%lex-param { http::Driver &driver }

%token OCTET
%token ALPHA
%token DIGIT
%token CTL
%token SP
%token HT
%token CR
%token LF

%code requires {
#include <string>
namespace http {
    class Driver;
};
}

%code {
#include "http_driver.hh"
}

%%

HTTP_MESSAGE		: 
			| REQUEST_LINE CRLF HEADERS CRLF BODY { http._type = HTTP_REQUEST; }
			| STATUS_LINE CRLF HEADERS CRLF BODY { http._type = HTTP_RESPONSE; }
			;
			
HEADERS			: 
			| HEADER CRLF HEADERS ;
BODY			: OCTETS { http._body = $1; } ;
OCTETS			:
			| STRING 
			| CTL 
			| CR 
			| LF
			;
			
STRING			: TOKEN | SP ;
TOKEN			: ALPHA 
			| DIGIT ;
			
HEADER			: TOKEN ":" STRING { http._headers[$1].push_back($3); };

REQUEST_LINE	: METHOD SP REQUEST_URI SP HTTP_VERSION { http._request_method = $1;
							  http._request_uri = $3;
							  http._request_version = $5;
							};

METHOD		: "OPTIONS"
			| "GET"
			| "HEAD"
			| "POST"
			| "PUT"
			| "DELETE"
			| "TRACE"
			| "CONNECT"
			;

CRLF		: CR LF;
HTTP_VERSION	: "HTTP/" DIGIT '.' DIGIT ;
REQUEST_URI	: "*" | STRING ;

STATUS_LINE	: HTTP_VERSION SP STATUS_CODE SP REASON { http._response_version = $1;
							  http._response_code = $3;
							  http._response_reason = $5;
							}; 
STATUS_CODE : DIGIT DIGIT DIGIT ;
REASON		: STRING ;

%%
